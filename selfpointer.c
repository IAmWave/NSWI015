#include <stdio.h>

int main(int argc, char** argv) {
	size_t p = 3;
	p = &p;
	printf("%zu\n", p);
}
