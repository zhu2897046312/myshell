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
int get_command_type(const char* cmd);
void list_environment_variables();
void print_help();
void io_redirect(char* args[]);

// CMD-TYPE
const int CMD_CD = 1;
const int CMD_LS = 2;
const int CMD_ENVIRON = 3;
const int CMD_HELP = 4;
const int CMD_ECHO = 5;
const int CMD_QUIT = 6;
const int CMD_MKDIR = 7;
const int CMD_RMDIR = 8;
const int CMD_RM = 9;
const int CMD_CP = 10;
const int CMD_CAT = 11;
const int CMD_CLEAR = 12;
const int CMD_TOUCH = 13;

void execute_cmd_cd(char* args[]);
void execute_cmd_ls(char* args[]);
void execute_cmd_environ(char* args[]);
void execute_cmd_help(char* args[]);
void execute_cmd_echo(char* args[]);
void execute_cmd_quit(char* args[]);
void execute_cmd_mkdir(char* args[]);
void execute_cmd_rmdir(char* args[]);
void execute_cmd_rm(char* args[]);
void execute_cmd_cp(char* args[]);
void execute_cmd_cat(char* args[]);
void execute_cmd_clear(char* args[]);
void execute_cmd_touch(char* args[]);


int main() {
    char input[1024];
    char cmd[256];
    char* args[256];
    char cwd[1024];

    while (1) {
        // Get current working directory
        if (getcwd(cwd, sizeof(cwd)) != NULL) {
            // Print shell prompt with current directory
            printf("\033[34m%s\033[0m> ", cwd);
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

// Get command type
int get_command_type(const char* cmd) {
    if (strcmp(cmd, "cd") == 0) return CMD_CD;
    if (strcmp(cmd, "ls") == 0) return CMD_LS;
    if (strcmp(cmd, "environ") == 0) return CMD_ENVIRON;
    if (strcmp(cmd, "help") == 0) return CMD_HELP;
    if (strcmp(cmd, "echo") == 0) return CMD_ECHO;
    if (strcmp(cmd, "quit") == 0) return CMD_QUIT;
    if (strcmp(cmd, "mkdir") == 0) return CMD_MKDIR;
    if (strcmp(cmd, "rmdir") == 0) return CMD_RMDIR;
    if (strcmp(cmd, "rm") == 0) return CMD_RM;
    if (strcmp(cmd, "cp") == 0) return CMD_CP;
    if (strcmp(cmd, "cat") == 0) return CMD_CAT;
    if (strcmp(cmd, "clear") == 0) return CMD_CLEAR;
    if (strcmp(cmd, "touch") == 0) return CMD_TOUCH;
    return -1; // Unknown command
}

// Execute the given command with arguments
void execute_command(char* cmd, char* args[]) {
    int cmd_type = get_command_type(cmd);
    
    switch (cmd_type) {
        case CMD_CD:
            execute_cmd_cd(args);
            break;
        case CMD_LS:
            execute_cmd_ls(args);
            break;
        case CMD_ENVIRON:
            execute_cmd_environ(args);
            break;
        case CMD_HELP:
            execute_cmd_help(args);
            break;
        case CMD_ECHO:
            execute_cmd_echo(args);
            break;
        case CMD_QUIT:
            execute_cmd_quit(args);
            break;
        case CMD_MKDIR:
            execute_cmd_mkdir(args);
            break;
        case CMD_RMDIR:
            execute_cmd_rmdir(args);
            break;
        case CMD_RM:
            execute_cmd_rm(args);
            break;
        case CMD_CP:
            execute_cmd_cp(args);
            break;
        case CMD_CAT:
            execute_cmd_cat(args);
            break;
        case CMD_CLEAR:
            execute_cmd_clear(args);
            break;
        case CMD_TOUCH:
        execute_cmd_touch(args);
            break;
        default:
            printf("Command not found: %s\n", cmd);
            break;
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
    printf("clear             Clear the screen\n");
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
                int fd

 = open(args[i + 1], O_RDONLY);
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

void execute_cmd_cd(char* args[]){
    if (args[1] != NULL) {
        if (chdir(args[1]) != 0) {
            perror("cd");
        }
    } else {
        printf("Usage: cd [directory]\n");
    }
}
void execute_cmd_ls(char* args[]){
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
}
void execute_cmd_environ(char* args[]){
    list_environment_variables();
}
void execute_cmd_help(char* args[]){
    print_help();
}
void execute_cmd_echo(char* args[]){
    int j = 1;
    while (args[j] != NULL) {
        printf("%s ", args[j++]);
    }
    printf("\n");
}
void execute_cmd_quit(char* args[]){
    exit(0);
}
void execute_cmd_mkdir(char* args[]){
    if (args[1] != NULL) {
        if (mkdir(args[1], 0777) != 0) {
            perror("mkdir");
        }
    } else {
        printf("Usage: mkdir [directory]\n");
    }
}
void execute_cmd_rmdir(char* args[]){
    if (args[1] != NULL) {
        if (rmdir(args[1]) != 0) {
            perror("rmdir");
        }
    } else {
        printf("Usage: rmdir [directory]\n");
    }
}
void execute_cmd_rm(char* args[]){
    if (args[1] != NULL) {
        if (unlink(args[1]) != 0) {
            perror("rm");
        }
    } else {
        printf("Usage: rm [file]\n");
    }
}
void execute_cmd_cp(char* args[]){
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
}
void execute_cmd_cat(char* args[]){
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
}
void execute_cmd_clear(char* args[]){
    printf("\033[H\033[J");
}
void execute_cmd_touch(char* args[]){
    if (args[1] != NULL) {
        int fd = open(args[1], O_WRONLY | O_CREAT, 0664);
        if (fd == -1) {
            perror("touch");
            return;
        }
        close(fd);
    } else {
        printf("Usage: touch [file]\n");
    }
}