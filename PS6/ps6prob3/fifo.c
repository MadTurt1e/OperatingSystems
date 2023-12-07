// fifo.c stuff

// we wanna include the fifo and the spinlock.
#include "fifo.h"
#include "spinlock.h"

#include <unistd.h>
// global variable with spinlock included!
struct spinlock *lock;

// creates the fifo datastructure with anything it may need
void fifo_init(struct fifo *f)
{

    // Step 1: make sure we're at the start of the list
    f->readLoc = 0;
    f->writeLoc = 0;

    // Step 2: reset the number of stuff in the fifo
    f->count = 0;

    // Step 3: Do pthread stuff
    pthread_mutex_init(&f->mutex, NULL);
    pthread_cond_init(&f->full, NULL);
    pthread_cond_init(&f->empty, NULL);

    // hopefully that's it.
}

// add stuff to the fifo unless fifo is full.
void fifo_wr(struct fifo *f, unsigned long d)
{
    // While we're here, we want nothing to be happening
    spin_lock(lock);

    // um acktually the buffer is full
    while (f->count == MYFIFO_BUFSIZ)
    {
        // unlock the spinlock
        spin_unlock(lock);

        // block until it's not full
        pthread_cond_wait(&f->full, &f->mutex);

        // okay we're done blocking: Let's see if the buffer is still full
        spin_lock(lock);
    }

    // Actual critical region

    // Write, than increase the write count
    f->buffer[f->writeLoc] = d;
    // make sure to loop around if we go too far
    f->writeLoc = (f->writeLoc + 1) % MYFIFO_BUFSIZ;
    f->count++;

    // Indicate the thing is not empty anymore
    pthread_cond_signal(&f->empty);

    // Release the spinlock
    spin_unlock(lock);
}

// read stuff from the fifo - also happens to be the only way to clear out the fifo for writers to do stuff
unsigned long fifo_rd(struct fifo *f)
{
    // output bit
    unsigned long output;
    // Acquire the spinlock
    spin_lock(lock);

    // If we don't have anything in the spinlock
    while (f->count == 0)
    {
        // we release the spinlock
        spin_unlock(lock);

        // block until it's not empty
        pthread_cond_wait(&f->empty, &f->mutex);

        // recheck to see if we actually have something
        spin_lock(lock);
    }

    // do an actual read
    output = f->buffer[f->readLoc];
    // update the read position
    f->readLoc = (f->readLoc + 1) % MYFIFO_BUFSIZ;
    f->count--;

    // Release the spinlock
    spin_unlock(lock);

    // signal that the thing is no longer full
    pthread_cond_signal(&f->full);

    // return output
    return output;
}
