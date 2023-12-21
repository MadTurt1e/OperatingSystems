#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char* argv[]) {
  if (argc < 4) {
    fprintf(stderr, "\e[0;31mError: too few arguments\n\e[0m");
    exit(2);
  }
  if (strlen(argv[1]) != strlen(argv[2])) {
    fprintf(stderr, "\e[0;31mError: string length mismatch\n\e[0m");
    exit(2);
  }
  size_t length = strlen(argv[1]);
  for (int i = 3; i < argc; i++) {
    int fd = open(argv[i], O_RDWR);
    if (fd == -1) {
      fprintf(stderr, "\e[0;31mError: opening %s for rw, %s\n\e[0m", argv[i], strerror(errno));
      continue;
    }
    struct stat st;
    fstat(fd, &st);
    char* region = (char*) mmap(NULL, st.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (region == MAP_FAILED) {
      fprintf(stderr, "\e[0;31mError: mapping %s to memory, %s\n\e[0m", argv[i], strerror(errno));
      close(fd);
      continue;
    }
    char* pos = strstr(region, argv[1]);
    while(pos) {
      strncpy(pos, argv[2], length);
      pos = pos + length;
      pos = strstr(pos, argv[1]);
    }
    munmap(region, st.st_size);
    close(fd);
  }
  return 0;
}
