/* queue.c                                              */
/* ref : https://it-ojisan.tokyo/queue/                 */
#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h> /* stat */
#include <unistd.h> /* ftruncate */
#include <sys/types.h> /* ftruncate */
#include <string.h> /* memcpy */
#include <pthread.h>
#include <errno.h>
#include <semaphore.h>
#include "queue.h"

/********************************************************/
/* Macros                                               */
/********************************************************/
#define QUEUE_MAX_MSG_IDX     (0)
#define QUEUE_MAX_SIZE_IDX    (QUEUE_MAX_MSG_IDX    + sizeof(long))
#define QUEUE_STAT_IDX        (QUEUE_MAX_SIZE_IDX   + sizeof(long))
#define QUEUE_READ_IDX        (QUEUE_STAT_IDX       + sizeof(int))
#define QUEUE_WRITE_IDX       (QUEUE_READ_IDX       + sizeof(long))
#define QUEUE_NUM_QUEUE_INFO  (QUEUE_WRITE_IDX      + sizeof(long))

#define QUEUE_ELEM_SIZE_INFO_SIZE  (sizeof(size_t))

#define QUEUE_E_NOT_OK              (-1)
#define QUEUE_E_OK                  (0)

#define QUEUE_STAT_EMPTY      (0)
#define QUEUE_STAT_AVAILABLE  (1)
#define QUEUE_STAT_FULL       (2)

#define QUEUE_SET_QUEUE_MAX_MSG( addr, data ) \
    *(long*)(&addr[QUEUE_MAX_MSG_IDX]) = (long)data
#define QUEUE_SET_QUEUE_MAX_SIZE( addr, data ) \
    *(long*)(&addr[QUEUE_MAX_SIZE_IDX]) = (long)data
#define QUEUE_SET_READ_IDX( addr, data ) \
    *(long*)(&addr[QUEUE_READ_IDX]) = (long)data
#define QUEUE_SET_WRITE_IDX( addr, data ) \
    *(long*)(&addr[QUEUE_WRITE_IDX]) = (long)data
#define QUEUE_SET_STAT( addr, data ) \
    *(int*)(&addr[QUEUE_STAT_IDX]) = (int)data

#define QUEUE_GET_QUEUE_MAX_MSG( addr ) \
    *(long*)(&addr[QUEUE_MAX_MSG_IDX])
#define QUEUE_GET_QUEUE_MAX_SIZE( addr ) \
    *(long*)(&addr[QUEUE_MAX_SIZE_IDX])
#define QUEUE_GET_READ_IDX( addr ) \
    *(long*)(&addr[QUEUE_READ_IDX])
#define QUEUE_GET_WRITE_IDX( addr ) \
    *(long*)(&addr[QUEUE_WRITE_IDX])
#define QUEUE_GET_STAT( addr ) \
    *(int*)(&addr[QUEUE_STAT_IDX])

/********************************************************/
/* Data                                                 */
/********************************************************/
static sem_t*   queue_semaphore;

/********************************************************/
/* Function Prototypes                                  */
/********************************************************/
static int      queue_init( int fd, long queue_maxmsg, size_t queue_maxsize );
static int      queue_enqueue( unsigned char *shmPtr, const char *msg_ptr, size_t msg_len );
static ssize_t  queue_dequeue( unsigned char *shmPtr, char *msg_ptr );

/********************************************************/
/* External Functions                                   */
/********************************************************/
int queue_open( const char *name, int oflag, long queue_maxmsg, size_t queue_msgsize )
{
    int rtn;
    int fd;
    int initResult;

    rtn = QUEUE_E_NOT_OK;
    fd = shm_open( name, oflag, S_IRWXU | S_IRWXG | S_IRWXO );
    if( fd != QUEUE_E_NOT_OK )
    {
        if(( oflag & O_CREAT ) == O_CREAT )
        {
            queue_semaphore = sem_open( name, O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO, 1 );
            initResult = queue_init( fd, queue_maxmsg, queue_msgsize );
            if( initResult == QUEUE_E_OK )
            {
                rtn = fd;
            }
            else
            {
                (void)close( fd );
                (void)shm_unlink( name );
            }
        }
        else
        {
            queue_semaphore = sem_open( name, 0 );
            rtn = fd;
        }
    }

    return rtn;
}

