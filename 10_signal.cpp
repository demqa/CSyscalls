// #include <stdio.h>

#include <cstdio>
#include <cstdlib>
#include <fcntl.h>  // open
#include <unistd.h> // close

#include <errno.h>
#include <stddef.h>
#include <stdlib.h>

#include <string.h>

#include <signal.h>

ssize_t safe_write(int fd_out, const char *buffer, ssize_t writesize) {
    ssize_t result_ws = 0;

    while (result_ws != writesize) {
        ssize_t current = write(fd_out, buffer + result_ws, writesize - result_ws);
        if (current < 0) {
            return -1;
        } else if (current == 0) {
            break;
        }

        result_ws += current;
    }

    return result_ws;
}

int cat(int fd_in, int fd_out) {
    const size_t buf_size = 1024;
    char buffer[buf_size];

    int error = 0;

    signal(2, [](int signal)->void{
       printf("based\n");
       kill(getpid(), SIGKILL);
    });

    while (true) {
        ssize_t readsize = read(fd_in, buffer, buf_size);
        if (readsize < 0) {
            fprintf(stderr, "cannot read file from file descriptor %d: %s\n", fd_in, strerror(errno));
            error = -1;
            break;
        } else if (readsize == 0) {
            break;
        }

        ssize_t writesize = safe_write(fd_out, buffer, readsize);
        if (writesize < 0) {
            fprintf(stderr, "cannot write file to fd = %d: %s\n", fd_out, strerror(errno));
            error = -1;
            break;
        } else if (readsize != writesize) {
            fprintf(stderr, "cannot write file to fd = %d, readsize != writesize: %s\n", fd_out, strerror(errno));
            error = -1;
            break;
        }
    };

    return error;
}

int main(int argc, char **argv) {
    if (argc > 1) {
        for (int index = 1; index < argc; ++index) {
            int fd = open(argv[index], O_RDONLY);
            if (fd < 0) {
                fprintf(stderr, "cannot open file with name %s: %s\n", argv[index], strerror(errno));
                continue;
            }

            int result = cat(fd, STDOUT_FILENO);
            if (result < 0) {
                return EXIT_FAILURE;
            }

            result = close(fd);
            if (result < 0) {
                fprintf(stderr, "cannot close file with name %s: %s\n", argv[index], strerror(errno));
                return EXIT_FAILURE;
            }
        }
    } else {
        int result = cat(STDIN_FILENO, STDOUT_FILENO);
        if (result < 0) {
            return EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;
}
