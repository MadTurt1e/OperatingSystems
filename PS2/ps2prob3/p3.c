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

int main(int argc, char* argv[])  {
    if (!argv[1]) {
        fprintf(stderr, "\e[0;31mError: No file name provided.\n\e[0m");
        exit(-1);
    }
    FILE* fp = fopen(argv[1], "r");
    if (!fp) {
        fprintf(stderr, "\e[0;31mOpening file %s for read. Error: %s\n\e[0m", argv[1], strerror(errno));
        exit(-1);
    } 
    struct stat fileStat;
    stat(argv[1], &fileStat);
    if (!S_ISREG(fileStat.st_mode)) {
        fprintf(stderr, "\e[0;31mOpening file %s for read. Error: Not a regular file\n\e[0m", argv[1]);
        exit(-1);
    }

    if (!argv[2]) {
        fprintf(stderr, "\e[0;31mError: no directory provided.\n\e[0m");
        exit(-1);
    }
    DIR* dir = opendir(argv[2]);
    if (!dir) {
        fprintf(stderr, "\e[0;31mOpening directory %s for traversal. Error: %s.\n\e[0m", argv[2], strerror(errno));
        exit(-1);
    }
    struct stat dirStat;
    stat(argv[2], &dirStat);
    if (!S_ISDIR(dirStat.st_mode)) {
        fprintf(stderr, "\e[0;31mOpening directory %s for traversal. Error: Not a directory\n\e[0m", argv[2]);
        exit(-1);
    }
    closedir(dir);

    traverse(argv[2], fp, &fileStat);
}

void traverse(char* dirName, FILE* fp, struct stat* fileStat) {
    DIR* dir = opendir(dirName);
    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        char path[1024];
        snprintf(path, sizeof(path), "%s/%s", dirName, entry->d_name);
        struct stat entryStat;

        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        } else if (lstat(path, &entryStat) < 0) {
            fprintf(stderr, "\e[0;33mGetting stat of %s. Warning: %s\n\e[0m", path, strerror(errno));
            continue;
        } else if (S_ISREG(entryStat.st_mode)) {
            if (entryStat.st_ino == fileStat->st_ino) {
                printf("%s HARD LINK TO TARGET\n", path);
            } else if (entryStat.st_size == fileStat->st_size && compare(path, fp, entryStat.st_size)) {
                printf("%s DUPLICATE OF TARGET (nlink=%d)\n", path, entryStat.st_nlink);
            }
        } else if (S_ISLNK(entryStat.st_mode)) {
            char linkPath[1024];

            ssize_t len = readlink(path, linkPath, sizeof(linkPath)-1);
            if (len < 0) {
                fprintf(stderr, "\e[0;33mReading symlink %s. Warning: %s\n\e[0m", path, strerror(errno));
                continue;
            }           
            linkPath[len] = '\0';
            struct stat linkStat;
            if (stat(path, &linkStat) < 0) {
                fprintf(stderr, "\e[0;33mGetting stat of %s. Warning: %s\n\e[0m", linkPath, strerror(errno));
                continue;
            } else if (!S_ISREG(linkStat.st_mode)) {
                continue;
            } else if (linkStat.st_ino == fileStat->st_ino) {
                printf("%s SYMLINK (%s) RESOLVES TO TARGET\n", path, linkPath);
            } else if (linkStat.st_size == fileStat->st_size && compare(path, fp, linkStat.st_size)) {
                printf("%s SYMLINK (%s) RESOLVES TO DUPLICATE\n", path, linkPath);
            }            
        } else if (S_ISDIR(entryStat.st_mode)) {
            traverse(path, fp, fileStat);
        }
    }
    closedir(dir);
}

bool compare(char* path, FILE* fp, int size) {
    FILE* cmpFp = fopen(path, "r");

    fseek(fp, 0, SEEK_SET);

    char* buff1 = malloc(size);
    char* buff2 = malloc(size);

    if (!buff1 || !buff2) {
        fprintf(stderr, "\e[0;33mAllocating buffer for %s. Warning: %s\n\e[0m", path, strerror(errno));
        free(buff1);
        free(buff2);
        return false;
    }

    size_t bytesRead1 = fread(buff1, 1, size, cmpFp);
    size_t bytesRead2 = fread(buff2, 1, size, fp);

    int result = memcmp(buff1, buff2, size);
    
    free(buff1);
    free(buff2);
    fclose(cmpFp);

    return result == 0;
}
