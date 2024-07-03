# 嘉应学院-操作系统课程设计-myshell

``` 
一、基本要求
1.cd [directory] 将当前目录改为 [directory]
2.environ 列出所有环境变量
3.ls 列出指定目录下所有文件
4.help 显示所有支持的命令
5.echo 在屏幕上显示参数并换行
6.quit 退出 shell
7.支持I/O重定向
8.常见的目录和文件操作，比如新建目录或文件，目录或文件的删除、复制，显示文件内容等。
二、添加功能
1.支持管道符操作    
2.不限制命令参数个数  
3.实现彩色输出
4.支持其它一些常见的linux命令
5.自己认为比较重要或能体现自己水平的功能    
``` 

```
基本功能
命令集：
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
添加功能：
1.实现彩色输出
2.其他常见命令
3.支持管道符
```