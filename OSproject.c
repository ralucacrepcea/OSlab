#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>

// Function to print access rights
void printAccessRights(mode_t mode) {
    printf("User:\n");
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
    printf("\t -n for name \n");
    printf("\t -d for size \n");
    printf("\t -h for hard link count \n");
    printf("\t -m for time of last modification \n");
    printf("\t -a for access rights \n");
    printf("\t -l for create symbolic link \n");
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

// Function to print file information
void regularFileOptions(char *file_path, struct stat file_stats) {
    char link_name[100];

    printf("\nFile name: %s\n", file_path);

    if (S_ISREG(file_stats.st_mode)) {
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
                    printf("File name: %s\n", file_path);
                    break;
                case 'd':
                    printf("File size: %lld bytes\n", file_stats.st_size);
                    break;
                case 'h':
                    printf("Hard link count: %hu\n", file_stats.st_nlink);
                    break;
                case 'm':
                    printf("Time of last modification: %s", ctime(&file_stats.st_mtime));
                    break;
                case 'a':
                    printAccessRights(file_stats.st_mode);
                    break;
                case 'l':
                    printf("Enter the name of the symbolic link: ");
                    scanf("%s", link_name);

                    if (symlink(file_path, link_name) == -1) {
                        printf("Error creating symbolic link: %s\n", strerror(errno));
                    } else {
                        printf("Symbolic link created successfully!\n");
                    }

                    break;
                default:
                    break;
            }
        }
    } else if (S_ISDIR(file_stats.st_mode)) {
        printf("File type: directory\n");
    } else if (S_ISLNK(file_stats.st_mode)) {
        printf("File type: symbolic link\n");
    } else {
        printf("Unknown file type\n");
    }
}

void symbolicLinkOptions(char *file_path, struct stat file_stats) {
    printf("\nSymbolic link name: %s\n", file_path);

    if (S_ISLNK(file_stats.st_mode)) {
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
        ssize_t len = readlink(file_path, target_path, sizeof(target_path) - 1);

        for (int i = 1; i < strlen(options) && !deletedLink; i++) {
            switch(options[i]) {
                case 'n':
                    printf("Symbolic link name: %s\n", file_path);
                    break;
                case 'd':
                    printf("Symbolic link size: %lld bytes\n", file_stats.st_size);
                    break;
                case 'l':
                    if (unlink(file_path) == -1) {
                        printf("Error deleting symbolic link: %s\n", strerror(errno));
                    } else {
                        printf("Symbolic link deleted successfully!\n");
                    }
                    deletedLink = 1;
                    break;
                case 'a':
                    printf("Access rights:");
                    printAccessRights(file_stats.st_mode);
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
    } else if (S_ISDIR(file_stats.st_mode)) {
        printf("File type: directory\n");
    } else if (S_ISREG(file_stats.st_mode)) {
        printf("File type: regular file\n");
    } else {
        printf("Unknown file type\n");
    }
}

int main(int argc, char *argv[]) {
    if(argc < 2) {
        printf("Not enough arguments!");
    } else {
        for (int i = 1; i < argc; i++) {
            char *file_path = argv[i];
            struct stat file_stats;

            if (lstat(file_path, &file_stats) == -1) {
                printf("Error accessing file: %s\n", strerror(errno));
                continue;
            }

            if(S_ISREG(file_stats.st_mode)) {
                regularFileOptions(file_path, file_stats);
            } else if(S_ISLNK(file_stats.st_mode)) {
                symbolicLinkOptions(file_path, file_stats);
            } else {
                printf("Unknown type of file!\n");
            }
        }
    }

    return 0;
}