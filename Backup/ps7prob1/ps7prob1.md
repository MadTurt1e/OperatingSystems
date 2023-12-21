**Problem 1 -- Short Answer**
1A) In the kernel’s system call entrypoint code in assembly, we see two instructions:
```S
cmpl $nr_syscalls,%eax
jae syscall_badsys
```
If these two instructions were not in the code, explain why this would be a very serious security hole.

The first line compares the system call number to the %eax register, and jae jumps if zero.  

This would be important because if we went beyond the number of possible system calls, we could be going out of the bounds of the system call, which would not be ideal for obvious reasons. However the main issue comes from the second fact: if we were able to get control of the %eax register, (without these two lines) we could run any code we want which could lead to arbituary code execution with higher permissions - not ideal for a kernel to contain.  

1B) If an interrupt is received by a CPU running in X86-32 mode and is not being masked, what determines whether
or not the %esp register gets fetched from the TSS?

The TSS is the Task State Segment. If an interrupt is recieved by a CPU running in x86-32 mode without being masked, the mode the processor is currently in determines whether or not the %esp register gets fetched. If it is in "supervisor" mode upon inturrupt already, the stack pointer is already in a valid kernel stack, so there's no reason to perform a fetch operation. Otherwise, we would have to do so to get the %esp register in a valid kernel stack.  

1C) Just before the kernel hands over to user mode during boot-up, it mounts the root filesystem. What happens if
that fails, e.g. the disk containing the root filesystem has a fatal error. {This is a research question, the answer is not
in the lecture notes directly}

At this point, most of the kernel is loaded in, inturrupts are enabled, and has no usermode processes. Because of this, we should probably expect the kernel to enter "Emergency Mode" where barebones actions can be performed to try to fix this issue. The solution is probably distribution dependent - in some instances we may face a "Kernel Panic" instead which causes an error message to pop up and the computer to restart.  

1D) Describe three situations where the TIF_NEED_RESCHED flag would be turned on and cause a deferred return
from a system call, interrupt, and/or fault handler.  

This is the case where a task needs to be rescheduled. This flag can be turned on when the scheduler determines that the task which is currently running is not the best use of the CPU's time at the present moment - a result from a variety of reasons:  
1. Tick inturrupt handler. The process has used up its allocated "timeslice" (running for too long, perhaps) and must be rescheduled. 
2. Task priority. Another process has emerged from the shadows with higher priority, so the currently running task is not as prioritized and can be rescheduled. 
3. Task Wakeup. Another process has just woken up, and must be run as soon as possible. Thus, the currently running task can also be rescheduled in this case. 