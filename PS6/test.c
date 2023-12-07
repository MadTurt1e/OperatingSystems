#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <asm-generic/mman-common.h>

#include "spinlock.h"

// data type we're testing
struct shared
{
    struct spinlock lock;
    int count;
};

// the meat and potatoes, without meat or potatoes
int main(int argc, char *argv[])
{
    // quick check to make sure we actually put down a good item.
    if (argc != 3)
    {
        printf("\e[0;31mUsage: %s [number of children] [repitition times]\n\e[0m", argv[0]);
        return 1;
    }

    // atoi is going to give something good.
    int childQuant = atoi(argv[1]);
    int repQuant = atoi(argv[2]);

    // someone say mmap?
    struct shared *shared = mmap(NULL, sizeof(struct shared), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);

    // do however many times we need to.
    for (int i = 0; i < childQuant; i++)
    {
        // child case
        switch (fork())
        {
        //child case
        case 0:
            for (int j = 0; j < repQuant; j++)
            {
                spin_lock(&shared->lock);
                shared->count++;
                spin_unlock(&shared->lock);
            }
            exit(0);
        //we messed up case
        case -1:
            perror("Fork Error");
            exit(1);
        //parent case
        default:
            continue;
        }
    }

    // Wait for all children to finish
    for (int i = 0; i < childQuant; i++)
    {
        wait(NULL);
    }

   
    printf("Final count: %d\n", shared->count);
    printf("Expected count: %d\n", childQuant * repQuant);

    // Repeat this for the case we don't use spin locks
    shared->count = 0;
    // do however many times we need to.
    for (int i = 0; i < childQuant; i++)
    {
        // child case
        switch (fork())
        {
        // child case
        case 0:
            for (int j = 0; j < repQuant; j++)
            {
                shared->count++;
            }
            exit(0);
        // we messed up case
        case -1:
            perror("Fork Error");
            exit(1);
        // parent case
        default:
            continue;
        }
    }

    // Wait for all children to finish
    for (int i = 0; i < childQuant; i++)
    {
        wait(NULL);
    }

    // Print out the final count, and the expected count, I guess.
    printf("Bad Case\nFinal count: %d\n", shared->count);
    printf("Expected count: %d\n", childQuant * repQuant);

    return 0;
}