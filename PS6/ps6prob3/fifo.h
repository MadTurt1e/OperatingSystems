#ifndef __FIFO_H
#define __FIFO_H

// Necessary(?) defines
#define MYFIFO_BUFSIZ 1024

// Max virtual processors
#define NPROC 64

#include <pthread.h>

//fifo is a struct
struct fifo
{
    // buffer for the number of allowable fifo elements
    unsigned long buffer[MYFIFO_BUFSIZ];

    //track next write slot and the next read slot
    int readLoc;
    int writeLoc;

    //some mutex stuff
    pthread_cond_t full, empty;
    pthread_mutex_t mutex;

    //we wanna be tracking the number of stuff in the fifo
    int count;
};

/* Initialize the shared memory FIFO *f including any required underlying
 * initializations (such as calling cv_init). The FIFO will have a static
 * fifo length of MYFIFO_BUFSIZ elements. #define this in fifo.h.
 * A value of 1K is reasonable. In most cases, simply setting the
 * entire struct fifo to 0 will suffice as initialization.
 */
void fifo_init(struct fifo *f);

/* Enqueue the data word d into the FIFO, blocking unless and until the
 * FIFO has room to accept it. (i.e. block until !full)
 * Wake up a reader which was waiting for the FIFO to be non-empty
 */
void fifo_wr(struct fifo *f, unsigned long d);

/* Dequeue the next data word from the FIFO and return it. Block unless
 * and until there are available words. (i.e. block until !empty)
 * Wake up a writer which was waiting for the FIFO to be non-full
 */
unsigned long fifo_rd(struct fifo *f);

#endif