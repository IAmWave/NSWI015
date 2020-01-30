#include <err.h>
#include <fcntl.h>
#include <libgen.h>
#include <netdb.h>
#include <netinet/in.h>
#include <poll.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define CMD_LEN 100

struct config_entry {
    int port;
    char portstr[10];
    char cmd[CMD_LEN];
};

#define MAX_SERVICES 50
int n_services = 0;
struct config_entry config[MAX_SERVICES];

#define BUFFER_SIZE 100

void load_config(char *path) {
    int fd;
    if ((fd = open(path, O_RDONLY)) == -1) {
        err(1, "open");
    }

    int il = 0, jl = 0;
    char lines[2][BUFFER_SIZE];
    char buf[BUFFER_SIZE];
    int n_read;
    while ((n_read = read(fd, buf, BUFFER_SIZE)) != 0) {
        if (n_read == -1) {
            err(1, "read");
        }
        for (int i = 0; i < n_read; i++) {
            if (buf[i] == '\n') {
                jl = 0;
                if (il == 1) {
                    il = 0;
                    config[n_services].port = atoi(lines[0]);
                    strncpy(config[n_services].portstr, lines[0], 10);
                    strncpy(config[n_services].cmd, lines[1], CMD_LEN);
                    config[n_services].cmd[CMD_LEN - 1] = 0;
                    n_services++;
                    memset(lines, 0, sizeof(lines));
                } else {
                    il++;
                }
            } else {
                if (il >= BUFFER_SIZE) {
                    errx(1, "Line too long!");
                }
                lines[il][jl] = buf[i];
                jl++;
            }
        }
    }
}

void start_service(int service_i, int fd) {
    pid_t fork_pid;
    switch (fork_pid = fork()) {
        case -1:
            err(1, "fork");
        case 0:
            break;
        default:
            // The parent goes back to the main loop
            return;
    }
    char buf[BUFFER_SIZE];

    int sock;
    if ((sock = accept(fd, NULL, 0)) == -1) {
        err(1, "accept");
    }

    fprintf(stderr, "Connection accepted, running %s\n", config[service_i].cmd);

    if (close(0) == -1) err(1, "close");
    if (close(1) == -1) err(1, "close");
    if (dup(sock) == -1) err(1, "dup");
    if (dup(sock) == -1) err(1, "dup");

    char *base = basename(config[service_i].cmd);
    if (base == NULL) err(1, "basename");
    execl(config[service_i].cmd, base, NULL);
    err(1, "execl");

    int n;
    while ((n = read(0, buf, BUFFER_SIZE)) != 0) {
        if (n == -1) {
            err(1, "read");
        }
        write(1, buf, n);
    }

    (void)close(sock);
    (void)fprintf(stderr, "-- connection closed --\n");
    exit(0);
}

int main(int argc, char **argv) {
    if (argc < 2) {
        errx(1, "usage: %s config_file", basename(argv[0]));
    }
    load_config(argv[1]);
    for (int i = 0; i < n_services; i++) {
        printf("%d %d %s\n", i, config[i].port, config[i].cmd);
    }

    struct pollfd fds[n_services];
    memset(fds, 0, sizeof(fds));

    for (int i = 0; i < n_services; i++) {
        struct sockaddr_storage ca;
        struct addrinfo *rorig, *r, hints;
        memset(&hints, 0, sizeof(hints));
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_flags = AI_PASSIVE;
        getaddrinfo(NULL, config[i].portstr, &hints, &rorig);

        for (r = rorig; r != NULL; r = r->ai_next) {
            if ((fds[i].fd = socket(r->ai_family, r->ai_socktype, r->ai_protocol)) == -1) {
                err(1, "socket");
            }
            /* So that we can use the port immediately again after restart. */
            int optval = 1;
            if (setsockopt(fds[i].fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1) {
                err(1, "setsockopt");
            }

            if (bind(fds[i].fd, r->ai_addr, r->ai_addrlen) == 0) {
                break;
            }
        }
        freeaddrinfo(rorig);
        if (r == NULL) {
            errx(1, "Could not bind");
        }
        if (listen(fds[i].fd, 0) == -1) {
            err(1, "listen");
        }
        fds[i].events = POLLIN;
        printf("FD: %d\n", fds[i].fd);
    }

    while (1) {
        if (poll(fds, n_services, -1) == -1) {
            err(1, "poll");
        }
        printf("Polled!\n");
        for (int i = 0; i < n_services; i++) {
            printf("%d: %d %d\n", i, fds[i].fd, fds[i].revents);
            if (fds[i].revents & POLLERR) {
                err(1, "poll");
            }

            if (fds[i].revents & POLLIN) {
                start_service(i, fds[i].fd);
            }
        }
        sleep(1);
    }

    return (0);
}
