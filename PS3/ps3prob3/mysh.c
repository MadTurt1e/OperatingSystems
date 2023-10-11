#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>
#include <stdbool.h>
#include <unistd.h>

int lineReader(char line[]);
int execcmd(char *args[]);

int cdcmd(char *args[]);
int pwdcmd();
int exitcmd(char *args[]);
int arbitcmd(char *args[]);

int lastCommand;

int main(int argc, char *argv[])
{
	// This should go line by line
	char *line[4096];
	size_t size = 4096;

	FILE *iFile = 0;
	bool openAttempt = false;
	// argv[1] should be the name of the file we are reading
	if (argv[1])
	{
		iFile = fopen(argv[1], "r");
		openAttempt = true;
		dup2(stdin, iFile);
		close(iFile); // something about clean file descriptors
	}
	// otherwise, we're just looking at terminal mode where we read from terminal

	// a quick check to make sure the input file is valid
	if (!iFile && openAttempt)
	{
		fprintf(stderr, "\e[0;31mOpening file %s for read. Error: %s\n\e[0m", argv[1], strerror(errno));
		exit(-1);
	}

	// gaming time

	// Basically, we want to be reading. Forever.
	while (gets(line, 4096, stdin) != EOF)
	{
		// we leave this as a job for another function.
		lastCommand = lineReader(line);
		continue;
	}

	fprintf(stderr, "End of file read");
	char* exitSequence[2];
	exitcmd(exitSequence);
}

// reads something line by line, and does what's on the tin.
int lineReader(char line[])
{
	// comments are ignored
	if (line[0] == '#')
	{
		return 0;
	}

	char *args[4096];
	char *input[4096];
	char *output[4096];
	//"tokenize" string using space as the delimiter
	char *tok = strtok(line, " ");
	/// go through the string, until we can't anymore, and add each value to the args array
	int argLoc = 0;

	while (tok != NULL)
	{
		args[argLoc++] = tok;
		tok = strtok(NULL, " ");
	}
	// set the final value of args to null
	args[argLoc] = NULL;

	// execute commands
	execcmd(args);
}

int execcmd(char *args[])
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
		return arbitcmd(args);
	}
	// uhh...
	return 1;
}

// change directory command
int cdcmd(char *args[])
{
	//case for no specified path
	if (args[1] == NULL){
		if (chdir(getenv("HOME")) == -1){
			fprintf(stderr, "\e[0;31mError executing chdir for HOME path %s: %s\e[0m", getenv("HOME"), strerror(errno));
			return -1;
		}
		return 0;
	}
	//case for specified path
	else if (chdir(args[1]) == -1){
		fprintf(stderr, "\e[0;31mError executing chdir for %s: %s\e[0m", args[1], strerror(errno));
		return 0;
	}

	//uhh...
	return -1;
}

// print working directory
int pwdcmd()
{
	char *buf[4096];
	if (getcwd(buf, 4096) == NULL)
	{
		fprintf(stderr, "\e[0;31mError executing getcwd: %s\e[0m", strerror(errno));
		return -1;
	}
	fprintf(stdout, "%s\n", buf);
	return 0;
}

// exit
int exitcmd(char *args[])
{
	//case where no input
	if (args[1] == NULL){
		fprintf(stderr, "Exiting shell with exit code %d", lastCommand);
		exit(lastCommand);
	}
	//other case
	else{
		fprintf(stderr, "Exiting shell with exit code %d", atoi(args[1]));
		exit(atoi(args[1]));
	}

	//bad case
	fprintf(stderr, "Exiting shell with exit code %d", lastCommand);
	exit(lastCommand);
	return 1;
}

//executes (or tries to) whatever arbituary command
int arbitcmd(char *args[]){
	//step 1: fork
	int pid = fork();
	
	//fork cases
	switch(pid){
		case -1:
			fprintf(stderr, "\e[0;31mError executing fork: %s\e[0m", strerror(errno));
			return -1;
		case 0: //child case
			execvp(args[0], args);
		default:
			//returns the first child that comes back
			return wait(pid);
	}
}
