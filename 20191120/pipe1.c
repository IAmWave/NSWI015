#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>

int main() {
    //pid_t pid = getpid();
    //printf("%d\n", pid);
    pid_t fork_pid, parent_pid = getpid(), child_pid;
    bool parent;
    int pd[2];
    int fd;
    pipe(pd);
    switch(fork_pid = fork()) {
        case -1:
            printf("Error when forking\n");
            exit(1);
        case 0:
            parent = false;
            child_pid = getpid();
            close(0);
            fd = dup(pd[0]);
            assert(fd == 0);
            close(pd[0]);
            close(pd[1]);
            break;
        default:
            parent=true;
            child_pid = fork_pid;
            close(1);
            fd = dup(pd[1]);
            assert(fd == 1);
            close(pd[0]);
            close(pd[1]);
    }
    
    fprintf(stderr, "P %d C %d\n", parent_pid, child_pid);
    if (parent) {
        fprintf(stderr, "Parent waiting\n");
        printf("a\n");
        fflush(stdout);
        int stat_loc;
        wait(&stat_loc);
        if (WIFSIGNALED(stat_loc)) {
            fprintf(stderr, "Child terminated with signal %d\n", WTERMSIG(stat_loc));
        }
        fprintf(stderr, "Parent exiting %d\n", WEXITSTATUS(stat_loc));
    } else {
        printf("Child reading\n");
        char c;
        scanf(" %c", &c);
        return (int)c;
    }
}
