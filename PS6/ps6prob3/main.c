// All the files we're going to need
#include "fifo.h"
#include "spinlock.h"

// includes
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <asm-generic/mman-common.h>

// suggested amounts
int writerCount = 8;
unsigned long iterations = 32768;

// data block that will be shared
struct fifo *shared_fifo;

// Every writer will just write a string of numbers from 1 to the num of iterations
void writer(int id)
{
    // just iterate through everything
    for (unsigned long i = 0; i < iterations; ++i)
    {
        // write to the thing - this math makes sure it is pseudo sequential (or at least we can trace it back)
        fifo_wr(shared_fifo, (id * iterations) + i);
    }

    //debug message
    fprintf(stderr, "Writer %d completed", id);
}

// reads out the fifo until it's empty
void reader(int id)
{
    unsigned long received = 0;
    int writer;

    for (unsigned long i = 0; i < iterations; ++i)
    {
        // read from the fifo
        received = fifo_rd(shared_fifo);

        // figure out who the writer is
        writer = i / iterations;

        // whatever we get better be what we expect
        if (received != (writer * iterations) + i)
        {
            fprintf(stderr, "Read problem: Writer %d out of sequence. Expected %lu, got %lu\n", id, i, received);
        }
    }

    // getting here is the good case
    fprintf(stderr, "Reader stream %d completed.\n", id);
}

int main()
{
    // Initialize shared memory for struct fifo
    shared_fifo = mmap(NULL, sizeof(struct fifo), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    fifo_init(shared_fifo);

    // debug message
    fprintf(stderr, "Beginning test with %d writers, %lu items each\n", writerCount, iterations);

    // Create writer processes
    for (int i = 0; i < writerCount; ++i)
    {
        // fork as many times as necessary
        switch (fork())
        {
        // child case
        case 0:
            writer(i);
            exit(0);
        // messed up case
        case -1:
            perror("Fork Error");
            exit(1);
        // parent case
        default:
            continue;
        }
    }

    //read immediately after we finish forking everything
    for (int i = 0; i < writerCount; ++i)
    {
        // fork as many times as necessary
        switch (fork())
        {
        // child case
        case 0:
            reader(i);
            exit(0);
        // messed up case
        case -1:
            perror("Fork Error");
            exit(1);
        // parent case
        default:
            continue;
        }
    }

    fprintf(stderr, "All streams done\n");

    fprintf(stderr, "Waiting for writer children to die\n");
    return 0;
}