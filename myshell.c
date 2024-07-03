
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/wait.h>

// Function prototypes
void parse_command(char* command, char* cmd, char* args[]);
void execute_command(char* cmd, char* args[]);
int get_command_type(const char* cmd);
void list_environment_variables();
void print_help();
void io_redirect(char* args[]);
void execute_pipeline(char* pipeline);

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
void execute_cmd_grep(char* args[]);

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
const int CMD_GREP = 14;

int main() {
    char input[1024];
    char cwd[1024];

    while (1) {
        // Get current working directory    |   获取当前工作目录
        if (getcwd(cwd, sizeof(cwd)) != NULL) {
            // Print shell prompt with current directory    |   实现彩色输出-工作目录路径
            printf("\033[34m%s\033[0m> ", cwd);
        } else {
            perror("getcwd");
            exit(1);
        }
        fflush(stdout);//刷新标准输出流

        // Read input command   |   读取输入命令
        if (!fgets(input, sizeof(input), stdin)) {
            break;
        }

        // Check if the input contains a pipeline   |   检查是否有管道符
        if (strchr(input, '|') != NULL) {
            execute_pipeline(input);
        } else {
            char cmd[256];  //  命令
            char* args[256];    //  参数列表

            // Parse command and arguments  |   解析命令和参数      args[0] = cmd(命令)
            parse_command(input, cmd, args);

            // printf("Arguments:\n");
            // for (int i = 0; args[i] != NULL; i++) {
            //     printf("%d: %s\n", i, args[i]);
            // }

            // Execute the command  |   执行命令
            execute_command(cmd, args);
        }
    }

    return 0;
}

// Parse command into cmd and args array    |   解析命令行参数
void parse_command(char* command, char* cmd, char* args[]) {
    char* token;
    int i = 0;

    // Get the command (first token)
    token = strtok(command, " \n");  // 用空格和换行符作为分隔符
    if (token != NULL) {
        strcpy(cmd, token);  // 将第一个标记（命令）复制到 cmd
        args[i++] = token;  // 将第一个标记放入 args 数组

        // Get the arguments
        while ((token = strtok(NULL, " \n")) != NULL) {
            args[i++] = token;  // 将后续标记放入 args 数组
        }
    }

    // Null-terminate the argument list
    args[i] = NULL;  // 将最后一个参数置为 NULL
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
    if (strcmp(cmd, "grep") == 0) return CMD_GREP;
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
        case CMD_GREP:
            execute_cmd_grep(args);
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
    printf("touch [file]      Create an empty file\n");
    printf("grep [pattern]    Search for pattern in input\n");
}

// Handle I/O redirection
void io_redirect(char* args[]) {
    int i = 0;
    while (args[i] != NULL) {  // 遍历参数列表
        if (strcmp(args[i], ">") == 0) {  // 检查是否有输出重定向符号 '>'
            if (args[i + 1] != NULL) {  // 确认 '>' 后面有文件名
                int fd = open(args[i + 1], O_WRONLY | O_CREAT | O_TRUNC, 0664);  // 打开文件用于写入，若文件不存在则创建文件，若文件存在则截断文件
                if (fd == -1) {  // 检查文件是否成功打开
                    perror("open");  // 打印错误信息
                    return;  // 退出函数
                }
                if (dup2(fd, STDOUT_FILENO) == -1) {  // 将标准输出重定向到文件
                    perror("dup2");  // 打印错误信息
                    return;  // 退出函数
                }
                close(fd);  // 关闭文件描述符
                args[i] = NULL;  // 从参数列表中移除 '>'
                break;  // 退出循环
            } else {
                printf("Missing filename after '>'\n");  // 若缺少文件名，则打印错误信息
                return;  // 退出函数
            }
        } else if (strcmp(args[i], "<") == 0) {  // 检查是否有输入重定向符号 '<'
            if (args[i + 1] != NULL) {  // 确认 '<' 后面有文件名
                int fd = open(args[i + 1], O_RDONLY);  // 打开文件用于读取
                if (fd == -1) {  // 检查文件是否成功打开
                    perror("open");  // 打印错误信息
                    return;  // 退出函数
                }
                if (dup2(fd, STDIN_FILENO) == -1) {  // 将标准输入重定向到文件
                    perror("dup2");  // 打印错误信息
                    return;  // 退出函数
                }
                close(fd);  // 关闭文件描述符
                args[i] = NULL;  // 从参数列表中移除 '<'
                break;  // 退出循环
            } else {
                printf("Missing filename after '<'\n");  // 若缺少文件名，则打印错误信息
                return;  // 退出函数
            }
        }
        i++;  // 移动到下一个参数
    }
}


