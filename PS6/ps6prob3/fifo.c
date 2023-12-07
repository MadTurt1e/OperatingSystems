// fifo.c stuff

// we wanna include the fifo and the spinlock.
#include "fifo.h"
#include "spinlock.h"

#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <asm-generic/mman-common.h>

#include <signal.h>

//we have to use a heccin mask
sigset_t mask;

// creates the fifo datastructure with anything it may need
void fifo_init(struct fifo *f)
{

    // Step 1: make sure we're at the start of the list
    f->readLoc = 0;
    f->writeLoc = 0;

    // Step 2: reset the number of stuff in the fifo
    f->count = 0;

    //signal stuff
    sigfillset(&mask);
    sigdelset(&mask, SIGUSR1);

    // hopefully that's it.
}

// add stuff to the fifo unless fifo is full.
void fifo_wr(struct fifo *f, unsigned long d)
{
    
    // While we're here, we want nothing to be happening
    spin_lock(&f->lock);

    // um acktually the buffer is full
    while (f->count == MYFIFO_BUFSIZ)
    {
        fprintf(stderr, "writer state: r %d w%d count%d\n", f->readLoc, f->writeLoc, f->count);
        // unlock the spinlock
        spin_unlock(&f->lock);

        // block until the array is not full
        sigsuspend(&mask);

        // okay we're done blocking: Let's see if the buffer is still full
        spin_lock(&f->lock);
    }

    // Actual critical region

    // Write, than increase the write count
    f->buffer[f->writeLoc] = d;
    // make sure to loop around if we go too far
    f->writeLoc = (f->writeLoc + 1) % MYFIFO_BUFSIZ;
    f->count++;

    // Indicate the thing is not empty anymore
    pthread_kill(gettid(), SIGUSR1);
    
    // Release the sp inlock
    spin_unlock(&f->lock);
}

// read stuff from the fifo - also happens to be the only way to clear out the fifo for writers to do stuff
unsigned long fifo_rd(struct fifo *f)
{
    int a = 0;
    // output bit
    unsigned long output;
    // Acquire the spinlock
    spin_lock(&f->lock);

    // If we don't have anything in the spinlock
    while (f->count == 0)
    {
        fprintf(stderr, "reader state: r %d w%d count%d\n", f->readLoc, f->writeLoc, f->count);
        // we release the spinlock
        
        spin_unlock(&f->lock);

        // block until the array is not empty
        sigsuspend(&mask);

        // recheck to see if we actually have something
        spin_lock(&f->lock);
    }

    // do an actual read
    output = f->buffer[f->readLoc];
    // update the read position
    f->readLoc = (f->readLoc + 1) % MYFIFO_BUFSIZ;
    f->count--;

    // signal that the thing is no longer full
    pthread_kill(gettid(), SIGUSR1);

    // Release the spinlock
    spin_unlock(&f->lock);

    // return output
    return output;
}
