#define _POSIX_C_SOURCE 200809L
#define _GNU_SOURCE

#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define debug(format, ...) fprintf(stderr, "%d: " format, getpid(), ##__VA_ARGS__)
//#define debug(format, ...) fprintf(stderr, format, __VA_ARGS__)
//#define debug(format) fprintf(stderr, format)

void job() {
    srand(getpid());
    debug("Child starting\n");
    sleep(1 + rand() % 5);
    debug("Child exiting\n");
}

void spawn_child() {
    pid_t fork_pid;
    bool parent;
    switch (fork_pid = fork()) {
        case -1:
            debug("Error when forking\n");
            exit(1);
        case 0:
            parent = false;
            break;
        default:
            parent = true;
    }
    if (!parent) {
        job();
        exit(0);
    }
}

void write_pid(char* out) {
    int fd = open(out, O_WRONLY);
    if (fd == -1) {
        debug("Opening %s failed.\n", out);
        exit(1);
    }
    // char pid_s[16];
    // itoa((int)getpid(), pid_s, 10);
    // ssize_t size_written = write(fd, pid_s, strlen(pid_s));
    int size_written = dprintf(fd, "%d\n", getpid());
    if (size_written < 0) {
        debug("Error when writing");
        exit(1);
    }
    int ans = close(fd);
    if (ans == -1) {
        debug("Error when closing file");
        exit(1);
    }
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: processpool [n_processes] [fifo_path]\n");
    }
    int pool_size = atoi(argv[1]);
    char* fifo_path = argv[2];
    write_pid(fifo_path);
    // pid_t pid = getpid();
    // printf("%d\n", pid);
    int children = 0;
    while (true) {
        if (children < pool_size) {
            children++;
            spawn_child(children);
        } else {
            int stat_loc;
            wait(&stat_loc);
            if (WIFSIGNALED(stat_loc)) {
                printf("Child terminated with signal %d\n", WTERMSIG(stat_loc));
            }
            children--;
        }
    }
}
