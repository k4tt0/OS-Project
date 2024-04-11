// Description: The proposed project combines functionalities for monitoring a directory to manage differences
// between two captures (snapshots) of it. The user will be able to observe and intervene in the changes in the monitored directory.
// Directory Monitoring:
// The user can specify the directory to be monitored as an argument in the command line, and the program will track changes occurring in it and its subdirectories, 
// parsing recursively each entry from the directory.
// With each run of the program, the snapshot of the directory will be updated, storing the metadata of each entry.

#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdlib.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>

# define CURRENT_DIR "/."
# define PREVIOUS_DIR "/.."
# define VSCODE_DIR "/.vscode"

# define STR_SIZE 512
# define ARR_SIZE 20

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



char *getEntryPermissions(struct stat fileStat, char *permissions_buffer) {

    sprintf(permissions_buffer, "%s%s%s%s%s%s%s%s%s%s", 
    (S_ISDIR(fileStat.st_mode)) ? "d" : "-",
    (fileStat.st_mode & S_IRUSR) ? "r" : "-",
    (fileStat.st_mode & S_IWUSR) ? "w" : "-",
    (fileStat.st_mode & S_IXUSR) ? "x" : "-",
    (fileStat.st_mode & S_IRGRP) ? "r" : "-",
    (fileStat.st_mode & S_IWGRP) ? "w" : "-",
    (fileStat.st_mode & S_IXGRP) ? "x" : "-",
    (fileStat.st_mode & S_IROTH) ? "r" : "-",
    (fileStat.st_mode & S_IWOTH) ? "w" : "-",
    (fileStat.st_mode & S_IXOTH) ? "x" : "-"
    );

    return permissions_buffer;
}

void createSnapshot(struct stat entry_stat, Entry *entries, int *snapshots_count, char *entry_name) {
    char last_modified_time_buffer[26], permissions_buffer[STR_SIZE];
    time_t mod_time = entry_stat.st_mtim.tv_sec;

    ctime_r(&mod_time, last_modified_time_buffer);
    if (last_modified_time_buffer[strlen(last_modified_time_buffer) - 1] == '\n') {
        last_modified_time_buffer[strlen(last_modified_time_buffer) - 1] = '\0';
    }
    //ctime_r puts a newline charcter at the end of last_modified_time_buffer, this how we get rid of it 

    time_t t = time(NULL);
    struct tm tm = *localtime(&t);        
    
    sprintf(entries[*snapshots_count].entry_name, "%s", entry_name);
    sprintf(entries[*snapshots_count].size, "%ld", entry_stat.st_size);
    sprintf(entries[*snapshots_count].inode_nr, "%ld", entry_stat.st_ino);
    sprintf(entries[*snapshots_count].last_modified_time, "%s", last_modified_time_buffer);
    sprintf(entries[*snapshots_count].permissions, "%s", getEntryPermissions(entry_stat, permissions_buffer));
    sprintf(entries[*snapshots_count].snapshot_timestamp, "%02d-%02d-%04d %02d:%02d:%02d",tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900, tm.tm_hour, tm.tm_min, tm.tm_sec);

    *snapshots_count += 1;
}

void writeSnapshotToFile(int fd, Entry entry) {
    char buffer[STR_SIZE * 8];
    sprintf(buffer,
    "Snapshot for entry %s\nEntry name: %s\nTimestamp: %s\nSize: %s\nLast Modified: %s\nPermissions: %s\nInode no: %s\n\n",
    entry.entry_name,
    entry.entry_name,
    entry.snapshot_timestamp,
    entry.size,
    entry.last_modified_time,
    entry.permissions,
    entry.inode_nr
    );

    write(fd, buffer, sizeof(buffer));

}

// #TODO: Function that compares two snapshots
void compareSnapshots(char *snapshots_path, char *dir_name, Entry *entries, int entries_len) {
    int fd;
    char buffer[STR_SIZE * 8];

    if ((access(snapshots_path, F_OK)) == 0) {
        // if a snapshot file exits the contents of said 
        // file should be compared to what is curently stored in snapshots.entries_snapshots
         if ((fd = open(snapshots_path, O_RDONLY)) == -1) {
            printf("Error occurred when opening file!\n");
            exit(1);
        }
        printf("Comparing changes...\n");

        for (int i = 0; i < entries_len; i++) {
            

            while(read(fd, buffer, sizeof(buffer)) > 0){
                if (strstr(buffer, entries->entry_name)) {
                    
                    // printf("%s\n", buffer);
                    // TODO: To be continued
                }
            }

        }

    }
    else {
        // if a previous snapshots file doesn t exist then we create
        //  one and populate it with data fromsnapshots.entries_snapshots 
        printf("Previous snapshots file doesn t exist!\nCreating snapshot file...\n");

        if ((fd = open(snapshots_path, O_WRONLY | O_CREAT | O_TRUNC, S_IWUSR)) == -1) {
            printf("Error occurred when opening file!\n");
            exit(1);
        }

        char buffer[STR_SIZE];
        sprintf(buffer, "//Snapshots for %s dir\n", dir_name);
        write(fd, buffer, strlen(buffer));

        for (int i = 0; i < entries_len; i++) {
            writeSnapshotToFile(fd, entries[i]);
        }
    }

    close(fd);
}

void parseDir(char *name, int indent, Snapshots *snapshots, int *snapshots_count) {
    DIR *dir;
    struct dirent *entry;
    struct stat entry_stat;


    if(!(dir = opendir(name))) return; 

    while ((entry = readdir(dir)) != NULL) {
        // for (int i = 0; i <= indent - 1; i++) {
        //     printf("\t");
        // } 

        char path[STR_SIZE];
        sprintf(path, "%s/%s", name, entry->d_name);


        // lstat() is identical to stat(), except that if pathname is a symbolic link, then it returns information about the link itself, not the file that the link refers to
        if ((lstat(path, &entry_stat)) == 0) {

            
            if (S_ISDIR(entry_stat.st_mode)) {

                if ((strstr(path, CURRENT_DIR)) || (strstr(path, PREVIOUS_DIR)) || (strstr(path, VSCODE_DIR))) continue; 
                
                parseDir(path, indent + 2, snapshots, snapshots_count);
            }
            
            createSnapshot(entry_stat, snapshots->entries_snapshots, snapshots_count, entry->d_name);
        }
        else {
            printf("%s\n", strerror(errno));
        }

    }

    closedir(dir);
}


int main(int argc, char **argv) {
    // if (argc != 2) {
    //     printf("Usage ./main dir\n");
    //     exit(1);
    // }
    // TODO: Implement this for multiple folders

    Snapshots snapshots;
    int files_count = 0;

    parseDir("./test", 0, &snapshots, &files_count);

    compareSnapshots("./snapshots.txt", "./test", snapshots.entries_snapshots, files_count);

    return 0;
}
