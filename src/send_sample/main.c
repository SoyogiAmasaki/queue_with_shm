/* mqueue_mock.c                                        */
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "../queue/queue.h"

/********************************************************/
/* Macros                                               */
/********************************************************/
#define RECV_DRV_QUEUE_NAME     ( "/Queue" )

/********************************************************/
/* Functions                                            */
/********************************************************/
int main( void )
{
    int qopen_result;
    int counter;
    int sendResult;
    char msg[100];
    qd_t qd;

    qopen_result = queue_open( RECV_DRV_QUEUE_NAME, O_RDWR, 0, 0, &qd );

    if( qopen_result != -1 )
    {
        for( counter = 0; counter < 100; counter++ )
        {
            while(1)
            {
                (void)memset( msg, counter, 100 );
                sendResult = queue_send( &qd, msg, ( counter % 10 ) + 1 );
                if( sendResult != -1 )
                {
                    break;
                }
                (void)usleep( 10 );
            }
        }

        queue_close( &qd );
    }
    return 0;
}
