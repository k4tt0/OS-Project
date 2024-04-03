#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdlib.h>
#include <time.h>

# define CURRENT_DIR "/."
# define PREVIOUS_DIR "/.."
# define VSCODE_DIR "/.vscode"

# define SIZE 2048

typedef struct {
    char file_details[SIZE];
}Snapshot;

// #TODO: Function that compares two snapshots

void getSnapshot(Snapshot *snapshots, int *snapshots_count, char *file_name) {
    strcpy(snapshots[(*snapshots_count)].file_details, file_name);

    *snapshots_count += 1;
}

void parseDir(char *name, int indent, Snapshot *snapshots, int *snapshots_count) {
    DIR *dir;
    struct dirent *entry;
    struct stat entry_stat;


    if(!(dir = opendir(name))) return;

    while ((entry = readdir(dir)) != NULL) {
        for (int i = 0; i <= indent - 1; i++) {
            printf("\t");
        } 

        char path[SIZE];
        sprintf(path, "%s/%s", name, entry->d_name);

        
        // lstat() is identical to stat(), except that if pathname is a symbolic link, then it returns information about the link itself, not the file that the link refers to
        if ((lstat(path, &entry_stat)) == 0) {
            time_t mod_time = entry_stat.st_mtim.tv_sec;
            char last_modified_time_buffer[26], buffer[SIZE];
            ctime_r(&mod_time, last_modified_time_buffer);


            if (S_ISDIR(entry_stat.st_mode)) {
                sprintf(buffer, "Size: %ld | %s -> dir | Last modified: %s\n", entry_stat.st_size, entry->d_name, last_modified_time_buffer); 
                // printf("%s\n", buffer);

                if ((strstr(path, CURRENT_DIR)) || (strstr(path, PREVIOUS_DIR)) || (strstr(path, VSCODE_DIR))) continue; 

                getSnapshot(snapshots, snapshots_count, buffer);
                parseDir(path, indent + 2, snapshots, snapshots_count);
            }
            else if (S_ISLNK(entry_stat.st_mode)) {
                sprintf(buffer, "Size: %ld | %s -> link | Last modified: %s", entry_stat.st_size, entry->d_name, last_modified_time_buffer); 
                getSnapshot(snapshots, snapshots_count, buffer);
                // printf("%s", buffer);
            }
            else if (S_ISREG(entry_stat.st_mode)) {
                sprintf(buffer, "Size: %ld | %s -> regular file | Last modified: %s", entry_stat.st_size, entry->d_name, last_modified_time_buffer); 
                getSnapshot(snapshots, snapshots_count, buffer);
                // printf("%s", buffer);
            }
            
        }
        else {
            printf("%s\n", strerror(errno));
        }

        // printf("\n");
    }
    closedir(dir);
}


int main(int argc, char **argv) {
    if (argc != 2) {
        printf("Usage ./main dir\n");
        exit(1);
    }

    Snapshot snapshots[1000];
    int files_count = 0;

    parseDir(argv[1], 0, snapshots, &files_count);

    return 0;
}