#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <assert.h>

#include <stdio.h>
#include <ctype.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <unistd.h>

#include <fcntl.h>
#include <pwd.h>
#include <sys/mman.h>

#define DEBUG(arg) \
do { fprintf(stderr, "%03d: %s = %d\n", __LINE__, #arg, arg); } while(0)

#define DEBUGS(arg) \
do { fprintf(stderr, "%03d: %s = %s\n", __LINE__, #arg, arg); } while(0)
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

class Mode
{
    unsigned int mode_;

public:
    enum Modes
    {
        PRINT_DATA = 1 << 0,
    };

    bool check(Modes mode) const
    {
        return mode_ & (unsigned int) mode;
    };

    void set(Modes mode)
    {
        mode_ |= (unsigned int) mode;
    };
};

static Mode mode;

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

    // printf("word count: %lu\n", stat->word_count);
    // printf("line count: %lu\n", stat->line_count);
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

size_t proceed_fd(int fd_num)
{
    char   buf[BUFF_SIZE];
    size_t size = 0;

    // if (mode.check(Mode::PRINT_DATA))
    //     printf("text received from %s:\n", info_src);

    statistic stat = {};
    do
    {
        size = read(fd_num, buf, BUFF_SIZE);

        proceed(buf, size, &stat);

        if (mode.check(Mode::PRINT_DATA))
            write(1, buf, size);
    }
    while (size != 0);

    // assert(info_src != nullptr);
    // printf("info about %s:\n", info_src);

    // print_stat(&stat);

    return stat.byte_count;
}

int main(int argc, char **argv)
{
    int c = -1;
    int last_option = 1;

    // DEBUGS(argv[0]);

    // DEBUG(argc);
    // DEBUG(optind);
    // DEBUG(getpid());

    while ((c = getopt(argc, argv, "d")) != -1)
    {
        // DEBUG(optind - last_option);
        last_option = optind;
        // DEBUG(optind);
        // DEBUG(getpid());

        switch (c)
        {
            case 'd': mode.set(Mode::PRINT_DATA); break;

            case '?': break;
            default:
                fprintf(stderr, "wc: getoption returned impossible symbol: %c\n", c);
                return EXIT_FAILURE;
        }

        // DEBUG(optind);
        // DEBUG(getpid());
    }

    last_option = optind;

    int status = 0;
    int fds_out[2];
    int fds_err[2];

    status = pipe(fds_out);
    if (status == -1)
    {
        perror("wc: pipe for stdout cannot be created.");
        return EXIT_FAILURE;
    }

    status = pipe(fds_err);
    if (status == -1)
    {
        perror("wc: pipe for stderr cannot be created.");
        return EXIT_FAILURE;
    }

    if (fork() == 0) // on child
    {
        // close fds from where parent will read
        close(fds_out[0]);
        close(fds_err[0]);

        // opens fds[1] as 1 for child
        status = dup2(fds_out[1], 1);
        close(fds_out[1]);
        if (status == -1)
        {
            perror("wc: dup2 on stdout failed in child.");
            return EXIT_FAILURE;
        }

        // opens fds[2] as 2 for child
        status = dup2(fds_err[1], 2);
        close(fds_err[1]);
        if (status == -1)
        {
            perror("wc: dup2 on stderr failed in child.");
            return EXIT_FAILURE;
        }

        // DEBUG(last_option + 1);

        status = execvp(argv[last_option], argv + last_option);
        if (status == -1)
        {
            perror("wc: execvp failed.");
            return EXIT_FAILURE;
        }

        return EXIT_SUCCESS;
    }

    // close fds where child is writing.
    close(fds_out[1]);
    close(fds_err[1]);

    //--------------------------------------------------------//
    // I am excited of this code working properly next commit //
    //--------------------------------------------------------//

    size_t* sizes = (size_t *)mmap(nullptr, 2 * sizeof(size_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (sizes == (void *) -1) {
        perror("wc: mmap failed.");
        return EXIT_FAILURE;
    }

    if (fork() == 0)
    {
        sizes[0] = proceed_fd(fds_out[0]);
        return 0;
    }

    if (fork() == 0)
    {
        sizes[1] = proceed_fd(fds_err[0]);
        return 0;
    }

    waitpid(-1, &status, 0);
    waitpid(-1, &status, 0);

    printf("out size = %lu\n", sizes[0]);
    printf("err size = %lu\n", sizes[1]);

    munmap(sizes, 2 * sizeof(size_t));

    return EXIT_SUCCESS;
}
