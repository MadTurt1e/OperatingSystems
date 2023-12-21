//a bunch of includes
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

// main stuff
int main(int argc, char* argv[]) {
  //error handling
  if (argc < 4) {
    fprintf(stderr, "\e[0;31mError: too few arguments\n\e[0m");
    exit(2);
  }
  //check to make sure the string lengths are the same
  if (strlen(argv[1]) != strlen(argv[2])) {
    fprintf(stderr, "\e[0;31mError: string length mismatch\n\e[0m");
    exit(2);
  }

  //remember the stringlength
  size_t length = strlen(argv[1]);

  //do for every case after the first two
  for (int i = 3; i < argc; i++) {
    int fd = open(argv[i], O_RDWR);
    // case we fail an open
    if (fd == -1) {
      fprintf(stderr, "\e[0;31mError: opening %s for rw, %s\n\e[0m", argv[i], strerror(errno));
      continue;
    }//i think that's all the fail cases
    
    // remember file information stuff
    struct stat st;
    fstat(fd, &st);

    //the time has come
    char* region = (char*) mmap(NULL, st.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    // oh no
    if (region == MAP_FAILED) {
      fprintf(stderr, "\e[0;31mError: mapping %s to memory, %s\n\e[0m", argv[i], strerror(errno));
      close(fd);
      continue;
    }

    //substring finder
    char* pos = strstr(region, argv[1]);

    // repeat for all
    while(pos) {
      // painful painful C string manipulation
      strncpy(pos, argv[2], length);
      pos = pos + length;
      pos = strstr(pos, argv[1]);
    }

    // eliminate map
    munmap(region, st.st_size);
    close(fd);
  }
  //good case
  return 0;
}
