#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>
#include <stdbool.h>
#include <unistd.h>

void traverse(char*, FILE*, struct stat*);
bool compare(char*, FILE*, int);

int main(int argc, char *argv[])
{
    // this is to make sure that we actually have input
    if (!argv[1])
    {
        fprintf(stderr, "\e[0;31mError: No file name provided.\n\e[0m");
        exit(-1);
    }
    // open file for reading
    FILE* fp = fopen(argv[1], "r");
    if (!fp)
    {
        fprintf(stderr, "\e[0;31mOpening file %s for read. Error: %s\n\e[0m", argv[1], strerror(errno));
        exit(-1);
    }

    // this is basically able to have all the metadata info of a particlular file
    struct stat fileStat;
    stat(argv[1], &fileStat);

    // This evaluates to false if the argument is a regular file.
    if (!S_ISREG(fileStat.st_mode))
    {
        fprintf(stderr, "\e[0;31mOpening file %s for read. Error: Not a regular file\n\e[0m", argv[1]);
        exit(-1);
    }
    // ensuring a directory specified
    if (!argv[2])
    {
        fprintf(stderr, "\e[0;31mError: no directory provided.\n\e[0m");
        exit(-1);
    }

    // because we're going to open it
    DIR* dir = opendir(argv[2]);
    if (!dir)
    {
        fprintf(stderr, "\e[0;31mOpening directory %s for traversal. Error: %s.\n\e[0m", argv[2], strerror(errno));
        exit(-1);
    }

    // get the stats from the directory
    struct stat dirStat;
    stat(argv[2], &dirStat);
    if (!S_ISDIR(dirStat.st_mode))
    {
        fprintf(stderr, "\e[0;31mOpening directory %s for traversal. Error: Not a directory\n\e[0m", argv[2]);
        exit(-1);
    }
    closedir(dir);
    traverse(argv[2], fp, &fileStat);
}

// looks through the directory, given a file and its metadata
void traverse(char* dirName, FILE* fp, struct stat* fileStat)
{
    // get dir back up
    DIR* dir = opendir(dirName);
    // https://www.gnu.org/software/libc/manual/html_node/Directory-Entries.html for quick reference
    struct dirent* entry;

    // get the directory info from the directory, and repeat for all directories
    while ((entry = readdir(dir)) != NULL)
    {
        char path[1024];
        snprintf(path, sizeof(path), "%s/%s", dirName, entry->d_name);
        struct stat entryStat;
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
        {
            continue;
        }
        //if lstat resolves in an error, continue
        else if (lstat(path, &entryStat) < 0)
        {
            fprintf(stderr, "\e[0;33mGetting stat of %s. Warning: %s\n\e[0m", path, strerror(errno));
            continue;
        }
        else if (S_ISREG(entryStat.st_mode))
        {
            //good case
            if (entryStat.st_ino == fileStat->st_ino) // wrong! && entryStat.st_dev == fileStat->st_dev
            {
                printf("%s HARD LINK TO TARGET\n", path);
            }
            //case where the file has been copied over
            else if (entryStat.st_size == fileStat->st_size && compare(path, fp, entryStat.st_size))
            {
                printf("%s DUPLICATE OF TARGET (nlink=%d)\n", path, entryStat.st_nlink);
            }
        }
        //we have ourselves a symlink
        else if (S_ISLNK(entryStat.st_mode))
        {
            char linkPath[1024];

            ssize_t len = readlink(path, linkPath, sizeof(linkPath) - 1);
            //if len results in a error, continue
            if (len < 0)
            {
                fprintf(stderr, "\e[0;33mReading symlink %s. Warning: %s\n\e[0m", path, strerror(errno));
                continue;
            }
            linkPath[len] = '\0';
            struct stat linkStat;
            //if stat resolves in an error, we continue
            if (stat(path, &linkStat) < 0)
            {
                fprintf(stderr, "\e[0;33mGetting stat of %s from %s Warning: %s\n\e[0m", linkPath, path,strerror(errno));
                continue;
            }
            //weird file? continue
            else if (!S_ISREG(linkStat.st_mode))
            {
                continue;
            }
            //symlink is good
            else if (linkStat.st_ino == fileStat->st_ino)
            {
                printf("%s SYMLINK (%s) RESOLVES TO TARGET\n", path, linkPath);
            }
            //symlink is okay
            else if (linkStat.st_size == fileStat->st_size && compare(path, fp, linkStat.st_size))
            {
                printf("%s SYMLINK (%s) RESOLVES TO DUPLICATE\n", path, linkPath);
            }
        }
        //if we find another directory
        else if (S_ISDIR(entryStat.st_mode))
        {
            traverse(path, fp, fileStat);
        }
    }
    //finish
    closedir(dir);
}

//compare fxn just in case the file has been copied
bool compare(char* path, FILE* fp, int size)
{
    FILE* cmpFp = fopen(path, "r");

    fseek(fp, 0, SEEK_SET);

    char* buff1 = malloc(size);
    char* buff2 = malloc(size);

    if (!buff1 || !buff2)
    {
        fprintf(stderr, "\e[0;33mAllocating buffer for %s. Warning: %s\n\e[0m", path, strerror(errno));
        free(buff1);
        free(buff2);
        return false;
    }

    size_t bytesRead1 = fread(buff1, 1, size, cmpFp);
    size_t bytesRead2 = fread(buff2, 1, size, fp);

    //compares the memory
    int result = memcmp(buff1, buff2, size);

    free(buff1);
    free(buff2);
    fclose(cmpFp);

    //returns true if the memory is good
    return result == 0;
}
