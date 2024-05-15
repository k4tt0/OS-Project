#include <stdio.h>
#include "functions.h"

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

void createSnapshotData(struct stat entry_stat, Entry *entries, int *snapshots_count, char *entry_name) {
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
    // strcat(entries[*snapshots_count].entry_name, "\0");

    sprintf(entries[*snapshots_count].size, "%ld", entry_stat.st_size);
    // strcat(entries[*snapshots_count].size, "\0");

    sprintf(entries[*snapshots_count].inode_nr, "%ld", entry_stat.st_ino);
    // strcat(entries[*snapshots_count].inode_nr, "\0");

    sprintf(entries[*snapshots_count].last_modified_time, "%s", last_modified_time_buffer);
    // strcat(entries[*snapshots_count].last_modified_time, "\0");

    sprintf(entries[*snapshots_count].permissions, "%s", getEntryPermissions(entry_stat, permissions_buffer));
    // strcat(entries[*snapshots_count].permissions, "\0");

    sprintf(entries[*snapshots_count].snapshot_timestamp, "%02d-%02d-%04d %02d:%02d:%02d",tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900, tm.tm_hour, tm.tm_min, tm.tm_sec);
    // strcat(entries[*snapshots_count].snapshot_timestamp, "\0");

    *snapshots_count += 1;
}

void writeEntryToFile(int fd, Entry entry) {
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

    write(fd, buffer, strlen(buffer));

}

void updateSnapshots(char *snapshots_path, char *dir_name, Entry *entries, int entries_len) {
    int fd;


    if ((access(snapshots_path, F_OK)) == 0) {
        printf("Updating snapshot...\n");
    }
    else {
        // if a previous snapshots file doesn t exist then we create
        //  one and populate it with data fromsnapshots.entries_snapshots
        printf("Previous snapshot file doesn t exist!\nCreating snapshot file...\n");

    }

    if ((fd = open(snapshots_path, O_WRONLY | O_CREAT | O_TRUNC, S_IWUSR)) == -1) {
        printf("Error occurred when opening file!\n");
        exit(1);
    }

    char buffer[STR_SIZE];
    sprintf(buffer, "//Snapshots for %s dir\n", dir_name);
    write(fd, buffer, strlen(buffer));

    for (int i = 0; i < entries_len; i++) {
        writeEntryToFile(fd, entries[i]);
    }

    close(fd);
}

bool checkIfEntryMissingAllPermissions(struct stat entry) {
    char permissions_buffer[BUFFER_SIZE];

    if (strcmp(getEntryPermissions(entry, permissions_buffer), "-r-xr-xr-x") == 0) return true;

    return false;
}

void parseDir(char *name, int indent, Snapshots *snapshots, int *snapshots_count) {
    DIR *dir;
    struct dirent *entry;
    struct stat entry_stat;
    int returnCode;
    pid_t pid;
    // char buffer[100];


    if(!(dir = opendir(name))) return; 

    while ((entry = readdir(dir)) != NULL) {
        char path[STR_SIZE];
        sprintf(path, "%s/%s", name, entry->d_name);


        if ((lstat(path, &entry_stat)) == 0) {

            if (S_ISDIR(entry_stat.st_mode)) {
                if ((strstr(path, CURRENT_DIR)) || (strstr(path, PREVIOUS_DIR)) || (strstr(path, VSCODE_DIR))) continue; //sari peste dir ul curent (./.), peste dir ul anterior (./..) si peste dir ul de la vscode (./.vscode)

                pid = fork();
                if (pid == -1) {
                    printf("Error creating process!\n");
                    exit(1);
                }
                else if (pid == 0) {
                    parseDir(path, indent + 2, snapshots, snapshots_count);
                    exit(0);
                }
            }
            
                int pipefd[2];

            if (pipe(pipefd) == -1) {
                printf("Error creating pipe.\n");
                exit(1);
            }

            if (S_ISREG(entry_stat.st_mode) && checkIfEntryMissingAllPermissions(entry_stat)) {
                pid_t filesyntpid;

                filesyntpid = fork();
                if (filesyntpid == -1) {
                    printf("Error creating process!\n");
                    exit(1);
                }
                else if (filesyntpid == 0) {
                    // close(pipefd[0]);
                    // dup2(pipefd[1], STDOUT_FILENO);
                    // close(pipefd[1]);


                    execl("./fileSyntacticAnalysis.sh", "./fileSyntacticAnalysis.sh", path, NULL);
                    exit(0);
                }
                // else {

       
                // }
                    // close(pipefd[1]); // Close the write end of the pipe
                    // read(pipefd[0], buffer, sizeof(buffer));
                    // close(pipefd[0]); // Close the read end of the pipe
            }

            createSnapshotData(entry_stat, snapshots->entries_snapshots, snapshots_count, entry->d_name);
        }
        else {
            printf("%s\n", strerror(errno));
        }

    }

    wait(&returnCode);
    printf("The process with PID %d has ended with code %d.\n", pid, returnCode);
    // printf("%s", buffer);

    closedir(dir);
}

void getSnapshotForGivenDir(char *name, int *files_count, Snapshots *snapshots) {
    char buffer[BUFFER_SIZE];

    strcpy(buffer, name);
    strcat(buffer, OUTPUT_FILE);
    
    parseDir(name, 0, snapshots, files_count);
    updateSnapshots(buffer, name, snapshots->entries_snapshots, *files_count);
}
