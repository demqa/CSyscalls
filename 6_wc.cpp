#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <assert.h>

#include <stdio.h>
#include <ctype.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <unistd.h>

/*

$ mywc echo hello
$ 1 1 6

actually the same as:

$ echo hello | wc
$ 1 1 6


I have to make child process to write to parent.
Parent (wc) is reading it and proceeds.

pipe.
bytes written to fds[1]
can be read from fds[0]

*/

struct statistic
{
    size_t word_count;
    size_t line_count;
    size_t byte_count;
};

enum states
{
    EOF_STATE = 0,
    IS_WORD,
    IS_SPACE,
};

const static size_t BUFF_SIZE = 4096;

void print_stat(const statistic* stat)
{
    assert(stat != nullptr);

    printf("word count: %lu\n", stat->word_count);
    printf("line count: %lu\n", stat->line_count);
    printf("byte count: %lu\n", stat->byte_count);
}

void proceed(const char buf[BUFF_SIZE], size_t size, statistic* stat)
{
    assert(stat != nullptr);

    int cond = IS_SPACE;
    size_t i = 0;

    while (cond && i < size)
    {
        char sym = buf[i];

        switch (cond) {
            case IS_SPACE:
                if (sym == '\n') stat->line_count++;

                if (!isspace(sym))
                {
                    cond = IS_WORD;
                    stat->word_count++;
                    continue;
                }

                i++;
                break;

            case IS_WORD:
                if (isspace(sym))
                {
                    cond = IS_SPACE;
                    continue;
                }

                i++;
                break;

            default:
                break;
        }
    }

    stat->byte_count += i;
}

int main(int argc, char **argv)
{
    int status = 0;
    int fds[2];

    status = pipe(fds);
    if (status == -1)
    {
        perror("wc: pipe cannot be created.");
        return EXIT_FAILURE;
    }

    if (fork() == 0)
    {
        // close fd from where parent will read
        close(fds[0]);

        // opens fds[1] as 1 for child
        status = dup2(fds[1], 1);
        close(fds[1]);

        if (status == -1)
        {
            perror("wc: dup2 failed in child.");
            return EXIT_FAILURE;
        }

        status = execvp(argv[1], argv + 1);
        if (status == -1)
        {
            perror("wc: execvp failed.");
            return EXIT_FAILURE;
        }

        return EXIT_SUCCESS;
    }

    // close fd where child is writing.
    close(fds[1]);

    timeval start;
    gettimeofday(&start, nullptr);

    char   buf[BUFF_SIZE];
    size_t size = 0;

    statistic stat = {};
    do
    {
        size = read(fds[0], buf, BUFF_SIZE);

        proceed(buf, size, &stat);
    }
    while (size != 0);

    print_stat(&stat);

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
