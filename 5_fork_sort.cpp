#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>

int main(int argc, char **argv)
{
    int max = 0;
    const unsigned multiplier = 10000u;

    for (int i = 1; i < argc; ++i)
    {
        int num = atoi(argv[i]);
        assert(num >= 0);

        if (num > max) max = num;

        int process = fork();
        if (process == 0)
        {
            usleep(multiplier * num);
            printf("%s", argv[i]);
            if (i - 1 != argc) putchar(' ');

            return 0;
        }
    }

    usleep(multiplier * (max + 1));
    putchar('\n');

    return 0;
}
