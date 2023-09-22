#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>

void traverse(DIR*, char* ,FILE*);

int main(int argc, char* argv[])  {
    struct stat tmpStat;

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
}

void traverse(DIR* dir, char* dirName, FILE* fp) {
    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        char path[1024];
        snprintf(path, sizeof(path), "%s/%s", dirName, entry->d_name);
        printf("%s\n", entry->d_name);

        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }
        struct stat info;
        if (lstat(path, &info) < 0) {
            fprintf(stderr, "\e[0;31mGetting stat of %s. Error: %s\n\e[0m", path, strerror(errno));
            continue;
        }
        if (S_ISREG(info.st_mode)) {
            printf("%s is reg", path);
            // compare files
            // st_size equal?
            // memcmp same?
        } else if (S_ISLNK(info.st_mode)) {
            printf("%s is lnk", path);
            // look at link target
        } else if (S_ISDIR(info.st_mode)) {
            printf("%s is dir", path);
            // recursive call
        }
    }
    closedir(dir);
}
