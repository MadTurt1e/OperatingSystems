#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/resource.h>
#include <time.h>

int lineReader(char line[]);
int execcmd(int argLoc, char* args[]);

int cdcmd(char* args[]);
int pwdcmd();
int exitcmd(char* args[]);
int arbitcmd(int argLoc, char* args[]);

int redirect(char* arg);

int exitCode;
FILE* iFile;


int main(int argc, char* argv[])
{
  // This should go line by line
  char line[4096];
  // argv[1] should be the name of the file we are reading
  if (argv[1])
  {
    iFile = fopen(argv[1], "r");

    if (!iFile)
    {
      fprintf(stderr, "\e[0;31mOpening file %s for read. Error: %s\n\e[0m", argv[1], strerror(errno));
      exit(-1);
    }

    while (fgets(line, sizeof(line), iFile) != NULL)
    {
      // we leave this as a job for another function.
      exitCode = lineReader(line);
    }

    fclose(iFile);
  }
  // we continue looking at stdin
  while (fgets(line, sizeof(line), stdin) != NULL)
  {
    // we leave this as a job for another function.
    exitCode = lineReader(line);
  }

  char* exitSequence[2];
  exitcmd(exitSequence);
}

// reads something line by line, and does what's on the tin.
int lineReader(char line[])
{
  if (line[0] == '#' || line[0] == '\n')
  {
    return 0;
  }
  line[strlen(line) - 1] = '\0';
  char* args[4096];
  //"tokenize" string using space as the delimiter
  char* tok = strtok(line, " ");
  // go through the string, until we can't anymore, and add each value to the args array
  int argLoc = 0;

  while (tok != NULL)
  {
    if (strcmp(tok, "$?") == 0) {
      char str[20];
      sprintf(str, "%d", exitCode);
      args[argLoc++] = str;
    } else {
      args[argLoc++] = tok;
    }
    tok = strtok(NULL, " ");
  }
  // set the final value of args to null
  args[argLoc] = NULL;

  // execute commands
  return execcmd(argLoc, args);
}

int execcmd(int argLoc, char* args[])
{
  // case: nothing is there
  if (args[0] == NULL)
  {
    return 0;
  }
  // cd implmentation
  if (!strcmp(args[0], "cd"))
  {
    return cdcmd(args);
  }
  // pwd implementation
  if (!strcmp(args[0], "pwd"))
  {
    return pwdcmd();
  }
  // exit implementation.
  if (!strcmp(args[0], "exit"))
  {
    exitcmd(args);
  }
  // case of unidentified command
  else
  {
    //function call because it looks nicer
    return arbitcmd(argLoc, args);
  }
  // uhh...
  return 1;
}

// change directory. return 0 on success, 1 on fail.
int cdcmd(char* args[])
{
  //case for no specified path
  if (args[1] == NULL){
    if (chdir(getenv("HOME")) == -1){
      fprintf(stderr, "\e[0;31mError executing chdir for HOME path %s: %s\e[0m", getenv("HOME"), strerror(errno));
      return 1;
    }
    return 0;
  }
  //case for specified path
  else if (chdir(args[1]) == -1){
    fprintf(stderr, "\e[0;31mError executing chdir for %s: %s\e[0m", args[1], strerror(errno));
    return 1;
  }
  return 0;
}

// print working directory. return 0 on success, 1 on fail.
int pwdcmd()
{
  char buf[4096];
  if (getcwd(buf, 4096) == NULL)
  {
    fprintf(stderr, "\e[0;31mError executing getcwd: %s\e[0m", strerror(errno));
    return 1;
  }
  fprintf(stdout, "%s\n", buf);
  return 0;
}

// goodbye world
int exitcmd(char* args[])
{
  //case where no input
  if (args[1] == NULL){
    fprintf(stderr, "Exiting shell with exit code %d\n", exitCode);
    exit(exitCode);
  }
  //other case
  else{
    fprintf(stderr, "Exiting shell with exit code %d\n", atoi(args[1]));
    exit(atoi(args[1]));
  }
}

//executes (or tries to) whatever arbituary command
int arbitcmd(int argLoc, char* args[]){
  // flush before eating
  fflush(stdout);
  struct timespec start, end;
  struct rusage use;
  clock_gettime(CLOCK_MONOTONIC, &start);
  // eat with fork
  int pid = fork();
  int status;

  switch(pid){
    case -1:
      fprintf(stderr, "\e[0;31mError forking: %s\e[0m\n", strerror(errno));
      return -1;
    case 0: 
      // hello world
      for(int i = argLoc - 1; i > 0; i--) {
        int rdir = redirect(args[i]);
        if (rdir == 1) {
          args[i] = NULL;
        } else if (rdir == -1) {
          exit(1);
        } else {
          break;
        }
      }
      // cut off my tail
      if(iFile) {
        fclose(iFile);
      }
      // flush before dying
      fflush(stdout);
      if (execvp(args[0], args) == -1) {
        fprintf(stderr, "\e[0;31mError executing %s: %s\e[0m\n", args[0], strerror(errno));
        exit(1);
      }

    default:
      // outlive the child
      wait4(pid, &status, 0, &use);

      // funeral speech
      int code;
      if (WIFEXITED(status)) {
        code = WEXITSTATUS(status);
        fprintf(stderr, "\e[0;34mPID %d exited with code %d\e[0m\n", pid, code);
      }
      else if (WIFSIGNALED(status)) {
        code = WTERMSIG(status);
        fprintf(stderr, "\e[0;34mPID %d murdered by signal %d\e[0m\n", pid, code);
      }

      clock_gettime(CLOCK_MONOTONIC, &end);  

      fprintf(stderr, "\e[0;34mreal time: %d.%06d, user time: %d.%06d, sys time: %d.%06d\e[0m\n", end.tv_sec - start.tv_sec, end.tv_nsec - start.tv_nsec, use.ru_utime.tv_sec, use.ru_utime.tv_usec, use.ru_stime.tv_sec, use.ru_stime.tv_usec);

      return code;
  }
}

// check for redirection. return 0 if none, 1 if set, -1 if fail
int redirect(char* arg) {
  if (arg[0] == '<') {
    int fd = open(&arg[1], O_RDONLY, 0666);
    if (fd == -1) {
      fprintf(stderr, "\e[0;Error opening %s for read: %s\e[0m\n", arg, strerror(errno));
      return -1;
    }
    dup2(fd, STDIN_FILENO);
    close(fd);
    return 1;
  } else if (arg[0] == '>') {
    int fd;
    if (arg[1] == '>') {
      fd = open(&arg[2], O_WRONLY | O_CREAT | O_APPEND, 0666);
    } else {
      fd = open(&arg[1], O_WRONLY | O_CREAT | O_TRUNC, 0666);
    }
    if (fd == -1) {
      fprintf(stderr, "\e[0;31mError opening %s for write: %s\e[0m\n", arg, strerror(errno));
      return -1;
    } else {
      dup2(fd, STDOUT_FILENO);
      close(fd);
      return 1;
    }
  } else if (arg[0] == '2' && arg[1] == '>') {
    int fd;
    if (arg[1] == '>') {
      fd = open(&arg[2], O_WRONLY | O_CREAT | O_APPEND, 0666);
    } else {
      fd = open(&arg[1], O_WRONLY | O_CREAT | O_TRUNC, 0666);
    }
    if (fd == -1) {
      fprintf(stderr, "\e[0;31mError opening %s for write: %s\e[0m\n", arg, strerror(errno));
      return -1;
    } else {
      dup2(fd, STDERR_FILENO);
      close(fd);
      return 1;
    }
  } else {
    return 0;
  } 
}
