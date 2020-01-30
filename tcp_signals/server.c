/*
 * Trivial TCP echo server, single threaded.  Run like this:
 *
 * server$ ./a.out 4444
 *
 * client$ nc localhost 4444
 * <type anything, it will be printed on the server side and here as well>
 * <type anything, it will be printed on the server side and here as well>
 * ...
 *
 * Note: this example is IPv4-only !
 *
 * Beware of missing SO_REUSEADDR.  See tcp-sink-server.c on how to use it.  Do
 * not kill this "server" if there are active connections otherwise you will see
 * "bind: Address already in use" on restart.
 *
 * (c) jp@devnull.cz
 */

#include <err.h>
#include <libgen.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>

#define BUF_LEN 100

int main(int argc, char **argv) {
    int fd, newsock, n;
    struct sockaddr_in in;

    if (argc < 2) errx(1, "usage: %s <port>", basename(argv[0]));

    memset(&in, 0, sizeof(in));
    in.sin_family = AF_INET;
    in.sin_port = htons(atoi(argv[1]));
    in.sin_addr.s_addr = htons(INADDR_ANY);

    if ((fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        err(1, "socket");
    }

    /* So that we can use the port immediately again after restart. */
    int optval = 1;
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1) {
        err(1, "setsockopt");
    }

    if (bind(fd, (struct sockaddr *)&in, sizeof(in)) == -1) {
        err(1, "bind");
    }
    if (listen(fd, 0) == -1) {
        err(1, "listen");
    }

    while (1) {
        char buf[BUF_LEN];

        if ((newsock = accept(fd, NULL, 0)) == -1) {
            err(1, "accept");
        }

        (void)fprintf(stderr, "-- connection accepted --\n");
        while ((n = read(newsock, buf, BUF_LEN)) != 0) {
            if (n == -1) {
                err(1, "read");
            }
            int signal, pid;
            if (sscanf(buf, "%d %d", &pid, &signal) != 2) {
                fprintf(stderr, "Expected two integers, pid and signal\n");
                continue;
            }
            memset(buf, 0, sizeof(buf));
            printf("Sending signal %d to PID %d\n", signal, pid);
            kill(pid, signal);
        }

        (void)close(newsock);
        (void)fprintf(stderr, "-- connection closed --\n");
    }

    return (0);
}
