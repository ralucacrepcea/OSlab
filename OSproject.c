#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include <dirent.h>

char* extractName(char* path) {
    char* filename = strrchr(path, '/');
    if (filename != NULL) {
        return filename + 1;
    } else {
        return path;
    }
}

// Function to print access rights
void printAccessRights(mode_t mode) {
    printf("\nUser:\n");
    printf("Read - %s\n", (mode & S_IRUSR) ? "yes" : "no");
    printf("Write - %s\n", (mode & S_IWUSR) ? "yes" : "no");
    printf("Exec - %s\n", (mode & S_IXUSR) ? "yes" : "no");

    printf("\nGroup:\n");
    printf("Read - %s\n", (mode & S_IRGRP) ? "yes" : "no");
    printf("Write - %s\n", (mode & S_IWGRP) ? "yes" : "no");
    printf("Exec - %s\n", (mode & S_IXGRP) ? "yes" : "no");

    printf("\nOthers:\n");
    printf("Read - %s\n", (mode & S_IROTH) ? "yes" : "no");
    printf("Write - %s\n", (mode & S_IWOTH) ? "yes" : "no");
    printf("Exec - %s\n", (mode & S_IXOTH) ? "yes" : "no");
}

// Function to print options for a regular file
void printRegularFileOptions() {
    printf("Options: \n");
    printf("\t -n name \n");
    printf("\t -d size \n");
    printf("\t -h hard link count \n");
    printf("\t -m time of last modification \n");
    printf("\t -a access rights \n");
    printf("\t -l create symbolic link \n");
    printf("Enter the option: ");
}

// Function to print options for a symbolic link
void printSymbolicLinkOptions() {
    printf("Options: \n");
    printf("\tn - name\n");
    printf("\tl - delete\n");
    printf("\td - size\n");
    printf("\tt - size of target file\n");
    printf("\ta - access rights\n");
    printf("Enter the option: ");
}

// Function to print options for a directory
void printDirectoryOptions() {
    printf("Options: \n");
    printf("\tn - name\n");
    printf("\td - size\n");
    printf("\ta - access rights\n");
    printf("\tc - number of files with the .c extension\n");
    printf("Enter the option: ");
}

// Function to print file information
void regularFileOptions(char *path, struct stat stats) {
    char link_name[100];

    printf("\nFile name: %s\n", extractName(path));

    if (S_ISREG(stats.st_mode)) {
        printf("File type: regular file\n");
    
        printRegularFileOptions();
        
        int isValid = 0;
        char options[10];
        do {
            if (isValid) {
                break;
            }
            isValid = 1;
    
            scanf("%9s", options);
    
            if (options[0] != '-' || strlen(options) < 2) {
                printf("Invalid option. Please enter a valid option.\n");
                isValid = 0;
            } else {
                for (int i = 1; i < strlen(options); i++) {
                    if (!strchr("ndhmal", options[i])) {
                        printf("Invalid option: %c. Please enter a valid option.\n", options[i]);
                        isValid = 0;
                        break;
                    }
                }
            }
    
            if (!isValid) {
                printRegularFileOptions();
            }
        } while (!isValid);


        printf("Selected options: %s\n", options);

        for (int i = 1; i < strlen(options); i++) {
            switch (options[i]) {
                case 'n':
                    printf("File name: %s\n", extractName(path));
                    break;
                case 'd':
                    printf("File size: %lld bytes\n", stats.st_size);
                    break;
                case 'h':
                    printf("Hard link count: %hu\n", stats.st_nlink);
                    break;
                case 'm':
                    printf("Time of last modification: %s", ctime(&stats.st_mtime));
                    break;
                case 'a':
                    printAccessRights(stats.st_mode);
                    break;
                case 'l':
                    printf("Enter the name of the symbolic link: ");
                    scanf("%s", link_name);

                    if (symlink(path, link_name) == -1) {
                        printf("Error creating symbolic link: %s\n", strerror(errno));
                    } else {
                        printf("Symbolic link created successfully!\n");
                    }

                    break;
                default:
                    break;
            }
        }
    } else if (S_ISDIR(stats.st_mode)) {
        printf("File type: directory\n");
    } else if (S_ISLNK(stats.st_mode)) {
        printf("File type: symbolic link\n");
    } else {
        printf("Unknown file type\n");
    }
}