ssize_t queue_receive( int fd, char *msg_ptr )
{
    ssize_t rtn;
    ssize_t msg_len;
    unsigned char *shmPtr;
    int fstatResult;
    int munmapResult;
    struct stat status;

    rtn = QUEUE_E_NOT_OK;
    if( fd != QUEUE_E_NOT_OK )
    {
        fstatResult = fstat( fd, &status );
        if( fstatResult != QUEUE_E_NOT_OK )
        {
            shmPtr = mmap( NULL, status.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0 );
            if( shmPtr != (unsigned char *)QUEUE_E_NOT_OK )
            {
                (void)sem_wait( queue_semaphore );
                msg_len = queue_dequeue( shmPtr, msg_ptr );
                (void)sem_post( queue_semaphore );
                munmapResult = munmap( shmPtr, status.st_size );
                if( munmapResult != QUEUE_E_NOT_OK )
                {
                    rtn = msg_len;
                }
            }
        }
    }

    return rtn;
}

int queue_send( int fd, const char *msg_ptr, size_t msg_len )
{
    int rtn;
    int sendResult;
    unsigned char *shmPtr;
    int fstatResult;
    int munmapResult;
    struct stat status;

    rtn = QUEUE_E_NOT_OK;
    if( fd != QUEUE_E_NOT_OK )
    {
        fstatResult = fstat( fd, &status );
        if( fstatResult != QUEUE_E_NOT_OK )
        {
            shmPtr = mmap( NULL, status.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0 );
            if( shmPtr != (unsigned char *)QUEUE_E_NOT_OK )
            {
                (void)sem_wait( queue_semaphore );
                sendResult = queue_enqueue( shmPtr, msg_ptr, msg_len );
                (void)sem_post( queue_semaphore );
                munmapResult = munmap( shmPtr, status.st_size );
                if( munmapResult != QUEUE_E_NOT_OK )
                {
                    rtn = sendResult;
                }
            }
        }
    }
    return rtn;
}

int queue_close( int fd )
{
    int rtn;
    rtn = QUEUE_E_NOT_OK;
    if( fd != QUEUE_E_NOT_OK )
    {
        (void)sem_close( queue_semaphore );
        rtn = close( fd );
    }

    return rtn;
}

int queue_unlink( const char *name )
{
    int rtn;
    (void)sem_unlink( name );
    rtn = shm_unlink( name );
    return rtn;
}

/********************************************************/
/* Internal Functions                                   */
/********************************************************/
static int queue_init( int fd, long queue_maxmsg, size_t queue_maxsize )
{
    int rtn;
    int ftruncateResult;
    int munmapResult;
    off_t shmSize;
    unsigned char *shmPtr;

    rtn = QUEUE_E_NOT_OK;

    /* Set size of shared memory */
    shmSize = QUEUE_NUM_QUEUE_INFO + (( queue_maxmsg + QUEUE_ELEM_SIZE_INFO_SIZE ) * queue_maxsize );
    ftruncateResult = ftruncate( fd, shmSize );
    if( ftruncateResult != QUEUE_E_NOT_OK )
    {
        /* Set values of shared memory */
        shmPtr = mmap( NULL, shmSize, PROT_WRITE, MAP_SHARED, fd, 0 );
        if( shmPtr != (unsigned char *)QUEUE_E_NOT_OK )
        {
            QUEUE_SET_QUEUE_MAX_MSG( shmPtr, queue_maxmsg );
            QUEUE_SET_QUEUE_MAX_SIZE( shmPtr, queue_maxsize );
            QUEUE_SET_READ_IDX( shmPtr, 0 );
            QUEUE_SET_WRITE_IDX( shmPtr, 0 );
            QUEUE_SET_STAT( shmPtr, QUEUE_STAT_EMPTY );
            munmapResult = munmap( shmPtr, shmSize );
            if( munmapResult != QUEUE_E_NOT_OK )
            {
                rtn = QUEUE_E_OK;
            }
        }
    }
    return rtn;
}

