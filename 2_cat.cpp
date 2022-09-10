// #include <stdio.h>

#include <cstdio>
#include <cstdlib>
#include <fcntl.h>  // open
#include <unistd.h> // close

#include <errno.h>
#include <stddef.h>
#include <stdlib.h>

#include <string.h>

int cat(int fd_in, int fd_out) {
    const size_t buf_size = 1024;
    char buffer[buf_size];

    ssize_t readsize = 0;
    do {
        readsize = read(fd_in, buffer, buf_size);
        if (readsize < 0) {
            fprintf(stderr, "cannot read file from file descriptor %d: %s\n", fd_in, strerror(errno));
            break;
        }
        else
        if (readsize == 0) {
            break;
        }

        ssize_t writesize = write(fd_out, buffer, readsize);
        if (writesize < 0) {
            fprintf(stderr, "cannot write file to fd = %d: %s\n", fd_out, strerror(errno));
            break;
        } else if (readsize != writesize) {
            fprintf(stderr, "cannot write file to fd = %d, readsize != writesize: %s\n", fd_out, strerror(errno));
            break;
        }
    } while (true);

    return 0;
}

int main(int argc, char **argv) {
    if (argc > 1) {
        printf("This program cannot get any arguments now.\n");

        return EXIT_SUCCESS;
    }

    cat(0, 1);

    return 0;
}