// Function to print link information
void symbolicLinkOptions(char *path, struct stat stats) {
    printf("\nSymbolic link name: %s\n", extractName(path));

    if (S_ISLNK(stats.st_mode)) {
        printf("File type: symbolic link\n");
    
        printSymbolicLinkOptions();
        
        int isValid = 0;
        char options[10];
        do {
            if (isValid) {
                break;
            }
            isValid = 1;
    
            scanf("%9s", options);
    
            if (options[0] != '-' || strlen(options) < 2) {
                printf("Invalid option. Please enter a valid option.\n");
                isValid = 0;
            } else {
                for (int i = 1; i < strlen(options); i++) {
                    if (!strchr("ndtal", options[i])) {
                        printf("Invalid option: %c. Please enter a valid option.\n", options[i]);
                        isValid = 0;
                        break;
                    }
                }
            }
    
            if (!isValid) {
                printSymbolicLinkOptions();
            }
        } while (!isValid);


        printf("Selected options: %s\n", options);

        int deletedLink = 0;

        char target_path[1024];
        ssize_t len = readlink(path, target_path, sizeof(target_path) - 1);

        for (int i = 1; i < strlen(options) && !deletedLink; i++) {
            switch(options[i]) {
                case 'n':
                    printf("Symbolic link name: %s\n", extractName(path));
                    break;
                case 'd':
                    printf("Symbolic link size: %lld bytes\n", stats.st_size);
                    break;
                case 'l':
                    if (unlink(path) == -1) {
                        printf("Error deleting symbolic link: %s\n", strerror(errno));
                    } else {
                        printf("Symbolic link deleted successfully!\n");
                    }
                    deletedLink = 1;
                    break;
                case 'a':
                    printf("Access rights:");
                    printAccessRights(stats.st_mode);
                    break;
                case 't':
                    if (len == -1) {
                        printf("Error reading target path: %s\n", strerror(errno));
                    } else {
                        target_path[len] = '\0';
                        struct stat target_stats;
                        if (lstat(target_path, &target_stats) == -1) {
                            printf("Error getting target file stats: %s\n", strerror(errno));
                        } else {
                            printf("Size of target file: %lld bytes\n", target_stats.st_size);
                        }
                    }
                    break;
                default:
                    break; 
            }
        }
    } else if (S_ISDIR(stats.st_mode)) {
        printf("File type: directory\n");
    } else if (S_ISREG(stats.st_mode)) {
        printf("File type: regular file\n");
    } else {
        printf("Unknown file type\n");
    }
}


