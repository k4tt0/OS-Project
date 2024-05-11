#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <stdlib.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>

# define CURRENT_DIR "/."
# define PREVIOUS_DIR "/.."
# define VSCODE_DIR "/.vscode"

# define BUFFER_SIZE 1024
# define STR_SIZE 512
# define ARR_SIZE 20

# define OUTPUT_FILE "/snapshots.txt"

typedef struct {
    char snapshot_timestamp[STR_SIZE];
    char entry_name[STR_SIZE];
    char size[STR_SIZE];
    char last_modified_time[STR_SIZE];
    char permissions[STR_SIZE];
    char inode_nr[STR_SIZE];
}Entry;


typedef struct {
    Entry entries_snapshots[ARR_SIZE];
}Snapshots;

void getSnapshotForGivenDir(char *name, int *files_count, Snapshots *snapshots);

#endif
