/********************************************
 * wordsearch.c          ECE357
 * This program will search for "words"
 * *****************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <sys/signal.h>

int matchedwords = 0;

void handler(int s) {
  exit(0);
}

void report() {
  fprintf(stderr, "Matched %d words\n", matchedwords);
}

// converts something to uppercase
void upper(char* str)
{
  for (; *str; ++str)
    *str = toupper(*str);
}

int main(int argc, char* argv[])
{
  atexit(report);
  signal(SIGPIPE, handler);
  int maxLength = 100;

  // if argc is not 2, someone messed up, and it's not me.
  if (argc != 2)
  {
    fprintf(stderr, "Usage: %s dictionary_file\n", argv[0]);
    return 1;
  }

  // attempt opening the file
  FILE* file = fopen(argv[1], "r");
  if (!file)
  {
    fprintf(stderr, "Error opening file %s: %s\n", argv[1], strerror(errno));
    return 1;
  }

  // assume 100 is the largest size
  char word[maxLength];

  // load dictionary

  int dictSize = 10000;
  int loadedWords = 0;
  int rejectedWords = 0;
  char(* dictionary)[maxLength] = malloc(dictSize * sizeof(word));

  while (fgets(word, sizeof(word), file))
  {
    upper(word);
    for (int i = 0; i < strlen(word); i++) {
      if (!isalpha(word[i])) {
        ++rejectedWords;
        continue;
      }
    }
    // makes a seperate instanse of the word so no weird pointer stuff happens to it
    strcpy(dictionary[loadedWords++], (word));
    if (loadedWords >= dictSize)
    {
      dictSize*=2;
      dictionary = realloc(dictionary, dictSize * sizeof(word));
    }
  }
  fclose(file);

  fprintf(stderr, "Accepted %d words, rejected %d\n", loadedWords, rejectedWords);

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
        fflush(stdout);
        ++matchedwords;
        break;
      }
    }
  }

  return 0;
}