static int queue_enqueue( unsigned char *shmPtr, const char *msg_ptr, size_t msg_len )
{
    int rtn;
    int queueStat;
    long queueMaxMsg;
    long queueMaxSize;
    long postQueueStat;
    long readIdx;
    long writeIdx;
    long postWriteIdx;
    long queueIdx;

    rtn = QUEUE_E_NOT_OK;
    queueStat = QUEUE_GET_STAT( shmPtr );
    if( queueStat != QUEUE_STAT_FULL )
    {
        /* Get information data from shared memory. */
        queueMaxMsg = QUEUE_GET_QUEUE_MAX_MSG( shmPtr );
        queueMaxSize = QUEUE_GET_QUEUE_MAX_SIZE( shmPtr );
        writeIdx = QUEUE_GET_WRITE_IDX( shmPtr );
        readIdx = QUEUE_GET_READ_IDX( shmPtr );

        /* Set data to queue. */
        queueIdx = QUEUE_NUM_QUEUE_INFO
                 + (( QUEUE_ELEM_SIZE_INFO_SIZE + queueMaxSize )
                   * writeIdx );
        *(size_t*)(&shmPtr[queueIdx]) = msg_len;
        memcpy( &shmPtr[queueIdx + QUEUE_ELEM_SIZE_INFO_SIZE], msg_ptr, msg_len );

        /* Update indexes. */
        if( writeIdx == queueMaxMsg - 1 )
        {
            postWriteIdx = 0;
        }
        else
        {
            postWriteIdx = writeIdx + 1;
        }

        /* Update a status of a queue. */
        if( readIdx == postWriteIdx )
        {
            postQueueStat = QUEUE_STAT_FULL;
        }
        else
        {
            postQueueStat = QUEUE_STAT_AVAILABLE;
        }
        QUEUE_SET_WRITE_IDX( shmPtr, postWriteIdx );
        QUEUE_SET_STAT( shmPtr, postQueueStat );
        rtn = QUEUE_E_OK;
    }

    return rtn;
}

static ssize_t queue_dequeue( unsigned char *shmPtr, char *msg_ptr )
{
    ssize_t rtn;
    long queueMaxMsg;
    long queueMaxSize;
    int queueStat;
    long postQueueStat;
    long readIdx;
    long writeIdx;
    long postReadIdx;
    long queueIdx;

    rtn = QUEUE_E_NOT_OK;
    queueStat = QUEUE_GET_STAT( shmPtr );

    if( queueStat != QUEUE_STAT_EMPTY )
    {
        /* Get information data from shared memory. */
        queueMaxMsg = QUEUE_GET_QUEUE_MAX_MSG( shmPtr );
        queueMaxSize = QUEUE_GET_QUEUE_MAX_SIZE( shmPtr );
        writeIdx = QUEUE_GET_WRITE_IDX( shmPtr );
        readIdx = QUEUE_GET_READ_IDX( shmPtr );

        /* Get data from shared memory. */
        queueIdx = QUEUE_NUM_QUEUE_INFO
                 + (( QUEUE_ELEM_SIZE_INFO_SIZE + queueMaxSize )
                   * readIdx );
        rtn = *(size_t*)(&shmPtr[queueIdx]);
        memcpy( msg_ptr, &shmPtr[queueIdx + QUEUE_ELEM_SIZE_INFO_SIZE], rtn );

        /* Update indexes. */
        if( readIdx == queueMaxMsg - 1 )
        {
            postReadIdx = 0;
        }
        else
        {
            postReadIdx = readIdx + 1;
        }

        /* Update a status of a queue. */
        if( writeIdx == postReadIdx )
        {
            postQueueStat = QUEUE_STAT_EMPTY;
        }
        else
        {
            postQueueStat = QUEUE_STAT_AVAILABLE;
        }
        QUEUE_SET_READ_IDX( shmPtr, postReadIdx );
        QUEUE_SET_STAT( shmPtr, postQueueStat );
    }

    return rtn;
}

/* End of file */
