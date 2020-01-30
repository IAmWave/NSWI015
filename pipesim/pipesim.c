#include <err.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/errno.h>
#include <sys/param.h>
#include <unistd.h>

#define check_errno(message) \
    if (errno != 0) {        \
        err(1, message);     \
    }
#define DIRNAME "/etc/"
#define BASENAME "services"
#define BUFFER_SIZE 1024

void copy_to_cwd() {
    char cwd[MAXPATHLEN];
    getcwd(cwd, MAXPATHLEN);
    check_errno("getcwd");

    int fd_from = open(DIRNAME BASENAME, O_RDONLY);
    check_errno("open");

    int fd_to = creat("./" BASENAME, 0666);
    check_errno("creat");

    char buf[BUFFER_SIZE];
    int n;
    while ((n = read(fd_from, buf, BUFFER_SIZE)) != 0) {
        check_errno("read");
        write(fd_to, buf, n);
        check_errno("write");
    }

    close(fd_from);
    check_errno("close");
    close(fd_to);
    check_errno("close");
}

const char* commands[][3] = {{"cat", "services"}, {"grep", "tcp"}, {"wc", "-l"}};

int main() {
    copy_to_cwd();

    int in_fds[3], out_fds[3];
    in_fds[0] = 0;   // stdin
    out_fds[2] = 1;  // stdout

    for (int i = 0; i < 2; i++) {
        int pd[2];
        pipe(pd);
        check_errno("pipe");
        out_fds[i] = pd[1];
        in_fds[i + 1] = pd[0];
    }
    // out_fds[1] = 1;  // stdout

    for (int i = 0; i < 3; i++) {
        printf("%d in: %d, out: %d\n", i, in_fds[i], out_fds[i]);
    }

    for (int i = 0; i < 3; i++) {
        int pid;
        switch (pid = fork()) {
            case -1:
                err(1, "fork");
                break;
            case 0:         // child
                errno = 0;  // hack
                printf("Process %d, %s %s\n", getpid(), commands[i][0], commands[i][1]);

                if (in_fds[i] != 0) {
                    close(0);
                    check_errno("close1");
                    dup(in_fds[i]);
                    check_errno("dup");
                    // close(in_fds[i]);
                    check_errno("close2");
                }
                if (out_fds[i] != 1) {
                    close(1);
                    check_errno("close3");
                    dup(out_fds[i]);
                    check_errno("dup");
                    // close(out_fds[i]);
                    check_errno("close4");
                }
                close(in_fds[1]);
                close(in_fds[2]);
                close(out_fds[1]);
                close(out_fds[0]);
                check_errno("close!");

                execlp(commands[i][0], commands[i][0], commands[i][1], NULL);
                err(1, "execlp");
                break;
            default:  // parent
                check_errno("parent");
                continue;
        }
    }

    close(in_fds[1]);
    close(in_fds[2]);
    close(out_fds[1]);
    close(out_fds[0]);
    pid_t wpid;
    int status;
    while ((wpid = wait(&status)) > 0)
        ;  // this way, the father waits for all the child processes
    /*for (int i = 0; i < 3; i++) {
        int stat_loc;
        pid_t wpid;
        wpid = wait(&stat_loc);
        if (wpid == -1) {
            err(1, "wait");
        }
        printf("%d done\n", wpid);
    }*/
    printf("main process done\n");
}
