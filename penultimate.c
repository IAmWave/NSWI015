#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/types.h>

int main() {
    ssize_t nread;
    size_t len = 0;
    char* line = NULL;

    while ((nread = getline(&line, &len, stdin)) != -1) {
        printf("Retrieved line of length %zu:\n", nread);
        fwrite(line, nread, 1, stdout);
    }
    free(line);
}
