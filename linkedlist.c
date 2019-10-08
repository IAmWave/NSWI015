#include <stdio.h>
#include <stdlib.h>

typedef struct List List;

struct List {
  char* val;
  List* next;
};

int main(int argc, char** argv) {
  List l = {NULL, NULL};
  List* cur = &l;
  for (int i = 0; i < argc; i++) {
    cur->val = argv[i];
    if (i < argc - 1) {
      cur->next = malloc(sizeof(List));
      cur = cur->next;
    }
  }

  cur = &l;

  while (cur != NULL) {
    printf("%s\n", cur->val);
    List* to_free = cur;
    cur = cur->next;
    if (to_free != &l) {
      free(to_free);
    }
  }
}