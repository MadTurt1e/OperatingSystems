/********************************************
 * launcher.c          ECE357
 * wombo combo of all 3 programs
 * *****************************************/

#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  int pipe1[2], pipe2[2];
  if (pipe(pipe1) == -1 || pipe(pipe2) == -1) {
    perror("pipe");
    return 1;
  }

  //fork
  pid_t pid = fork();
  if (pid == -1) {
    perror("fork");
    return 1;
  }

  // Hire a child to generate however many words we said to and output to the pipe
  if (pid == 0) {  
    dup2(pipe1[1], STDOUT_FILENO);
    close(pipe1[0]);
    close(pipe1[1]);
    close(pipe2[0]);
    close(pipe2[1]);
    execl("./wordgen", "./wordgen", argc > 1 ? argv[1] : NULL, NULL);
    perror("execl");
    return 1;
  }

  pid = fork();
  if (pid == -1) {
    perror("fork");
    return 1;
  }

  // Hire another child to search through the generated words with the dictionary and read from the pipe
  if (pid == 0) {
    dup2(pipe1[0], STDIN_FILENO);
    dup2(pipe2[1], STDOUT_FILENO);
    close(pipe1[0]);
    close(pipe1[1]);
    close(pipe2[0]);
    close(pipe2[1]);
    execl("./wordsearch", "./wordsearch", "words.txt", NULL);
    perror("execl");
    return 1;
  }

  pid = fork();
  if (pid == -1) {
    perror("fork");
    return 1;
  }

  //Hire another child to page through our outputs
  if (pid == 0) {
    dup2(pipe2[0], STDIN_FILENO);
    close(pipe1[0]);
    close(pipe1[1]);
    close(pipe2[0]);
    close(pipe2[1]);
    execl("./pager", "./pager", NULL);
    perror("execl");
    return 1;
  }

  close(pipe1[0]);
  close(pipe1[1]);
  close(pipe2[0]);
  close(pipe2[1]);

  //output exit status whenever the thing finishes
  int status;
  while ((pid = wait(&status)) != -1) {
    printf("Child %d exited with %d\n", pid, WEXITSTATUS(status));
  }

  return 0;
}