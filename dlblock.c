#include "dlblock.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

int main(int argc, char **argv)
{
    if (argc == 1) {
        fprintf(stderr, "usage: %s [-b block-list] command [args]\n", argv[0]);
        return 1;
    }

    int cmd_start = 1;
    if (argc > 1 && strcmp(argv[1], "-b") == 0) {
        if (argc <= 2) {
            fprintf(stderr, "missing -b value\n");
            return 1;
        }

        char *env = argv[2];
        if (setenv(ENV, env, 1) != 0) {
            fprintf(stderr, "setenv: ");
            perror(NULL);
            return 1;
        }

        cmd_start = 3;
    }

    if (cmd_start >= argc) {
        fprintf(stderr, "missing cmd\n");
        return 1;
    }

    char *paths[] = {"./libdlblock.so", "/usr/local/lib/libdlblock.so", "/usr/lib/libdlblock.so"};
    char *path = NULL;

    for (size_t i = 0; i < sizeof(paths) / sizeof(char*); i++) {
        struct stat st;
        if (stat(paths[i], &st) == 0) {
            path = paths[i];
            break;
        }
    }

    if (path == NULL) {
        fprintf(stderr, "failed to find libdlblock.so\n");
        return 1;
    }

    if (setenv("LD_PRELOAD", path, 1) != 0) {
        fprintf(stderr, "setenv: ");
        perror(NULL);
        return 1;
    }

    if (execvp(argv[cmd_start], &argv[cmd_start]) == -1) {
        fprintf(stderr, "execvp: %s: ", argv[cmd_start]);
        perror(NULL);
        return 1;
    }
}
