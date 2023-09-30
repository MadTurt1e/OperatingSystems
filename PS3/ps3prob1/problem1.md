# Problem 1

## Shell Script Invocation
The following is a "screen shot" of a terminal session. Commands that I typed are shown in bold, with output in plain typeface.
<pre>
    <b>$ cat script.sh</b>
    #!/bin/sh
    ls -l $*
    <b>$ ls -l</b>
    -rw-r--r-- 1 user1 students 1001 Sep 25 12:10 f1.c
    -rw-r--r-- 1 user1 students 4573 Sep 25 12:10 f1.out
    -rw-r--r-- 1 user1 students 9832 Sep 25 12:10 f2.c
    -rw-r--r-- 1 user1 students 9799 Sep 25 12:10 f2.out
    -rw-r--r-- 1 user1 students 9161 Sep 25 12:10 f3.c
    -rw-r--r-- 1 user1 students 9425 Sep 25 12:10 f3.out
    -rw-r--r-- 1 user1 students 1792 Sep 25 12:10 f4.c
    -rw-r--r-- 1 user1 students 9137 Sep 25 12:10 f5.c
    -rwxr-xr-x 1 user1 students 20 Sep 25 12:10 script.sh
    <b>$ ./script.sh f[2-4].c</b>
</pre>
As you can see, I invoked a shell script from my interactive terminal shell session. For argument’s sake, let’s say the
pid of my interactive shell is 123.

1. What is the full pathname of the binary which the child of pid 123 exec’s?

The binary which the child of PID 123 execs is `/bin/sh`. <br>This is because the shell script `script.sh` starts with `#!/bin/sh`, which means the script should be run using the `sh` shell (located there).

2. What is argc and argv for that binary?

`argc` would be 2 - this is the amount of arguments passed to the script. The actual arguments are below. <br>
`argv` are the "strings" in the C sense, which are `./script.sh` (`argv[0]`), and "`f[2-4].c`" (`argv[1]`). 

3. What does pid 123 do after the fork?

After the fork, pid 123 (the parent process) waits for the child process to finish execution. This is typically done using a system call like wait(). 

4. Now let’s say we changed the invocation of the shell script to <br>
`./script.sh foobar`<br>
How would we get the parent shell to report the exit status of the child? Is that status zero or non-zero, and why?

If you changed the invocation of the shell script to `./script.sh foobar`, we can get the parent shell to report the exit status of the child by using `$?` in bash. We can print it using `echo $?`<br>
This variable holds the exit status of the last command that was executed (the child's exit status, in this case). <br>
If foobar does not exist in your directory, then the exit status would be non-zero because ls -l $* would fail to find a file or directory named foobar. <br>
If foobar does exist, then the exit status would be zero, as 0 is a success. 