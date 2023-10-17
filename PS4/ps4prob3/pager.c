/********************************************
 * pager.c          ECE357
 * This program is a bootleg more
 * *****************************************/

#include <stdio.h>
#include <string.h>

#define LINES_PER_PAGE 23

int main()
{
    // dev/tty for reasons
    FILE *tty = fopen("/dev/tty", "r");
    if (!tty)
    {
        perror("Error opening /dev/tty");
        return 1;
    }

    // buffer
    char line[BUFSIZ];
    int lineCount = 0;

    char c = 0;
    // line by line
    while (fgets(line, sizeof(line), stdin))
    {
        printf("%s", line);
        // increment and makes sure its not a multiple of 23?
        if (++lineCount % LINES_PER_PAGE == 0)
        {
            // something we have to do
            printf("---Press RETURN for more---\n");
            // checks to see if we want to quit
            fgets(line, sizeof(line), tty);
            if (line[0] == 'q' || line[0] == 'Q'){
                fprintf(stderr, "pager terminated by %c command\n", line[0]);
                break;
            }
        }
    }

    fclose(tty);
    return 0;
}