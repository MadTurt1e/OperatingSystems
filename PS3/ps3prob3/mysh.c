#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>
#include <stdbool.h>
#include <unistd.h>

int lineReader(char line[4096]);
int execcmd(char *args[4096]);

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
		dup2(0, iFile);
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
	while (gets(line, 4096, stdin) != NULL)
	{
		// we leave this as a job for another function.
		lineReader(line);
		continue;
	}

	exit(0);
}

// reads something line by line, and does what's on the tin.
int lineReader(char line[4096])
{
	// comments are ignored
	if (line[0] == '#')
	{
		return 0;
	}

	char *args[4096];

	//"tokenize" string using space as the delimiter
	char *tok = strtok(line, " ");
	/// go through the string, until we can't anymore, and add each value to the args array
	int i = 0;
	while (tok != NULL)
	{
		args[i++] = tok;
		tok = strtok(NULL, " ");
	}
	// set the final value of args to null
	args[i] = NULL;

	// now for the cases
	// We fork, so that we can run things
	int pid = fork();
	switch (pid)
	{
	case -1:
		fprintf(stderr, "\e[0;31mFork error\e[0m");
		return -1;
	case 0: // child case
		execcmd(*args);
		exit(0); // We kill children
	default:	 // parent case
		// in this case, we'd continue
		return 0;
	}
}

int execcmd(char *args[4096])
{
	// case: nothing is there
	if (args[0] == NULL)
	{
		return 0;
	}
	// cd implmentation
	if (strcmp(args[0], "cd"))
	{
		return 0;
	}
	// pwd implementation
	if (strcmp(args[0], "pwd"))
	{
		return 0;
	}
	// exit implementation.
	if (strcmp(args[0], "exit"))
	{
	}
	// case of unidentified command
	else
	{
		//just execute it, no implementation required. 
		return execvp(args[0], args);
	}
	//uhh...
	return 1;
}