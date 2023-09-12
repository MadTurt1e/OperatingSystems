#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

int catch(int f, char* arg) {
    if (f < 0) {
        write(2, arg, strlen(arg));
        write(2, strerror(errno), strlen(strerror(errno)));
        exit(-1);
    } else {
        return f;
    }
}

int meow(int fd_in, int fd_out, char* buf, char* fd_name) {
    int charRead = -1;
    int charWritten = -1;
    int transfered = 0;
    char str[1024];
    int lineCount = 0;
    while (charRead != 0) {
        charRead = catch(read(fd_in, buf, sizeof buf), "read(fd_in, buf, sizeof buf): ");
        if (lineCount == 0 && charRead != 0) {
            lineCount = 1;
        }
        charWritten = catch(write(fd_out, buf, charRead), "write(fd_out, buf, charRead): ");
        transfered += charWritten;
        for (int i = 0; i < charRead; i++) {
            if (buf[i] == '\n') {
                lineCount++;
            }
        }
    }
    sprintf(str, "\nFile: %s, Bytes: %d, Lines: %d\n", fd_name, transfered, lineCount);
    write(2, str, strlen(str));
    return 0;
}

int main(int argc, char* argv[])  {
    int argn = 0;
    int fd_out = 1;
    int fd_in = 0;
    char* fd_name = '\0';
    char buf[4096];
    if (argc == 1) {
        fd_name = "<standard input>";
        meow(fd_in, fd_out, buf, fd_name);
    } else if(strcmp(argv[1], "-o") == 0) {
        fd_out = catch(open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0666), "open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0666): ");
        if (argc > 3) {
            argn = 3;
        } else {
            fd_name = "<standard input>";
            meow(fd_in, fd_out, buf, fd_name);
        }
    } else {
        argn = 1;
    }
    for (argn; argn < argc && argn != 0; argn++) {
        if (strcmp(argv[argn], "-") == 0) {
            fd_in = 0;
            fd_name = "<standard input>";
            meow(fd_in, fd_out, buf, fd_name);
        } else {
            fd_in = catch(open(argv[argn], O_RDONLY), "open(argv[argn], O_RDONLY): ");
            fd_name = argv[argn];
            meow(fd_in, fd_out, buf, fd_name);
            catch(close(fd_in), "close(fd_in)");
        }
    }
    return 0;
}