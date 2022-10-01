// gettimeofday()
// nanoseconds
//

#include <cstdlib>
#include <string.h>
#include <errno.h>

#include <cstdio>
#include <sys/time.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char **argv)
{
    timeval time;

    gettimeofday(&time, nullptr);
    int start = time.tv_usec;

    if (fork() == 0)
    {
        int status = execvp(argv[1], argv + 1);
        if (status == -1)
        {
            printf("exec: %s\n", strerror(errno));

            return EXIT_FAILURE;
        }

        return EXIT_SUCCESS;
    }

    int pid = waitpid(-1, 0, 0);

    gettimeofday(&time, nullptr);
    int end   = time.tv_usec;

    printf("exec %s. lasts for %u microseconds.\n", argv[1], end - start);

    return 0;
}
