#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <stdlib.h>

#include <grp.h>
#include <pwd.h>

#include <errno.h>
#include <string.h>

#define PRINT(arg) do { fprintf(stderr, "%s on line №%d is %d\n", #arg, __LINE__, (arg)); } while(0)

void print_group(const group *group) {
    assert(group != nullptr);

    printf("%d(%s)", group->gr_gid, group->gr_name);
}

void print_passwd(const passwd *passwd) {
    assert(passwd!= nullptr);

    printf("%d(%s)", passwd->pw_gid, passwd->pw_name);
}

int main(int argc, char **argv) {
    assert(argc == 1);

    int status = 0;

    uid_t euid = geteuid();
    printf("uid="); print_passwd(getpwuid(euid)); putchar(' ');

    gid_t  gid = getgid();
    printf("gid="); print_group(getgrgid(gid));   putchar(' ');

    const size_t group_max_size = 10;
    gid_t *list = (gid_t *) calloc(group_max_size, sizeof(gid_t));
    if (list == nullptr) {
        fprintf(stderr, "myid: cannot allocate memory.\n");
        return EXIT_FAILURE;
    }

    status = getgroups(group_max_size, list);
    if (status == -1) {
        fprintf(stderr, "myid: cannot get groups: %s.\n", strerror(errno));
        return EXIT_FAILURE;
    }

    printf("groups=");
    for (size_t index = 0; index < status; ++index) {
        group *current = getgrgid(list[index]);
        print_group(current);
        if (index + 1 != status) {
            putchar(',');
        }
    }

    putchar('\n');

    free(list);

    return 0;
}
