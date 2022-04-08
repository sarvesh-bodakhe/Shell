#include<stdio.h>
#include "shell.h"
#include "signal.h"

int main(){
    
    char cwd[100];

    signal_init_shell(); // Enable shell signal handler

    if (getcwd(cwd, 100) == NULL)
    {
        perror("getcwd() error\n");
        return 1;
    }
    promt = strcat(cwd, "$");
    // printf("cwd:%s\n", cwd);

    while (1)
    {
        if (getcwd(cwd, 100) == NULL)
        {
            perror("getcwd() error\n");
            return 1;
        }
        promt = strcat(cwd, "$");
        char *line = NULL;
        size_t len = 0;
        ssize_t linesize = 0;

        highlight();
        printf("%s", promt);
        reset();
        printf(" ");

        struct cmd_struct **cmd_list = NULL;
        char *op_list = "";

        linesize = getline(&line, &len, stdin);
        line[linesize - 1] = '\0';

        int total_commands = 0;
        cmd_list = parse(&line, &total_commands);

        // printf("\n\nTotal Commands in main():%d\n\n\n", total_commands);
        // printf("*** Printing cmd_list in main()\n");
        // for(int i=0; i<total_commands; i++){
        //     printf("%d=>\n", i);
        //     print_cmd_struct(cmd_list[i]);
        // }

        execute_commands_list(cmd_list, total_commands);
    }

    return 0;
}
