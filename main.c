#include <stdio.h>
#include "functions.h"

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("Usage ./main dir1 dir2 ...\n");
        exit(1);
    }

    Snapshots snapshots;
    int files_count = 0;

    for (int i  = 1; i < argc; i++) {
        getSnapshotForGivenDir("./test", &files_count, &snapshots);
    }

    return 0;
}
