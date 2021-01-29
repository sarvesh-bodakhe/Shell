#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include<string.h>

typedef struct cmd_struct{
    char* command;
    
    int is_in_redirection ;
    int is_out_redirection ;
    FILE *file_ptr ;
    int arg_count;
    char** arglist;
    char* file_name;
    
}cmd_struct;

void print_cmd_struct(cmd_struct *ptr){
    printf("\nCommand Strcture ->\n");
    printf("1.Command:%s\n", ptr->command);
    printf("2.argoument count:%d\n", ptr->arg_count);
    printf("3.Arguments:");
    for(int i=0; i<ptr->arg_count; i++){
        printf("%s ",  ptr->arglist[i]);
    }
    printf("\n");
    printf("4.input redirection?:%d\n", ptr->is_in_redirection);
    printf("5.output redirection?:%d\n", ptr->is_out_redirection);
    printf("6.file_name:%s\n\n", ptr->file_name);

}

cmd_struct* make_cmd_struct(cmd_struct *ptr, char *cmd){
    // printf("In make_cmd_struct.cmd=%s\n", cmd);
    int i=0, j=0;
    int flag = 1;
    int arg_count = 0;
    ptr = (cmd_struct*) malloc(sizeof(cmd_struct));
    ptr->arglist = (char**) malloc(sizeof(char**) * 10);
    while(cmd[j] != '\0'){
        // printf("%d:%c", j, cmd[j]);
        if(cmd[j] == ' '){
            if(flag == 0){
                // printf("Word from size=%d. %d to %d:",j-i+1, i, j);              
                int size = j-i;
                char* str = (char*) malloc(sizeof(char) * (size+1));
                int x=0;
                for(int k=i; k<j; k++){
                    str[x++] = cmd[k];
                }   
                str[x] = '\0';
                ptr->arglist[arg_count] = (char*) malloc(sizeof(str));
                ptr->arglist[arg_count] = str;
                arg_count++;
                flag=1;
            }    
            flag = 1; 
        }
        else{
            if(flag == 1){
                flag = 0;
                i = j;
            }
        }
        j++;
    }
    if(flag == 0){
        char* str = (char*) malloc(sizeof(char) * (j-i+1));
        int x=0;
        for(int k=i; k<j; k++){
            str[x++] = cmd[k];
        }
        str[x] = '\0';
        ptr->arglist[arg_count] = (char*) malloc(sizeof(str));
        ptr->arglist[arg_count] = str; 
        arg_count++;
    }
    printf("Command:%s\n", ptr->arglist[0]);
    ptr->command = ptr->arglist[0];
    ptr->arg_count = arg_count;

    print_cmd_struct(ptr);
    return ptr;
}

int find_no_commands(char *line){
    int total_commands = 0;
    char *token1 = strtok(line, "|");
    while(token1 != NULL){
        total_commands++;
        token1 = strtok(NULL, "|");
    }
    printf("Total Commands:%d\n", total_commands);
    return total_commands;
}

cmd_struct*  parse(char **op_list,  char **line, int *total_commands){
    printf("In parse() function. Line:%s\n", *line);
    char pipeline_delim = '|';
    char input_redirection_delim = '<';
    char output_redirection_delin = '>';
    

    cmd_struct ** cmd_list = (cmd_struct**)malloc(sizeof(cmd_struct**)*10);
    int cmd_count = 0;
    char *token = strtok(*line, "|");
    while(token != NULL){
        printf("Command No.%d => %s\n", cmd_count+1, token);
        cmd_struct* ptr = make_cmd_struct(cmd_list[cmd_count], token);
        cmd_list[cmd_count] = (cmd_struct*) malloc(sizeof(cmd_struct*));
        cmd_list[cmd_count] = ptr;
        token = strtok(NULL, "|");     
        cmd_count++;
    }
    printf("Total commands:%d\n", cmd_count);
    *total_commands = cmd_count;
    
    cmd_struct *list = *cmd_list;
    return list;
}

int main(int argc, char* argv[]){
    char* ENV = "/home/luffy/anaconda3/bin:/home/luffy/.local/bin:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/usr/games:/usr/local/games:/snap/bin";
    
    char *line = NULL;
    size_t len = 0;
    ssize_t linesize = 0;
    char* promt = "$";

    
    printf("%s", promt);
    cmd_struct* cmd_list = NULL;
    char* op_list = "";


    linesize = getline(&line, &len, stdin);
    line[linesize-1] = '\0';

    

    
    int total_commands = 0;
    cmd_list = parse( &op_list, &line, &total_commands);
    printf("Total Commands in main():%d\n", total_commands);


    // for(int i=0; i<total_commands; i++){
    //     print_cmd_struct(&cmd_list[i]);
    // }
    


    // char* binaryPath = "ls";
    // char* arg1 = "-l";
    // char* arg2 = "/home";
    // char* arglist[] = {binaryPath ,arg1, arg2, NULL};
    
    // execvp(binaryPath, arglist);
    // memset(arglist, 0, sizeof(arglist));
    
    
    return 0;
}