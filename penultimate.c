#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

int main() {
  ssize_t nread;
  size_t len = 0;
  char* lines[2];

  for (int i = 0; i < 2; i++) {
    lines[i] = malloc(1024 * sizeof(*lines));
  }

  int cur = 1;
  while ((nread = getline(&lines[cur], &len, stdin)) != -1) {
    cur = (cur + 1) % 2;
  }
  printf("%s", lines[cur]);

  for (int i = 0; i < 2; i++) {
    free(lines[i]);
  }
}