// Execute commands in a pipeline
void execute_pipeline(char* pipeline) {
    char* commands[256];  // 存储管道中的每个命令
    int i = 0;

    // Split the pipeline into individual commands | 使用 '|' 分割管道中的各个命令
    commands[i] = strtok(pipeline, "|");  // 获取第一个命令
    while (commands[i] != NULL) {  // 当命令不为空时
        commands[++i] = strtok(NULL, "|");  // 获取下一个命令
    }

    int num_commands = i;  // 计算命令的数量
    int pipefd[2], fd_in = 0;  // pipefd用于存储管道的文件描述符, fd_in保存上一个命令的输出

    for (i = 0; i < num_commands; i++) {  // 循环处理每个命令
        pipe(pipefd);  // 创建一个管道

        if (fork() == 0) {  // 创建一个子进程
            dup2(fd_in, 0); // 改变输入，根据上一个命令的输出设置输入
            if (i < num_commands - 1) {  // 如果不是最后一个命令
                dup2(pipefd[1], 1);  // 将当前命令的输出重定向到管道的写端
            }
            close(pipefd[0]);  // 关闭管道的读端

            char cmd[256];  // 存储当前命令
            char* args[256];  // 存储当前命令的参数

            // Parse the command and arguments | 解析命令和参数
            parse_command(commands[i], cmd, args);

            // Execute the command | 执行命令
            execute_command(cmd, args);
            exit(0);  // 终止子进程
        } else {
            wait(NULL); // Waiting for the child process to finish | 等待子进程完成
            close(pipefd[1]);  // 关闭管道的写端
            fd_in = pipefd[0]; // Save the input for the next command | 保存输入用于下一个命令
        }
    }
}

void execute_cmd_cd(char* args[]){
    if (args[1] != NULL) {  // 检查是否提供了目录参数
        if (chdir(args[1]) != 0) {  // 尝试改变当前工作目录
            perror("cd");  // 如果失败，输出错误信息
        }
    } else {
        printf("Usage: cd [directory]\n");  // 如果没有提供参数，打印用法提示
    }
}

