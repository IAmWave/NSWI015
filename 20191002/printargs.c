#include <stdio.h>

void loop1(int argc, char** argv) {
  for (int i = 0; i < argc; i++) {
    printf("%s ", argv[i]);
  }
  printf("\n");
}

void loop2(char** argv) {
  while (argv[0] != NULL) {
    printf("%s ", argv[0]);
    argv = argv + 1;
  }
  printf("\n");
}

void rec(char** argv) {
  if (argv[0] == NULL) {
    printf("\n");
  } else {
    printf("%s ", argv[0]);
    rec(argv + 1);
  }
}

int main(int argc, char** argv) {
  loop1(argc, argv);
  loop2(argv);
  rec(argv);
}
