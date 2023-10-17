/********************************************
 * wordgen.c          ECE357
 * This program will generate "words"
 * *****************************************/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// returns a random character
char randomChar()
{
    return 'A' + (rand() % 26);
}

// prints a random word of the length
void wordGen(int length)
{
    for (int i = 0; i < length; i++)
    {
        fprintf(stdout, "%c", randomChar());
    }
    printf("\n");
}

int main(int argc, char *argv[])
{
    srand(time(NULL));

    int minLength = 3;
    int maxLength = 25;

    int limit = 0;
    // 1: check if there are arguments
    if (argc > 1)
    {
        limit = atoi(argv[1]);
    }

    // 2: we are repeating forever
    int repeatForever = 0;
    if (limit == 0)
    {
        repeatForever = 1;
    }

    // do for as many words as we said
    int count = 0;
    for (count = 0; count < limit || limit == 0 || repeatForever; ++count)
    {
        // wordlength will be randomized + whatever off by 1 error
        int length = minLength + (rand() % (maxLength - minLength + 1));

        // make a word + output it
        wordGen(length);
    }
    fprintf(stderr, "Finished generating %d candidate words\n", count);
    return 0;
}