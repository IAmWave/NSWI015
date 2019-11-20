#include <assert.h>
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

int main() {
    // pid_t pid = getpid();
    // printf("%d\n", pid);
    const char* PATH = "foo.txt";
    int fd = open(PATH, O_RDWR);
    assert(fd != -1);
    int filesize = lseek(fd, 0, SEEK_END);
    assert(filesize != (off_t)-1);

    pid_t fork_pid, parent_pid = getpid(), child_pid;
    bool parent;
    int pd[2];
    int fd2;
    pipe(pd);
    switch (fork_pid = fork()) {
        case -1:
            printf("Error when forking\n");
            exit(1);
        case 0:
            parent = false;
            child_pid = getpid();
            close(0);
            fd2 = dup(pd[0]);
            assert(fd2 == 0);
            close(pd[0]);
            close(pd[1]);
            break;
        default:
            parent = true;
            child_pid = fork_pid;
            close(1);
            fd2 = dup(pd[1]);
            assert(fd2 == 1);
            close(pd[0]);
            close(pd[1]);
    }

    fprintf(stderr, "P %d C %d\n", parent_pid, child_pid);
    if (parent) {
        fprintf(stderr, "Parent writing, fd=%d\n", fd);
        char* addr = mmap(NULL, filesize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
        assert(addr != MAP_FAILED);
        char* to_write = "abc";
        addr[0] = strlen(to_write);
        strncpy(addr + 1, to_write, filesize);

        printf("a\n");
        fflush(stdout);

        int stat_loc;
        wait(&stat_loc);
        if (WIFSIGNALED(stat_loc)) {
            fprintf(stderr, "Child terminated with signal %d\n", WTERMSIG(stat_loc));
        }
        fprintf(stderr, "Parent exiting %d\n", WEXITSTATUS(stat_loc));
        munmap(addr, filesize);
    } else {
        printf("Child reading\n");
        char c;
        scanf(" %c", &c);

        char* addr = mmap(NULL, filesize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
        assert(addr != MAP_FAILED);
        int len = (int)addr[0];
        debug("Len: %d\n", len);
        debug("Read string: %.*s\n", len, addr + 1);

        munmap(addr, filesize);
    }
    close(fd);
}
