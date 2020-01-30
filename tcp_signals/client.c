/*
 * Simple TCP client program. Use the 1st command line argument to set the IP
 * address (do not use domain names), and the 2nd one to set the port. Read from
 * the standard input and send the data to the other side. We do not read
 * anything from the remote side.
 *
 * based on tcp/pconnect.c, (c) jp@devnull.cz
 */

//#define	_XOPEN_SOURCE	700

#include <arpa/inet.h>
#include <err.h>
#include <memory.h>
#include <netdb.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define BUF_LEN 100
#define MESSAGE "Signal caught!\n"
#define DEFAULT_PORT 1234
#define LOCALHOST "127.0.0.1"

void handle(int sig) {
    write(1, MESSAGE, sizeof(MESSAGE) - 1);
    exit(0);
    /*
     * Consider calling backtrace(3) after the signal is handled
     * to get stack trace information.
     */
}

int main(int argc, char **argv) {
    int fd;
    char buf[BUF_LEN];

    if (argc < 2 || argc > 3) {
        errx(1, "usage: %s signal [port]", argv[0]);
    }
    int signal = atoi(argv[1]);
    int port = (argc >= 3) ? atoi(argv[2]) : DEFAULT_PORT;

    struct sockaddr_in in = {0};
    in.sin_family = AF_INET;
    in.sin_port = htons(port);

    /*
     * Convert a dotted format to 4 bytes suitable for use in
     * sockaddr_in.
     */
    if (inet_pton(AF_INET, LOCALHOST, &(in.sin_addr)) != 1) {
        errx(1, "inet_pton failed for '%s'", argv[1]);
    }

    if ((fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        err(1, "socket");
    }

    if (connect(fd, (struct sockaddr *)&in, sizeof(in)) == -1) {
        err(1, "connect");
    }

    struct sigaction act = {0};
    act.sa_handler = handle;
    sigaction(signal, &act, NULL);

    memset(buf, 0, sizeof(buf));
    if (sprintf(buf, "%d %d\n", getpid(), signal) < 0) {
        err(1, "sprintf");
    }

    if (write(fd, buf, strlen(buf)) < 0) {
        close(fd);
        err(1, "write");
    }

    while (1) {
        sleep(10);
    }

    return 0;
}
