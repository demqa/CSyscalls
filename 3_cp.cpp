#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/stat.h>

#define DEBUG(arg) \
do { fprintf(stderr, "%03d: %s = %d\n", __LINE__, #arg, arg); } while(0)

#define DEBUGS(arg) \
do { fprintf(stderr, "%03d: %s = %s\n", __LINE__, #arg, arg); } while(0)

// flags
// static class Mode
// {
//     verbose
//     force
//     interactive
// }
class Mode
{
    unsigned int mode_;

public:
    enum Modes
    {
        VERBOSE     = 1 << 0,
        FORCE       = 1 << 1,
        INTERACTIVE = 1 << 2,
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

static char path[4096];
static size_t path_length = 0;

size_t connect_path(const char* name)
{
    size_t name_length = strlen(name);
    name_length++;

    path[path_length] = '/';
    strcpy(path + path_length + 1, name);
    path_length += name_length;

    return name_length;
}

void disconnect_path(size_t name_length)
{
    path_length -= name_length;
    path[path_length] = '\0';
}

static Mode mode;

// copy file from source to destination
void copy(char *source, char *destination)
{
    if (mode.check(Mode::VERBOSE))
    {
        printf("mycp: '%s' -> '%s'\n", source, destination);
    }
}

void cp(int nargs, char **args)
{
    if (nargs == 2)
    {
        copy(args[0], args[1]);
    }
    else
    {
        // verify args[nargs - 1] is a directory
        // generate filepathes for all copy operations
        // for (source in sources)
        // {
        //     copy(source, destination + '/' + source);
        // }

        char *dest_dir = args[nargs - 1];

        struct stat buf = {};
        if (stat(dest_dir, &buf) < 0)
        {
            perror("mycp. stat error");
            return;
        }

        if ((buf.st_mode & S_IFMT) != S_IFDIR)
        {
            perror("mycp. last argument is not a directory.");
            return;
        }

        path_length = strlen(dest_dir);
        strcpy(path, dest_dir);

        for (int i = 0; i + 1 < nargs; i++)
        {
            int name_length = connect_path(args[i]);

            copy(args[i], path);

            disconnect_path(name_length);
        }
    }
}

int main(int argc, char **argv) {

    if (argc > 2)
    {
        int c = -1;
        int last_option = 1;

        while ((c = getopt(argc, argv, "ifv")) != -1)
        {
            last_option = optind;
            switch (c)
            {
                case 'i': mode.set(Mode::INTERACTIVE); break;
                case 'f': mode.set(Mode::FORCE);       break;
                case 'v': mode.set(Mode::VERBOSE);     break;

                case '?':
                default:
                    return EXIT_FAILURE;
                    fprintf(stderr, "mycp: getoption returned impossible symbol: %c\n", c);
            }
        }

        int nargs = argc - last_option;
        if (nargs < 2)
        {
            fprintf(stderr, "mycp: invalid number of arguments\n");
            return EXIT_FAILURE;
        }

        // DEBUG(mode.check(Mode::INTERACTIVE));
        // DEBUG(mode.check(Mode::FORCE));
        // DEBUG(mode.check(Mode::VERBOSE));

        // DEBUG(nargs);

        // for (size_t index = last_option; index < argc; index++)
        // {
        //     DEBUGS(argv[index]);
        // }

        cp(nargs, argv + last_option);
    }
    else {
        fprintf(stderr, "mycp: invalid number of arguments\n");
        return EXIT_FAILURE;
    }
}
