#include <stdio.h>
#include <assert.h>

int main() {
    short x = 1;
    assert(sizeof(short)==2);
    char c0 = *(&x);
    char c1 = *(&x+1);
    if (c0 == 0 && c1 == 1) {
        printf("big endian\n");
    } else if (c0 == 1 && c1 == 0) {
        printf("little endian\n");
    } else {
        printf("something is amiss\n");
    }
}
