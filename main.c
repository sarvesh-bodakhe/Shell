#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include<string.h>

int main(){

    int a = 10;
    int *b = (int*)malloc(sizeof(int));
    *b = 10;
    char *cmd  = "ls -l";
    char* arglist[] = {"ls", "-l"};
    printf("%s\n", cmd);
    int pd = fork();
    
    if(pd < 0){
        perror("fork() falied\n");
        exit(0);
    }
    if(pd == 0){
        printf("In child process. Calling execvp()\n");
        // execvp("ls",arglist);
        char* binaryPath = "ls";
        char* arg1 = "-l";
        char* arg2 = "/home";
        char* arglist[] = {binaryPath ,arg1, arg2, NULL};    
        execvp(binaryPath, arglist);
        exit(0);
    }
    if(pd > 0){
        printf("In parent process\n");
        wait(NULL);
    }

    return 0;
}