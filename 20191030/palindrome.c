#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdbool.h>

off_t ch_seek(int fd, off_t offset, int whence) {
    off_t sz = lseek(fd, offset, whence);
    if (sz == (off_t) -1) exit(1);
    return sz;
}

int get_file_size(int fd) {
    off_t sz = ch_seek(fd, 0, SEEK_END);
    ch_seek(fd, 0, SEEK_SET);
    return sz;
}

char char_at_buf[4];

char char_at(int fd, int pos) {
    ch_seek(fd, pos, SEEK_SET);
    int res = read(fd, char_at_buf, 1);
    if (res == 0 || res == -1) exit(1);
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
    if (fd == -1) return 1;

//    int32_t file_size = get_file_size(fd);
//    printf("size: %d\n", file_size);

    bool p = is_palindrome(fd);

    int res = close(fd);
    if (res != 0) return 1;

    return 1 - p;
}
