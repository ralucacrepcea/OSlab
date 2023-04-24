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
                    //printf("Invalid option: %c\n", options[i]);
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

        regularFileOptions(file_path, file_stats);
        }
    }

    return 0;
}