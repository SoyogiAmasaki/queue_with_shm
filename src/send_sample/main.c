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
    int fd;
    int counter;
    int sendResult;
    char msg[100];

    fd = queue_open( RECV_DRV_QUEUE_NAME, O_RDWR, 0, 0 );

    if(fd != -1)
    {
        for( counter = 0; counter < 100; counter++ )
        {
            while(1)
            {
                (void)memset(msg, counter, 100);
                sendResult = queue_send(fd, msg, ( counter % 10 ) + 1);
                if( sendResult != -1 )
                {
                    break;
                }
                (void)usleep(50);
            }
        }

        queue_close(fd);
    }
    return 0;
}
