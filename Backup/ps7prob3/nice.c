//nice.c
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/wait.h>

pid_t* children;
int numChild;

// kill all children
void filicide(int signum) {
  //kill all the children -pid number whenever we wake up from alarm
  kill(-children[0], SIGKILL);

  struct rusage usage;
  for (int i = 0; i < numChild; i++) {
    // collect zombie children and print out their info
    wait4(children[i], NULL, 0, &usage);
    printf("Child %d: %ld.%06ld seconds\n", children[i], usage.ru_utime.tv_sec, usage.ru_utime.tv_usec);
  }
}

int main(int argc, char* argv[]) {
  // quick arg check
  if (argc != 4) {
    fprintf(stderr, "Error: wrong arguments\nUsage: ./nice [number of children] [niceness] [waittime]\n");
    exit(0);
  }

  // data to use later
  numChild = atoi(argv[1]);

  children = malloc(numChild * sizeof(pid_t));

  // signal handler to kill children
  signal(SIGALRM, filicide);
  alarm(atoi(argv[3]));

  // repeat for all children
  for (int i = 0; i < numChild; i++) {
    pid_t pid = fork();
    switch (pid) {
      case -1: // oh no
        fprintf(stderr, "Error: %s\nTrying to fork.\n", strerror(errno));
      case 0: //child case
        while(1);
      default: //parent case
        //keep track of child pid
        children[i] = pid;
        // most likely because we don't have permission
        if (i == 0) {
          if (setpriority(PRIO_PROCESS, pid, atoi(argv[2])) == -1) {
            fprintf(stderr, "Error: %s\nTrying to set niceness of process %d.\n", strerror(errno), pid);
            exit(0);
          }
        }

        //nice or naughty kid?
        setpgid(pid, children[0]);
    }
  }
  //we sleep until argv[3] o'clock in the morning. 
  pause();
}