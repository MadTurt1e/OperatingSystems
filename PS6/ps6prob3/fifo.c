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

#include <pthread.h>

#include <asm-generic/mman-common.h>

void sig_handler(int signum)
{
    fprintf(stderr, "Signal recieved\n");
    return;
}
// creates the fifo datastructure with anything it may need
void fifo_init(struct fifo *f)
{

    // Step 1: make sure we're at the start of the list
    f->readLoc = 0;
    f->writeLoc = 0;

    // Step 2: reset the number of stuff in the fifo
    f->count = 0;

    // step 4: heccin signals
    sigfillset(f->mask);
    sigdelset(f->mask, SIGUSR1);

    //block all sigusr1 signals normally
    sigset_t normalMask;
    sigemptyset(&normalMask);
    sigprocmask(SIG_SETMASK, &normalMask, NULL);

    // step 5: heccin queues
    f->queueLoc = 0;
    f->dequeueLoc = 0;

    // hopefully that's it.
}

// add stuff to the fifo unless fifo is full.
void fifo_wr(struct fifo *f, unsigned long d)
{
    signal(SIGUSR1, sig_handler);

    int queue;
    // While we're here, we want nothing to be happening
    spin_lock(&f->lock);

    // um acktually the buffer is full
    while (f->count == MYFIFO_BUFSIZ)
    {

        fprintf(stderr, "writer state: r %d w%d count%d\n", f->readLoc, f->writeLoc, f->count);

        // add something to the queue
        spin_lock(&f->queuelock);
        f->full = true;
        while (f->full)
        {
            f->queue[f->queueLoc] = getpid();
            f->queueLoc = (f->writeLoc + 1) % NPROC;
            spin_unlock(&f->lock);
            spin_unlock(&f->queuelock);

            sigsuspend(f->mask);
        }

        // we release the spinlock
        spin_unlock(&f->queuelock);

        // okay we're done blocking: Let's see if the buffer is still full
        spin_lock(&f->lock);
        fprintf(stderr, "W: buffer bypassed\n");
    }

    
    // Actual critical region

    // Write, than increase the write count
    f->buffer[f->writeLoc] = d;

    // make sure to loop around if we go too far
    f->writeLoc = (f->writeLoc + 1) % MYFIFO_BUFSIZ;
    f->count++;
    f->empty = false;

    // signal that the thing is no longer full
    if (f->queueLoc != f->dequeueLoc)
    {
        kill(f->queue[f->dequeueLoc], SIGUSR1);
        fprintf(stderr, "W: signal sent to %d\n", f->queue[f->dequeueLoc]);
        f->queueLoc = (f->dequeueLoc + 1) % NPROC;
    }

    // Release the spinlock
    spin_unlock(&f->lock);
}

// read stuff from the fifo - also happens to be the only way to clear out the fifo for writers to do stuff
unsigned long fifo_rd(struct fifo *f)
{
    fprintf(stderr, "start state: r %d w%d count%d\n", f->readLoc, f->writeLoc, f->count);
    // output bit
    unsigned long output;
    // Acquire the spinlock
    spin_lock(&f->lock);
    fprintf(stderr, "R: spinlock bypassed\n");
    // If we don't have anything in the spinlock
    while (f->count == 0)
    {
        fprintf(stderr, "reader state: r %d w%d count%d\n", f->readLoc, f->writeLoc, f->count);

        // add something to the queue
        spin_lock(&f->queuelock);
        f->empty = true;
        fprintf(stderr, "R: queue spinlock bypassed\n");
        while (f->empty)
        {
            f->queue[f->queueLoc] = getpid();
            f->queueLoc = (f->queueLoc + 1) % NPROC;
            spin_unlock(&f->lock);
            spin_unlock(&f->queuelock);

            sigsuspend(f->mask);
            spin_lock(&f->queuelock);
        }

        fprintf(stderr, "R: signals bypassed\n");

        // we release the spinlock
        spin_unlock(&f->queuelock);
        spin_unlock(&f->lock);

        // recheck to see if we actually have something
        spin_lock(&f->lock);
    }

    fprintf(stderr, "R: count check success\n");
    // do an actual read
    output = f->buffer[f->readLoc];
    // update the read position
    f->readLoc = (f->readLoc + 1) % MYFIFO_BUFSIZ;
    f->count--;
    f->full = false;

    spin_lock(&f->queuelock);
    // this is a check to see of the queue has something inside
    if (f->queueLoc != f->dequeueLoc)
    {
        kill(f->queue[f->dequeueLoc], SIGUSR1);
        fprintf(stderr, "R: signal sent to %d\n", f->queue[f->dequeueLoc]);
        f->queueLoc = (f->dequeueLoc + 1) % NPROC;

    }
    spin_unlock(&f->queuelock);

    // Release the spinlock
    spin_unlock(&f->lock);

    // return output
    return output;
}
