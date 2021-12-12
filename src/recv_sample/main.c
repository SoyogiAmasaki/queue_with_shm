/* mqueue_mock.c                                        */
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "../queue/queue.h"

/********************************************************/
/* Macros                                               */
/********************************************************/
#define RECV_DRV_QUEUE_NAME     ( "/Queue" )
#define RECV_DRV_MSG_NUM_MAX    ( 5 )
#define RECV_DRV_MSG_SIZE_MAX   ( 16 )

/********************************************************/
/* Functions                                            */
/********************************************************/
int main( void )
{
    int fd;
    int index;
    int counter;
    ssize_t msg_len;
    char msg[100];

    queue_unlink(RECV_DRV_QUEUE_NAME);
    fd = queue_open( RECV_DRV_QUEUE_NAME, O_RDWR | O_CREAT | O_EXCL , RECV_DRV_MSG_NUM_MAX, RECV_DRV_MSG_SIZE_MAX );
    if(fd != -1)
    {
        for( counter = 0; counter < 100; counter++ )
        {
            while(1)
            {
                (void)memset(msg, 0, 100);
                msg_len = queue_receive(fd, msg);
                if( msg_len > 0 )
                {
                    for( index = 0; index < msg_len; index++ )
                    {
                        printf( "%d ", msg[index]);
                    }
                    printf("\n");
                    break;
                }
                (void)usleep(100);
            }
        }
        queue_close(fd);
        queue_unlink(RECV_DRV_QUEUE_NAME);
    }
    return 0;
}
