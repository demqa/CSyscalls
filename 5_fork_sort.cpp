#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/wait.h>

int main(int argc, char **argv)
{
    const unsigned multiplier = 10000u;

    for (int i = 1; i < argc; ++i)
    {
        int num = atoi(argv[i]);
        assert(num >= 0);

        int process = fork();
        if (process == 0)
        {
            usleep(multiplier * num);
            printf("%s", argv[i]);
            if (i - 1 != argc) putchar(' ');

            return 0;
        }
    }

    while (argc > 1) {
        int status;
        int pid = waitpid(-1, &status, 0);

        argc--;
    }
    putchar('\n');

    return 0;
}
