#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdlib.h>

int main() {
    //pid_t pid = getpid();
    //printf("%d\n", pid);
    pid_t fork_pid, parent_pid = getpid(), child_pid;
    bool parent;
    switch(fork_pid = fork()) {
        case -1:
            printf("Error when forking\n");
            exit(1);
        case 0:
            parent = false;
            child_pid = getpid();
            break;
        default:
            parent=true;
            child_pid = fork_pid;
    }
    printf("P %d C %d\n", parent_pid, child_pid);
    if (parent) {
        printf("Parent waiting\n");
        int stat_loc;
        wait(&stat_loc);
        if (WIFSIGNALED(stat_loc)) {
            printf("Child terminated with signal %d\n", WTERMSIG(stat_loc));
        }
        printf("Parent exiting\n");
    } else {
        printf("Child sleeping\n");
        while(1) {
            sleep(1);
        }
    }
}
