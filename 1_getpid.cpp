#include <stdio.h>
#include <unistd.h>

int main() {
    printf(" getpid() = %d\n",  getpid());
    printf("getppid() = %d\n", getppid());

    return 0;
}
