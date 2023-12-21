#include <sys/signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int stabs = 0;

void handler(int s) {
  fprintf(stderr, "Got a signal %d, stabbed %d times.\n", s, ++stabs);
}
void report(int s) {
  exit(stabs);
}
int main(int argc, char* argv[]) {
  if (!argv[1] || !argv[2] || !argv[3]) {
    fprintf(stderr, "Did not provide signal number, assassin number, or iteration number\n");
    return 1;
  }
  int testNum = atoi(argv[2]);
  int iterNum = atoi(argv[3]);
  int* assassin = (int*) malloc(testNum * sizeof(int));

  int sigStab = atoi(argv[1]);
  signal(sigStab, handler);
  signal(SIGQUIT, report);

  int victim = fork();
  if (victim == 0) {
    while(stabs < testNum);
    exit(stabs);
  }

  int i = 0;
  for (i; i < testNum; i++) {
    assassin[i] = fork();
    if (assassin[i] == 0) {
      for (int i = 0; i < iterNum; i++) {
        kill(victim, sigStab);
      }
      exit(0);
    }
  }
  int status;
  sleep(5);
  kill(victim, SIGQUIT);
  waitpid(victim, &status, 0);
  fprintf(stderr, "Died after %d stabs.\n", WEXITSTATUS(status));
  return 0;
}