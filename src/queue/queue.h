/* queue.h                                              */
#ifndef __QUEUE_H__
#define __QUEUE_H__

#include <sys/types.h> /* For size_t and ssize_t */
#include <sys/fcntl.h> /* For O_x macros */
#include <time.h> /* For timespeck */

/********************************************************/
/* Functions                                            */
/********************************************************/
int queue_open( const char *name, int oflag, long queue_maxmsg, size_t queue_msgsize );
ssize_t queue_receive( int fd, char *msg_ptr );
int queue_send( int fd, const char *msg_ptr, size_t msg_len );
int queue_close( int fd );
int queue_unlink( const char *name );

#endif /* __QUEUE_H__ */
/* End of file */
