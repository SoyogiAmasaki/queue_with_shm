# Name
Queue with shm

# Description
In this repository, software for inter-process communication on POSIX by ring buffer is released.
Shared memory and semaphores are used for inter-process communication in this software.
This is a code created by the author for study purposes.
For inter-process communication in a queue structure on POSIX, I recommend using message queues instead of this software.

# System Requirements
The author has confirmed the operation in the following environment.
  WLS2, Ubuntu 20.04.3 LTS
  gcc version 9.3.0 (Ubuntu 9.3.0-17ubuntu1~20.04)
  GNU Make 4.2.1

# Sample Program
## build
/queue_with_shm/build/send_sample$ make all
    gcc -Wall -O2  -o ../../obj/send_sample/main.o -c ../../src/send_sample/main.c
    gcc -Wall -O2  -o ../../obj/queue/queue.o -c ../../src/queue/queue.c
    gcc -o ../../bin/send_sample ../../obj/send_sample/main.o ../../obj/queue/queue.o -lrt -pthread
/queue_with_shm/build/recv_sample$ make all
    rm -f ../../obj/recv_sample/main.o ../../obj/queue/queue.o ../../bin/recv_sample
    gcc -Wall -O2  -o ../../obj/recv_sample/main.o -c ../../src/recv_sample/main.c
    gcc -Wall -O2  -o ../../obj/queue/queue.o -c ../../src/queue/queue.c
    gcc -o ../../bin/recv_sample ../../obj/recv_sample/main.o ../../obj/queue/queue.o -lrt -pthread

## exec
1. Execute "recv_sample" in the recv terminal
/queue/queue_with_shm/bin$ ./recv_sample

2. Execute "send_sample" in the send terminal
/queue/queue_with_shm/bin$ ./send_sample

3. In the receiving terminal, the following string received from send_sample will be displayed.
0
1 1
2 2 2
3 3 3 3
4 4 4 4 4
5 5 5 5 5 5
6 6 6 6 6 6 6
7 7 7 7 7 7 7 7
8 8 8 8 8 8 8 8 8
9 9 9 9 9 9 9 9 9 9
10
11 11
...

99 99 99 99 99 99 99 99 99 99

# License
[MIT license](https://en.wikipedia.org/wiki/MIT_License).
