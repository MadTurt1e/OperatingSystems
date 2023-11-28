// spinlock.c
#include <sched.h>

#include "spinlock.h"
#include "tas.h"

extern int tas(volatile char *lock);

//locks a spinlock structure
void spin_lock(struct spinlock *l)
{
    //tas takes in a char pointer and only returns 0 when we break out (ie, we actually set the spinlock)
    while (tas(&(l->lock)))
    {
        //does this do something? 
        sched_yield();
    }

    //Set the spinlock to the pid of process using it
    l->holder = getpid();
    //show that we've accessed the spinlock. 
    ++(l->locks);
}

//unlocks a spin structure
void spin_unlock(struct spinlock *l)
{
    //resets the pid
    l->holder = 0;
    // for debugging perpeses
    ++(l->unlocks);

    // final operation: free the datatype.
    l->lock = 0;
}