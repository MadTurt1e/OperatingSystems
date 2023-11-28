#ifndef __SPINLOCK_H
#define __SPINLOCK_H

#include <unistd.h>

//all the stuff we might need for a spinlock
struct spinlock
{
    //actual lock
    volatile char lock;

    //whatever other stuff
    pid_t holder;
    int locks;
    int unlocks;
};

void spin_lock(struct spinlock *l);
void spin_unlock(struct spinlock *l);

#endif