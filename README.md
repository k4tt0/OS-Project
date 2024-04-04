The proposed project combines functionalities for monitoring a directory to manage differences between two captures (snapshots) of it. The user will be able to observe and intervene in the changes in the monitored directory.
Directory Monitoring:
The user can specify the directory to be monitored as an argument in the command line, and the program will track changes occurring in it and its subdirectories, parsing recursively each entry from the directory.
With each run of the program, the snapshot of the directory will be updated, storing the metadata of each entry. 
The functionality of the program will be updated to allow it to receive an unspecified number of arguments (directories) in the command line. The logic for capturing metadata will now apply to all received arguments, meaning the program will update snapshots for all directories specified by the user.

For each entry of each directory provided as an argument, the user will be able to compare the previous snapshot of the specified directory with the current one. If there are differences between the two snapshots, the old snapshot will be updated with the new information from the current snapshot.

The functionality of the code will be expanded so that the program receives an additional argument, representing the output directory where all snapshots of entries from the specified directories in the command line will be stored. This output directory will be specified using the `-o` option. For example, the command to run the program will be: `./program_exe -o output input1 input2 ...`. 
