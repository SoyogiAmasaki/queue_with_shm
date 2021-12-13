/* queue.h                                              */
#ifndef __QUEUE_H__
#define __QUEUE_H__

#include <sys/types.h> /* For size_t and ssize_t */
#include <sys/fcntl.h> /* For O_x macros */
#include <time.h> /* For timespeck */
#include <semaphore.h> /* For semaphore */

/********************************************************/
/* Types                                                */
/********************************************************/
typedef struct qd_st
{
    int     fd;
    sem_t*  sem;
} qd_t;


/********************************************************/
/* Functions                                            */
/********************************************************/
int queue_open( const char *name, int oflag, long queue_maxmsg, size_t queue_msgsize, qd_t *qd );
ssize_t queue_receive( const qd_t *qd, char *msg_ptr );
int queue_send( const qd_t *qd, const char *msg_ptr, size_t msg_len );
int queue_close( qd_t *qd );
int queue_unlink( const char *name );

#endif /* __QUEUE_H__ */
/* End of file */
