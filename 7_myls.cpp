#include <cstdlib>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <assert.h>

#include <stdio.h>
#include <dirent.h>
#include <ctype.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <pwd.h>
#include <grp.h>

#define DEBUG(arg) \
do { fprintf(stderr, "%03d: %s = %d\n", __LINE__, #arg, arg); } while(0)

#define DEBUGS(arg) \
do { fprintf(stderr, "%03d: %s = %s\n", __LINE__, #arg, arg); } while(0)

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
static char path[4096];
static size_t length;

size_t connect_path(const char* name)
{
    size_t name_length = strlen(name);
    name_length++;

    path[length] = '/';
    strcpy(path + length + 1, name);
    length += name_length;

    return name_length;
}

void disconnect_path(size_t name_length)
{
    length -= name_length;
    path[length] = '\0';
}

void show(const char* name)
{
    assert(name != nullptr);

    if (name[0] == '.' && !mode.check(Mode::ALL))
        return;

    if (mode.check(Mode::INODE))
    {
        size_t name_length = connect_path(name);

        struct stat stat_buf;

        int status = lstat(path, &stat_buf);
        if (status == -1)
        {
            fprintf(stderr, "myls: cannot access %s: %s\n", path, strerror(errno));

            disconnect_path(name_length);
            return;
        }

        printf("%lu ", stat_buf.st_ino);

        disconnect_path(name_length);
    }

    if (mode.check(Mode::LONG))
    {
        size_t name_length = connect_path(name);

        struct stat stat_buf;

        int status = lstat(path, &stat_buf);
        if (status == -1)
        {
            fprintf(stderr, "myls: cannot access %s: %s\n", path, strerror(errno));

            disconnect_path(name_length);
            return;
        }

        char type = '-';
        switch (stat_buf.st_mode & S_IFMT)
        {
            case S_IFDIR: type = 'd'; break;
            case S_IFLNK: type = 'l'; break;

            default:
                type = '-';
        }

        putchar(type);

        bool buff[9];
        buff[0] = (stat_buf.st_mode & S_IRUSR);
        buff[1] = (stat_buf.st_mode & S_IWUSR);
        buff[2] = (stat_buf.st_mode & S_IXUSR);

        buff[3] = (stat_buf.st_mode & S_IRGRP);
        buff[4] = (stat_buf.st_mode & S_IWGRP);
        buff[5] = (stat_buf.st_mode & S_IXGRP);

        buff[6] = (stat_buf.st_mode & S_IROTH);
        buff[7] = (stat_buf.st_mode & S_IWOTH);
        buff[8] = (stat_buf.st_mode & S_IXOTH);

        for (size_t i = 0; i < 9; i++)
        {
            if (buff[i])
            {
                char letter = '?';

                switch (i % 3) {
                    case 0: letter = 'r'; break;
                    case 1: letter = 'w'; break;
                    case 2: letter = 'x'; break;

                    default: letter = '?';
                }

                putchar(letter);
            }
            else
                putchar('-');
        }

        printf(" %lu", stat_buf.st_nlink);

        const passwd* pwd = getpwuid(stat_buf.st_uid);
        printf(" %s", pwd->pw_name);

        const  group* grp = getgrgid(stat_buf.st_gid);
        printf(" %s", grp->gr_name);

        printf(" %5lu", stat_buf.st_size);
        putchar(' ');

        disconnect_path(name_length);
    }

    printf("%s\n", name);
}

void ls(size_t number) // full path
{
    static size_t launch_time = 0;

    if ((number > 1 || mode.check(Mode::RECURSIVE)) && !mode.check(Mode::DIRECTORY))
    {
        if (launch_time != 0)
            putchar('\n');
        printf("%s:\n", path);
    }

    launch_time++;

    if (mode.check(Mode::DIRECTORY))
    {
        // TODO: call view long description.
        show("");

        return;
    }

    DIR *dir = opendir(path);
    if (dir == nullptr)
    {
        fprintf(stderr, "myls: cannot access %s: %s\n", path, strerror(errno));
        return;
    }

    errno = 0;

    dirent* entry = nullptr;
    while ((entry = readdir(dir)) != nullptr)
    {
        show(entry->d_name);
    }

    rewinddir(dir);

    while ((entry = readdir(dir)) != nullptr)
    {
        if (mode.check(Mode::RECURSIVE) && entry->d_type == DT_DIR)
        {
            if (entry->d_name[0] == '.' && !mode.check(Mode::ALL))
                continue;

            if (strcmp(entry->d_name, ".")  == 0 ||
                strcmp(entry->d_name, "..") == 0)
                return;

            size_t name_length = connect_path(entry->d_name);

            ls(2);

            disconnect_path(name_length);
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

    DEBUG(argc);
    DEBUG(last_option);

    DEBUG(mode.check(Mode::ALL));

    if (argc == last_option)
    {
        // DEBUGS(buf);
        strcpy(path, ".");
        length++;
        // DEBUGS(buf);
        ls(1);
    }
    else {
        int first_arg = last_option;
        for (int index = first_arg; index < argc; ++index)
        {
            strcpy(path, argv[index]);
            length = strlen(path);

            ls(argc - first_arg);

            if (index != argc - 1) putchar('\n');
        }
   }

   return EXIT_SUCCESS;
}
