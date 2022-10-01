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

    timeval start;
    gettimeofday(&start, nullptr);

    int status;
    int pid = waitpid(-1, &status, 0);

    timeval end;
    gettimeofday(&end, nullptr);

    timeval result;
    timersub(&end, &start, &result);

    unsigned long long diff = result.tv_sec * 1000000u + result.tv_usec;

    if (status == 0)
        printf("exec %s. lasts for %llu microseconds.\n", argv[1], diff);

    return EXIT_SUCCESS;
}
