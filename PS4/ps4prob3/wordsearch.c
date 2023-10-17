/********************************************
 * wordsearch.c          ECE357
 * This program will search for "words"
 * *****************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

// converts something to uppercase
void upper(char *str)
{
    for (; *str; ++str)
        *str = toupper(*str);
}

int main(int argc, char *argv[])
{
    int maxLength = 100;

    // if argc is not 2, someone messed up, and it's not me.
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s dictionary_file\n", argv[0]);
        return 1;
    }

    // attempt opening the file
    FILE *file = fopen(argv[1], "r");
    if (!file)
    {
        fprintf(stderr, "Error opening file %s: %s\n", argv[1], strerror(errno));
        return 1;
    }

    // assume 100 is the largest size
    char word[maxLength];

    // load dictionary

    //TODO: What if dictSize is more than a million? 
    int dictSize = 1000000;
    int loadedWords = 0;
    char *dictionary[dictSize];

    while (fgets(word, sizeof(word), file))
    {
        upper(word);
        // makes a seperate instanse of the word so no weird pointer stuff happens to it
        dictionary[loadedWords++] = strdup(word);
    }
    fclose(file);

    int matchedwords = 0;
    // get all the words line by line
    while (fgets(word, sizeof(word), stdin))
    {
        // converts everything to uppercase
        upper(word);
        // look for word in the list
        for (int i = 0; i < loadedWords; i++)
        {
            // case we find it
            if (strcmp(word, dictionary[i]) == 0)
            {
                fprintf(stdout, "%s", word);
                ++matchedwords;
                break;
            }
        }
    }

    fprintf(stderr, "Matched %d words\n", matchedwords);
    return 0;
}