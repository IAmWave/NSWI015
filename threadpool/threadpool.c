#include <err.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/errno.h>
#include <unistd.h>

pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int active_threads = 0;

void *thread(void *x) {
    int *id = (int *)x;
    printf("Thread %d starting\n", *id);
    sleep(1 + rand() % 10);

    printf("Thread %d exiting\n", *id);
    pthread_mutex_lock(&mutex);
    *id = 0;
    active_threads--;
    printf("Signalling to main!\n");
    pthread_mutex_unlock(&mutex);
    pthread_cond_signal(&cond);
    return (NULL);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "usage: %s n_threads", argv[0]);
    }
    int n_threads = strtol(argv[1], NULL, 10);
    if (errno != 0) {
        err(1, "strtol");
    }
    printf("n_threads: %d\n", n_threads);

    pthread_t threads[n_threads];
    int ids[n_threads];
    for (int i = 0; i < n_threads; i++) {
        ids[i] = 0;
    }

    for (int id = 1; true; id++) {
        pthread_mutex_lock(&mutex);
        while (active_threads >= n_threads) {
            printf("Main thread waiting\n");
            pthread_cond_wait(&cond, &mutex);
        }
        int slot = 0;
        while (ids[slot] != 0) {
            slot++;
            if (slot >= n_threads) {
                errx(1, "No free slot found!");
            }
        }
        printf("Spawning thread with id %d\n", id);
        ids[slot] = id;
        active_threads++;
        pthread_mutex_unlock(&mutex);
        pthread_create(&threads[slot], NULL, thread, &ids[slot]);
    }
}
