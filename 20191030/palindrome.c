#include <stdio.h>
#include <stdint.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdbool.h>

int get_file_size(int fd) {
    off_t sz = lseek(fd, 0, SEEK_END);
    lseek(fd, 0, SEEK_SET);
    return sz;
}


char char_at_buf[4];

char char_at(int fd, int pos) {
    lseek(fd, pos, SEEK_SET);
    read(fd, char_at_buf, 1);
    return char_at_buf[0];
}

bool is_palindrome(int fd) {
    int n = get_file_size(fd);
    for (int i = 0; i < n-1-i; i++) {
        if (char_at(fd, i) != char_at(fd, n-1-i)) {
            return false;
        }
    }
    return true;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Must have exactly one argument!");
    }
    char* filename = argv[1];
    int fd = open(filename, O_RDONLY);

    int32_t file_size = get_file_size(fd);

    printf("size: %d\n", file_size);

    bool p = is_palindrome(fd);

    close(fd);

    return 1 - p;
}
