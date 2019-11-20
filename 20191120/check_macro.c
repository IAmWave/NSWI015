#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define debug(format, ...) fprintf(stderr, "%d: " format, getpid(), ##__VA_ARGS__)

#define checked(cond, call, msg)                      \
    {                                                 \
        _res = call;                                  \
        if (_res cond) {                              \
            fprintf(stderr, "%s (%d)\n", msg, errno); \
        }                                             \
    }                                                 \
    _res

// Useful macro:
// https://stackoverflow.com/questions/41490810/how-to-properly-use-lseek-to-extend-file-size

int main(int argc, char* argv[]) {
    if (argc != 2) {
        exit(1);
    }
    char* filename = argv[1];
    int _res;
    int fd = (2, 3);
    //int fd = checked(!= -1, open(filename, O_RDWR), "Failed to open");
    printf("Opened %d\n", fd);
    close(fd);
}
