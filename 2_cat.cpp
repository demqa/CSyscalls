// #include <stdio.h>

#include <cstdio>
#include <cstdlib>
#include <fcntl.h>  // open
#include <unistd.h> // close

#include <errno.h>
#include <stddef.h>
#include <stdlib.h>

int cat(int fd_int, int fd_out) {
    const size_t buf_size = 1024;
    char buffer[buf_size];

    int readsize = 0;
    do {
        readsize = read(fd_int, buffer, buf_size);

        write(fd_out, buffer, readsize);
    } while (readsize > 0);

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