void execute_cmd_ls(char* args[]){
    char* dir = ".";  // 默认目录是当前目录
    if (args[1] != NULL) {  // 如果提供了目录参数
        dir = args[1];  // 将dir设置为该目录
    }
    DIR* dp = opendir(dir);  // 打开目录
    if (dp != NULL) {  // 如果目录成功打开
        struct dirent* entry;  // 用于存储目录项
        while ((entry = readdir(dp))) {  // 读取目录中的每个条目
            printf("%s\n", entry->d_name);  // 打印条目的名字
        }
        closedir(dp);  // 关闭目录
    } else {
        perror("opendir");  // 如果无法打开目录，输出错误信息
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
    while (args[j] != NULL) {  // 逐个打印参数
        printf("%s ", args[j++]);  // 打印参数并加空格
    }
    printf("\n");  // 打印新行
}

void execute_cmd_quit(char* args[]){
    exit(0);  // 退出程序
}

void execute_cmd_mkdir(char* args[]){
    if (args[1] != NULL) {  // 检查是否提供了目录名
        if (mkdir(args[1], 0777) != 0) {  // 创建目录并设置权限
            perror("mkdir");  // 如果失败，输出错误信息
        }
    } else {
        printf("Usage: mkdir [directory]\n");  // 如果没有提供目录名，打印用法提示
    }
}

void execute_cmd_rmdir(char* args[]){
    if (args[1] != NULL) {  // 检查是否提供了目录名
        if (rmdir(args[1]) != 0) {  // 删除目录
            perror("rmdir");  // 如果失败，输出错误信息
        }
    } else {
        printf("Usage: rmdir [directory]\n");  // 如果没有提供目录名，打印用法提示
    }
}

void execute_cmd_rm(char* args[]){
    if (args[1] != NULL) {  // 检查是否提供了文件名
        if (unlink(args[1]) != 0) {  // 删除文件
            perror("rm");  // 如果失败，输出错误信息
        }
    } else {
        printf("Usage: rm [file]\n");  // 如果没有提供文件名，打印用法提示
    }
}

void execute_cmd_cp(char* args[]){
    if (args[1] != NULL && args[2] != NULL) {  // 检查是否提供了源文件和目标文件
        int src_fd = open(args[1], O_RDONLY);  // 打开源文件
        if (src_fd == -1) {  // 如果无法打开源文件
            perror("open");  // 输出错误信息
            return;
        }
        int dest_fd = open(args[2], O_WRONLY | O_CREAT | O_TRUNC, 0664);  // 打开或创建目标文件
        if (dest_fd == -1) {  // 如果无法打开目标文件
            perror("open");  // 输出错误信息
            close(src_fd);  // 关闭源文件
            return;
        }
        char buffer[1024];  // 缓冲区
        ssize_t bytes;
        while ((bytes = read(src_fd, buffer, sizeof(buffer))) > 0) {  // 读取源文件内容
            if (write(dest_fd, buffer, bytes) != bytes) {  // 写入目标文件
                perror("write");  // 如果写入失败，输出错误信息
                break;
            }
        }
        close(src_fd);  // 关闭源文件
        close(dest_fd);  // 关闭目标文件
    } else {
        printf("Usage: cp [source] [destination]\n");  // 如果没有提供足够的参数，打印用法提示
    }
}

void execute_cmd_cat(char* args[]){
    if (args[1] != NULL) {  // 检查是否提供了文件名
        int fd = open(args[1], O_RDONLY);  // 打开文件
        if (fd == -1) {  // 如果无法打开文件
            perror("open");  // 输出错误信息
            return;
        }
        char buffer[1024];  // 缓冲区
        ssize_t bytes;
        while ((bytes = read(fd, buffer, sizeof(buffer))) > 0) {  // 读取文件内容
            if (write(STDOUT_FILENO, buffer, bytes) != bytes) {  // 打印到标准输出
                perror("write");  // 如果写入失败，输出错误信息
                break;
            }
        }
        close(fd);  // 关闭文件
    } else {
        printf("Usage: cat [file]\n");  // 如果没有提供文件名，打印用法提示
    }
}

void execute_cmd_clear(char* args[]){
    printf("\033[H\033[J");  // 清屏命令
}

void execute_cmd_touch(char* args[]){
    if (args[1] != NULL) {  // 检查是否提供了文件名
        int fd = open(args[1], O_WRONLY | O_CREAT, 0664);  // 打开或创建文件
        if (fd == -1) {  // 如果无法打开文件
            perror("touch");  // 输出错误信息
            return;
        }
        close(fd);  // 关闭文件
    } else {
        printf("Usage: touch [file]\n");  // 如果没有提供文件名，打印用法提示
    }
}

void execute_cmd_grep(char* args[]){
    if (args[1] != NULL) {  // 检查是否提供了匹配模式
        char* pattern = args[1];  // 获取匹配模式
        char buffer[1024];  // 缓冲区

        while (fgets(buffer, sizeof(buffer), stdin) != NULL) {  // 从标准输入读取一行
            if (strstr(buffer, pattern) != NULL) {  // 检查行中是否包含匹配模式
                printf("%s", buffer);  // 如果包含，打印行
            }
        }
    } else {
        printf("Usage: grep [pattern]\n");  // 如果没有提供匹配模式，打印用法提示
    }
}