// Function to print directory information
void directoryOptions(char *path, struct stat stats) {
    printf("\nDirectory name: %s\n", extractName(path));

    if (S_ISDIR(stats.st_mode)) {
        printf("File type: directory\n");
    
        printDirectoryOptions();
        
        int isValid = 0;
        char options[10];
        do {
            if (isValid) {
                break;
            }
            isValid = 1;
    
            scanf("%9s", options);
    
            if (options[0] != '-' || strlen(options) < 2) {
                printf("Invalid option. Please enter a valid option.\n");
                isValid = 0;
            } else {
                for (int i = 1; i < strlen(options); i++) {
                    if (!strchr("ndac", options[i])) {
                        printf("Invalid option: %c. Please enter a valid option.\n", options[i]);
                        isValid = 0;
                        break;
                    }
                }
            }
    
            if (!isValid) {
                printDirectoryOptions();
            }
        } while (!isValid);

        printf("Selected options: %s\n", options);

        DIR *dir = opendir(path);

        for(int i = 1; i < strlen(options); i++) {
            switch(options[i]) {
                case 'a': 
                    printf("Access rights:");
                    printAccessRights(stats.st_mode);
                    break;
                case 'n':
                    printf("Directory name: %s\n", extractName(path));
                    break;
                case 'd':
                    printf("Directory size: %lld bytes\n", stats.st_size);
                    break;
                case 'c':
                    if (dir == NULL) {
                        printf("Error opening directory: %s\n", strerror(errno));
                        break;
                    }
                    int count = 0;
                    struct dirent *entry;
                    while ((entry = readdir(dir)) != NULL) {
                        if (entry->d_type == DT_REG) {
                            char *ext = strrchr(entry->d_name, '.');
                            if (ext != NULL && strcmp(ext, ".c") == 0) {
                                count++;
                            }
                        }
                    }
                    closedir(dir);
                    printf("Number of files with .c extension: %d\n", count);
                    break;
                default:
                    break;

            }
        }
    } else if (S_ISLNK(stats.st_mode)) {
        printf("File type: link\n");
    } else if (S_ISREG(stats.st_mode)) {
        printf("File type: regular file\n");
    } else {
        printf("Unknown file type\n");
    }
}

void createTxtFile(char* dirname) {
    char filename[strlen(dirname) + 11];
    sprintf(filename, "%s_file.txt", dirname);
    char* args[] = {"touch", filename, NULL};
    execvp(args[0], args);
    perror("Failed to execute touch command!");
    exit(EXIT_FAILURE);
}

void compileScript(char* filename, int writeEnd, int readEnd) {
    close(readEnd); // close read end of pipe
    dup2(writeEnd, 1); // redirect stdout to pipe
    execlp("bash", "bash", "compile.sh", filename, NULL);
    perror("execlp error!");
    close(writeEnd);
    exit(EXIT_FAILURE);
}

void printNoOfLines(char* filename, int readEnd, int writeEnd) {
    close(readEnd); // close read descriptor; child will write into pipe
    dup2(writeEnd, 1); // redirect standard output to pipe write end
    execlp("wc", "wc", "-l", NULL); // execute wc command
    perror("execlp error!\n"); // print error message if execution fails
    close(writeEnd);
    exit(EXIT_FAILURE);
}

void changeRights(char* linkname) {
    char* args[] = {"chmod", "0760", linkname, NULL};
    execvp(args[0], args);
    perror("execvp failed");
    exit(EXIT_FAILURE);
}


