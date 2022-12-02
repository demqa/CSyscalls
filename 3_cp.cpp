#include <stdio.h>
#include <stdlib.h>

// flags
// static class Mode
// {
//     verbose
//     force
//     interactive
// }

// copy file from source to destination
void copy(char *source, char *destination)
{

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
    }
}

int main(int argc, char **argv) {

    if (argc > 2)
    {
        cp(argc - 1, argv + 1);
    }
    else {
        fprintf(stderr, "mycp: invalid usage\n");
        return EXIT_FAILURE;
    }
}
