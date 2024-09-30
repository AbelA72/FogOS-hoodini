/*
#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "user/user.h"
#include "kernel/fs.h"

int mkfifo(const char *path, short major, short minor) {

    if (mknod(path, major, minor) < 0) {
        printf("Error: Could not create special file\n");
        return -1;
    }
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc < 4) {
        printf("Usage: mkfifo path major minor\n");
        exit(1);
    }

    const char *path = argv[1];
    short major = atoi(argv[2]);
    short minor = atoi(argv[3]);

    if (mkfifo(path, major, minor) < 0) {
        printf("Error: mknod failed\n");
        exit(1);
    }

    printf("Created FIFO file: %s\n", path);
    return 0;
}
*/
