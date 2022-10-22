#include <cstdlib>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <assert.h>

#include <stdio.h>
#include <dirent.h>
#include <ctype.h>
#include <sys/stat.h>
#include <unistd.h>

/*

ls
-l // long
-d
-a
-R // recursive
-i
-n

show() - show file
ls() - called to every argv. can be recursive

*/

class Mode
{
    unsigned int mode_;

public:
    enum Modes
    {
        LONG       = 1 << 0,
        DIRECTORY  = 1 << 1,
        ALL        = 1 << 2,
        INODE      = 1 << 3,
        RECURSIVE  = 1 << 4,
        NUMERIC    = 1 << 5,
        PRINT_DIR  = 1 << 6,
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

void show(const dirent* entry)
{
    if (entry->d_name[0] == '.' && !mode.check(Mode::ALL))
        return;

    const char *type = "undefined";
    if (entry->d_type == DT_DIR)
        type = "dir ";
    if (entry->d_type == DT_REG)
        type = "file";

    printf("%s: %s\n", type, entry->d_name);
}

static char buf[4096];
static size_t length;

void ls(const char* path, size_t number) // full path
{
    if (number > 1)
        printf("%s:\n", path);

    struct stat stat_buf;

    int status = stat(path, &stat_buf);
    if (status == -1)
    {
        fprintf(stderr, "myls: cannot access %s: %s\n", path, strerror(errno));
        return;
    }

    // if (stat_buf.st_mode == S_IFREG)
    // {
    //     // show();
    // }

    DIR *dir = opendir(path);
    if (dir == nullptr)
    {
        return;
    }

    errno = 0;

    dirent* entry = nullptr;
    while ((entry = readdir(dir)) != nullptr)
    {
        show(entry);

        if (mode.check(Mode::RECURSIVE))
        {
            size_t add_len = strlen(entry->d_name);

            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                return;

            add_len++;

            buf[length] = '/';
            strcpy(buf + length + 1, entry->d_name);
            length += add_len;

            ls(buf, 2);

            length -= add_len;
            buf[length] = '\0';
        }
    }

    if (errno)
        perror("myls: readdir returned error");

    closedir(dir);
}

int main(int argc, char **argv)
{
    int c = -1;
    int last_option = 1;

    while ((c = getopt(argc, argv, "ldaRin")) != -1)
    {
        last_option = optind;
        switch (c)
        {
            case 'l': mode.set(Mode::LONG);      break;
            case 'd': mode.set(Mode::DIRECTORY); break;
            case 'a': mode.set(Mode::ALL);       break;
            case 'R': mode.set(Mode::RECURSIVE); break;
            case 'i': mode.set(Mode::INODE);     break;
            case 'n': mode.set(Mode::NUMERIC);   break;

            case '?':
            default:
                return EXIT_FAILURE;
                fprintf(stderr, "myls: getoption returned impossible symbol: %c\n", c);
        }
    }

    if (argc == 1)
    {
        strcpy(buf, ".");
        ls(buf, 1);
    }
    else
    {
        int first_arg = last_option;
        for (int index = first_arg; index < argc; ++index)
        {
            strcpy(buf, argv[index]);
            length = strlen(buf);

            ls(buf, argc - first_arg);

            if (index != argc - 1) putchar('\n');
        }
   }

   return EXIT_SUCCESS;
}
