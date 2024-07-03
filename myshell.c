#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/stat.h>

// Function prototypes
void parse_command(char* command, char* cmd, char* args[]);
void execute_command(char* cmd, char* args[]);
void list_environment_variables();
void print_help();
void io_redirect(char* args[]);

// // 0 false  1 true
// int is_cd();
// int is_ls();
// int is_environ();
// int is_help();
// int is_echo();
// int is_quit();
// int is_mkdir();
// int is_rmdir();
// int is_rm();
// int is_cp();
// int is_rm();
// int is_cat();
// int is_clear();



int main() {
    char input[1024];
    char cmd[256];
    char* args[256];
    char cwd[1024];

    while (1) {
        // Get current working directory
        if (getcwd(cwd, sizeof(cwd)) != NULL) {
            // Print shell prompt with current directory
            printf("%s> ", cwd);
        } else {
            perror("getcwd");
            exit(1);
        }
        fflush(stdout);

        // Read input command
        if (!fgets(input, sizeof(input), stdin)) {
            break;
        }

        // Parse command and arguments
        parse_command(input, cmd, args);

        // Execute the command
        execute_command(cmd, args);
    }

    return 0;
}

// Parse command into cmd and args array
void parse_command(char* command, char* cmd, char* args[]) {
    char* token;
    int i = 0;

    // Get the command (first token)
    token = strtok(command, " \n");
    if (token != NULL) {
        strcpy(cmd, token);
        args[i++] = token;

        // Get the arguments
        while ((token = strtok(NULL, " \n")) != NULL) {
            args[i++] = token;
        }
    }

    // Null-terminate the argument list
    args[i] = NULL;
}

// Execute the given command with arguments
void execute_command(char* cmd, char* args[]) {
    if (strcmp(cmd, "cd") == 0) {
        if (args[1] != NULL) {
            if (chdir(args[1]) != 0) {
                perror("cd");
            }
        } else {
            printf("Usage: cd [directory]\n");
        }
    } else if (strcmp(cmd, "environ") == 0) {
        list_environment_variables();
    } else if (strcmp(cmd, "ls") == 0) {
        char* dir = ".";
        if (args[1] != NULL) {
            dir = args[1];
        }
        DIR* dp = opendir(dir);
        if (dp != NULL) {
            struct dirent* entry;
            while ((entry = readdir(dp))) {
                printf("%s\n", entry->d_name);
            }
            closedir(dp);
        } else {
            perror("opendir");
        }
    } else if (strcmp(cmd, "help") == 0) {
        print_help();
    } else if (strcmp(cmd, "echo") == 0) {
        int j = 1;
        while (args[j] != NULL) {
            printf("%s ", args[j++]);
        }
        printf("\n");
    } else if (strcmp(cmd, "quit") == 0) {
        exit(0);
    } else if (strcmp(cmd, "mkdir") == 0) {
        if (args[1] != NULL) {
            if (mkdir(args[1], 0777) != 0) {
                perror("mkdir");
            }
        } else {
            printf("Usage: mkdir [directory]\n");
        }
    } else if (strcmp(cmd, "rmdir") == 0) {
        if (args[1] != NULL) {
            if (rmdir(args[1]) != 0) {
                perror("rmdir");
            }
        } else {
            printf("Usage: rmdir [directory]\n");
        }
    } else if (strcmp(cmd, "rm") == 0) {
        if (args[1] != NULL) {
            if (unlink(args[1]) != 0) {
                perror("rm");
            }
        } else {
            printf("Usage: rm [file]\n");
        }
    } else if (strcmp(cmd, "cp") == 0) {
        if (args[1] != NULL && args[2] != NULL) {
            int src_fd = open(args[1], O_RDONLY);
            if (src_fd == -1) {
                perror("open");
                return;
            }
            int dest_fd = open(args[2], O_WRONLY | O_CREAT | O_TRUNC, 0664);
            if (dest_fd == -1) {
                perror("open");
                close(src_fd);
                return;
            }
            char buffer[1024];
            ssize_t bytes;
            while ((bytes = read(src_fd, buffer, sizeof(buffer))) > 0) {
                if (write(dest_fd, buffer, bytes) != bytes) {
                    perror("write");
                    break;
                }
            }
            close(src_fd);
            close(dest_fd);
        } else {
            printf("Usage: cp [source] [destination]\n");
        }
    } else if (strcmp(cmd, "cat") == 0) {
        if (args[1] != NULL) {
            int fd = open(args[1], O_RDONLY);
            if (fd == -1) {
                perror("open");
                return;
            }
            char buffer[1024];
            ssize_t bytes;
            while ((bytes = read(fd, buffer, sizeof(buffer))) > 0) {
                if (write(STDOUT_FILENO, buffer, bytes) != bytes) {
                    perror("write");
                    break;
                }
            }
            close(fd);
        } else {
            printf("Usage: cat [file]\n");
        }
    } else {
        printf("Command not found: %s\n", cmd);
    }

    // Handle I/O redirection if '>' or '<' are present in arguments
    io_redirect(args);
}

// List all environment variables
void list_environment_variables() {
    extern char** environ;
    char** env = environ;
    while (*env != NULL) {
        printf("%s\n", *env);
        env++;
    }
}

// Print help information
void print_help() {
    printf("Supported commands:\n");
    printf("cd [directory]    Change current directory\n");
    printf("environ           List all environment variables\n");
    printf("ls [directory]    List files in directory\n");
    printf("help              Display this help message\n");
    printf("echo [args...]    Print arguments to screen\n");
    printf("quit              Exit the shell\n");
    printf("mkdir [directory] Create a new directory\n");
    printf("rmdir [directory] Remove a directory\n");
    printf("rm [file]         Remove a file\n");
    printf("cp [source] [destination] Copy a file\n");
    printf("cat [file]        Display file content\n");
}

// Handle I/O redirection
void io_redirect(char* args[]) {
    int i = 0;
    while (args[i] != NULL) {
        if (strcmp(args[i], ">") == 0) {
            if (args[i + 1] != NULL) {
                int fd = open(args[i + 1], O_WRONLY | O_CREAT | O_TRUNC, 0664);
                if (fd == -1) {
                    perror("open");
                    return;
                }
                if (dup2(fd, STDOUT_FILENO) == -1) {
                    perror("dup2");
                    return;
                }
                close(fd);
                args[i] = NULL; // Remove '>' from arguments
                break;
            } else {
                printf("Missing filename after '>'\n");
                return;
            }
        } else if (strcmp(args[i], "<") == 0) {
            if (args[i + 1] != NULL) {
                int fd = open(args[i + 1], O_RDONLY);
                if (fd == -1) {
                    perror("open");
                    return;
                }
                if (dup2(fd, STDIN_FILENO) == -1) {
                    perror("dup2");
                    return;
                }
                close(fd);
                args[i] = NULL; // Remove '<' from arguments
                break;
            } else {
                printf("Missing filename after '<'\n");
                return;
            }
        }
        i++;
    }
}
