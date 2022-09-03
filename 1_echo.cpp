#include <stdio.h>
#include <string.h>

int main(int argc, char **argv) {
    int  start_index =    1;
    bool next_line   = true;
    if (argc > 1 && strcmp(argv[1], "-n") == 0) {
        start_index++;
        next_line = false;
    }

    for (int i = start_index; i < argc; i++) {
        printf("%s", argv[i]);

        if (i != argc - 1) {
            putchar(' ');
        }
    }

    if (next_line) {
        putchar('\n');
    }

    return 0;
}
