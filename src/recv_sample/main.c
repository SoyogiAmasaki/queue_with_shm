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
    int qopen_result;
    int index;
    int counter;
    ssize_t msg_len;
    char msg[100];
    qd_t qd;

    queue_unlink( RECV_DRV_QUEUE_NAME );
    qopen_result = queue_open( RECV_DRV_QUEUE_NAME, O_RDWR | O_CREAT | O_EXCL , RECV_DRV_MSG_NUM_MAX, RECV_DRV_MSG_SIZE_MAX, &qd );
    if( qopen_result != -1 )
    {
        for( counter = 0; counter < 100; counter++ )
        {
            while(1)
            {
                (void)memset( msg, 0, 100 );
                msg_len = queue_receive( &qd, msg );
                if( msg_len > 0 )
                {
                    for( index = 0; index < msg_len; index++ )
                    {
                        printf( "%d ", msg[index]);
                    }
                    printf("\n");
                    break;
                }
                (void)usleep( 100 );
            }
        }
        queue_close( &qd );
        queue_unlink( RECV_DRV_QUEUE_NAME );
    }
    return 0;
}