int main(int argc, char **argv) {
    pid_t pidFile, pidFileOpt, pidLnk, pidLnkOpt, pidDir, pidDirOpt;
    //int status;
    int pfd[2];

    if(pipe(pfd) < 0) {
        perror("Pipe creation error!\n");
        exit(1);
    }

    if(argc < 2) {
        printf("Not enough arguments!\n");
    } else {
        for(int i = 1; i < argc; i++) {
            char *path = argv[i];
            struct stat stats;

            if (lstat(path, &stats) == -1) {
                printf("Error accessing file: %s\n", strerror(errno));
                continue;
            }

            if(S_ISREG(stats.st_mode)) {
                printf("\n%s - REGULAR FILE\n\n", argv[i]);

                pidFile = fork();
                if(pidFile < 0) {
                    perror("Error on fork!");
                    exit(1);
                } else if(pidFile == 0) { // child1
                    if(strstr(path, ".c") != NULL) {
                        compileScript(path, pfd[1], pfd[0]);
                        exit(EXIT_FAILURE);
                    } else {
                        printNoOfLines(path, pfd[1], pfd[0]);
                        exit(EXIT_FAILURE);
                    }
                }

                pidFileOpt = fork();
                if(pidFileOpt < 0) {
                    perror("Error on fork!");
                    exit(1);
                } else if(pidFileOpt == 0) { //child2
                    regularFileOptions(path, stats);
                    exit(EXIT_FAILURE);
                }

                close(pfd[1]); // close write end of pipe
                int status;
                waitpid(pidFile, &status, 0);
                waitpid(pidFileOpt, &status, 0);

                if (WIFEXITED(status)) {
                    int exit_code = WEXITSTATUS(status);
                    int score = 0, errors = 0, warnings = 0;
                    char buffer[1024];
                    // read in a while
                    int n = read(pfd[0], buffer, 1024); // read output from pipe

                    if (n > 0) {
                        buffer[n] = '\0';
                        char *ptr = buffer;
                        while (*ptr != '\0') {
                            if (strstr(ptr, "Errors:") == ptr) { // change to match the format of the output
                                errors = atoi(ptr+7); // skip "Errors: "
                            } else if (strstr(ptr, "Warnings:") == ptr) { // change to match the format of the output
                                warnings = atoi(ptr+9); // skip "Warnings: "
                            }
                            ptr = strchr(ptr, '\n');
                            if (ptr == NULL) break;
                            ptr++;
                        }
                        if (errors == 0 && warnings == 0) {
                            score = 10;
                        } else if (errors > 0) {
                            score = 1;
                        } else if (warnings > 10) {
                            score = 2;
                        } else {
                            score = 2 + 8 * (10 - warnings) / 10;
                        }
                    }
                    char grade[100000 + 10];
                    sprintf(grade, "%s: %d\n", argv[i], score);
                    int fd = open("grades.txt", O_WRONLY | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR);
                    if (fd == -1) {
                        perror("open failed");
                        exit(EXIT_FAILURE);
                    }
                    if (write(fd, grade, strlen(grade)) == -1) {
                        perror("write failed");
                        exit(EXIT_FAILURE);
                    }
                    close(fd);
                    printf("Compile process for %s exited with status %d\n", argv[i], exit_code);
                } else {
                    printf("Compile process for %s terminated abnormally\n", argv[i]);
                }

            } else if(S_ISLNK(stats.st_mode)) {
                printf("\n%s - SYMBOLIC LINK.\n", argv[i]);

                pidLnk = fork();
                if(pidLnk < 0) {
                    perror("Error on fork!");
                    exit(1);
                } else if(pidLnk == 0) { // child1
                    changeRights(path);
                    exit(EXIT_FAILURE);
                }

                pidLnkOpt = fork();
                if(pidLnkOpt < 0) {
                    perror("Error on fork!");
                    exit(1);
                } else if(pidLnkOpt == 0) { //child2
                    symbolicLinkOptions(path, stats);
                    exit(EXIT_FAILURE);
                }

                int status;
                waitpid(pidLnk, &status, 0);
                waitpid(pidLnkOpt, &status, 0);

                if (WIFEXITED(status)) {
                    int exit_code = WEXITSTATUS(status);
                    printf("Compile process for %s exited with status %d\n", argv[i], exit_code);
                } else {
                    printf("Compile process for %s terminated abnormally\n", argv[i]);
                }

            } else if(S_ISDIR(stats.st_mode)) {
                printf("\n%s - DIRECTORY\n\n", argv[i]);

                pidDir = fork();
                if(pidDir < 0) {
                    perror("Error on fork!");
                    exit(1);
                } else if(pidDir == 0) { //child1
                    createTxtFile(path);
                    exit(EXIT_FAILURE);
                }
                pidDirOpt = fork();
                if(pidDirOpt < 0) {
                    perror("Error on fork!");
                    exit(1);
                } else if(pidDirOpt == 0) { //child2
                    directoryOptions(path, stats);
                    exit(EXIT_FAILURE);
                }
    
                int status;
                waitpid(pidDir, &status, 0);
                waitpid(pidDirOpt, &status, 0);

                if (WIFEXITED(status)) {
                    int exit_code = WEXITSTATUS(status);
                    printf("Compile process for %s exited with status %d\n", argv[i], exit_code);
                } else {
                    printf("Compile process for %s terminated abnormally\n", argv[i]);
                }

            } else {
                printf("Unknown type of file!\n");
            }   

        }
    }

    //close(pfd[0]); // close read end of pipe
    return 0;
}
