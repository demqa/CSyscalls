#include <cstdlib>
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
    assert(passwd != nullptr);

    printf("%d(%s)", passwd->pw_gid, passwd->pw_name);
}

void print_groups(const gid_t *list, size_t number) {
    printf("groups=");
    for (size_t index = 0; index < number; ++index) {
        group *group = getgrgid(list[index]);
        print_group(group);
        if (index + 1 != number) {
            putchar(',');
        }
    }

    putchar('\n');
}

int main(int argc, char **argv) {
    int status = 0;

    if (argc > 2) {
        fprintf(stderr, "myid: all arguments will be ignored except of first.\n");
    }

    if (argc == 2) {
        passwd *passwd = getpwnam(argv[1]);
        if (passwd == nullptr) {
            fprintf(stderr, "myid: \'%s\': no such user.\n", argv[1]);
            return EXIT_FAILURE;
        }

        printf("uid="); print_passwd(passwd); putchar(' ');

        gid_t gid = passwd->pw_gid;
        group *group_ = getgrgid(gid);
        if (group_ == nullptr) {
            fprintf(stderr, "myid: cannot get group by gid=%d, %s\n", gid, strerror(errno));
        }
        else {
            printf("gid="); print_group(group_); putchar(' ');
        }

        int nelems = 0;
        getgrouplist(argv[1], gid, nullptr, &nelems);

        gid_t *groups = (gid_t *) calloc((size_t) nelems, sizeof(gid_t));
        getgrouplist(argv[1], gid, groups, &nelems);

        print_groups(groups, (size_t) nelems);

        return EXIT_SUCCESS;
    }

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

    print_groups(list, (size_t) status);

    free(list);

    return 0;
}
