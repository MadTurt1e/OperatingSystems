#include <sys/signal.h>
#include <stdlib.h>
#include <stdio.h>
void handler(int s) {
  fprintf(stderr, "Got a signal %d\n", s);
  *(int*)0=0;
}
int main() {
  signal(SIGSEGV,handler);
  fprintf(stderr, "Having a nice day, so far\n");
  *(int*) 0=3;
  fprintf(stderr, "Goodbye!\n");
  return 0;
}