#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include<string.h>
#include<fcntl.h> 
#include<stdlib.h>

#define PATH_MAX 30
enum redirection{NO, IN, OUT};

struct cmd_struct{
	char* command; 
	enum redirection *redirections;
	char **file_names;	
    int is_in_redirection ;
    int is_out_redirection ;
    FILE *file_ptr ;
    int arg_count;
    char** arglist;
    char* file_name;    
}cmd_struct;

struct cmd_line_struct{
    struct cmd_struct* cmd_list;
    int no_of_commands;
    
}cmd_line_struct;

void print_cmd_struct(struct cmd_struct *ptr){
    printf("Printing Command Strcture():\n");
    printf("\t1.Command:\t%s\n", ptr->command);
    printf("\t2.arg_count:\t%d\n", ptr->arg_count);
    printf("\t3.Arguments:\t");
    for(int i=0; i<ptr->arg_count; i++){
        printf("%s ",  ptr->arglist[i]);
    }
    printf("\n");
    if(ptr->is_in_redirection == 1)
        printf("\t4.Input Redirection\n");
    if(ptr->is_out_redirection == 1)
        printf("\t4.Output Redirection\n"); 
    if(ptr->file_name)
        printf("\t5.file_name:%s\n", ptr->file_name);

    // printf("4.input redirection?:%d\n", ptr->is_in_redirection);
    // printf("5.output redirection?:%d\n", ptr->is_out_redirection);
    // printf("6.file_name:%s\n\n", ptr->file_name);

}

/*
    TODO 
    Multiple Redirecions
*/


struct cmd_struct* make_cmd_struct(char *cmd){
    printf("\tIn make_cmd_struct():\n");
    // printf("In make_cmd_struct.cmd=%s\n", cmd);
    int i=0, j=0;
    int flag = 1;
    int arg_count = 0;
    int redirection_flag = 0;
    struct cmd_struct *ptr = (struct cmd_struct*) malloc(sizeof(cmd_struct));
    ptr->arglist = (char**) malloc(sizeof(char**) * 10);
    while(cmd[j] != '\0'){
        // printf("\t%d:%c\n", j, cmd[j]);
        if(cmd[j] == ' '){
            if(flag == 0){
                // printf("\tWord from size=%d. %d to %d:",j-i+1, i, j);              
                int size = j-i;
                char* str = (char*) malloc(sizeof(char) * (size+1));
                int x=0;
                for(int k=i; k<j; k++){
                    str[x++] = cmd[k];
                }   
                str[x] = '\0';
                // printf("%s\n", str);
                /* Redirection Operator has encountered */
                if(redirection_flag == 1){
                    printf("\tFile after redirection:%s\n", str);
                    ptr->file_name = (char*)malloc(sizeof(str));
                    ptr->file_name = str;
                    flag=1;
                    continue;
                }
                if(strcmp(str, "<")==0){
                    printf("\tInput Redirection Detected:%s\n", str);
                    ptr->is_in_redirection = 1;
                    redirection_flag = 1;
                    flag=1;
                    continue;
                }
                if(strcmp(str, ">")==0){
                    printf("\tOutput Redirection Detected:%s\n", str);
                    ptr->is_out_redirection = 1;
                    flag=1;
                    redirection_flag = 1;
                    continue;
                }
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
        if(redirection_flag == 1){
                    printf("\tFile after redirectoin:%s\n", str);
                    ptr->file_name = (char*)malloc(sizeof(str));
                    ptr->file_name = str;
                    flag=1;
        }
        else{
            ptr->arglist[arg_count] = (char*) malloc(sizeof(str));
            ptr->arglist[arg_count] = str; 
            arg_count++;
        }
    }
    // printf("Command:%s\n", ptr->arglist[0]);
    ptr->command = ptr->arglist[0];
    ptr->arg_count = arg_count;

    // print_cmd_struct(ptr);
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

struct cmd_struct**  parse(char **op_list,  char **line, int *total_commands){
    printf("\nIn parse():\n");
    // printf("In parse() function. Line:%s\n", *line);
    char pipeline_delim = '|';
    char input_redirection_delim = '<';
    char output_redirection_delin = '>';
    
    // Maxmimum Of 10 Commnads in a single line
    struct cmd_struct** cmd_list = (struct cmd_struct**)malloc(sizeof(struct cmd_struct**)*10);
    int cmd_count = 0;
    char *token = strtok(*line, "|");
    printf("\tTravesring All Commands\n");
    while(token != NULL){
        printf("\tCommand No.%d => %s\n", cmd_count+1, token);
        // Make a command struct from string pointed by ptr
        struct cmd_struct *ptr = (struct cmd_struct*)malloc(sizeof(struct cmd_struct));
        ptr = make_cmd_struct(token);   
        
        cmd_list[cmd_count] = (struct cmd_struct*) malloc(sizeof(struct cmd_struct*));
        cmd_list[cmd_count] = ptr;
        
        token = strtok(NULL, "|");     
        cmd_count++;
    }
    printf("Total commands In Parse():%d\n", cmd_count);
    *total_commands = cmd_count;
    
    /*
        printf("*** Printing cmd_list in parse\n");
        for(int i=0; i<cmd_count; i++){
            printf("%d=>\n", i);
            print_cmd_struct(cmd_list[i]);
        }
        printf("***\n");
    */  
    
    return cmd_list;
}



void execute_commands(struct cmd_struct** cmd_list, int total_commands){
    printf("\nIn execute_commands():\n");
    printf("\tTotal Commands:%d\n", total_commands);
    // printf("\tOutput:\n\n");
    int pipe_flag=0;
    int flag_write_into_pipe=0, flag_read_from_pipe=0;
    int pfd[2];
    /*Pipe Needed*/
    if(total_commands > 1){
        pipe(pfd);
    }
    for(int i=0; i<total_commands; i++){
        
        struct cmd_struct *ptr =   cmd_list[i];
        printf("\t%d.Command:%s\n",i+1, ptr->command);
        // print_cmd_struct(&cmd_list[i]);

        /*Need For Pipe*/
        if(i < total_commands-1){
            printf("\t** Need For Pipe. Write into pipe\n");
            pipe_flag = 1;
            flag_write_into_pipe = 1;
            flag_read_from_pipe = 1;
        }
        /*Creating Child process to run command*/
        int cpid = fork();
        if(cpid < 0){
            perror("\tfork() failed\n");
            exit(0);
        }
        if(cpid == 0){
            printf("\tIn child process1.\n");
            // print_cmd_struct(&cmd_list[i]);
            
            // execvp("ls",arglist);    
            if(ptr->is_out_redirection){
                /*Closing Screen Output*/
                printf("\toutput redirection.calling close(1)\n");
                // printf("\tptr->file_name:%s\n", ptr->file_name);
                close(1);
                int fd_temp = open(ptr->file_name, O_RDWR);
                printf("\tclose(1) finished\n");
                printf("\tNew File Descriptor:%d\n", fd_temp);
                
            }            
            if(ptr->is_in_redirection){
                printf("\tinput redirection.calling close(0)\n");
                close(0);
                int fd = open(ptr->file_name, O_RDONLY);
                printf("\tnew file descriptor fd=%d\n", fd);
            }
            if(flag_write_into_pipe){
                /*Supposed to close(1)*/
                printf("\tWriting into pipe[1]. and executing command:%d\n", i);
                close(1);
                dup(pfd[1]);
                close(pfd[0]);
                flag_read_from_pipe = 1;
                flag_write_into_pipe = 0;
                // printf("Mapped pfd[1] ie. pipe's write end to screen(1)\n");
            }
            // printf("\tExecuting Command %d\n", i);
            execvp(ptr->command,ptr->arglist);
            // printf("\tCommand Executed Successfully\n");
            // exit(0);
        }
        if(cpid > 0){
            // wait(NULL);
            printf("\tWait()1 call returned from child id:%d.In parent process1\n", cpid);
            if(flag_read_from_pipe){
                i++;
                ptr =   cmd_list[i];
                printf("\tCreating process to read from pipe.\n");
                int cpid2 = fork();
                
                if(cpid2 < 0){
                    perror("\tfork() failed\n");
                    exit(0);
                }
                if(cpid2 == 0){
                    printf("\tIn child2 process. which is to read from pipe\n");
                    close(0);
                    char str3 [1000];
                    dup(pfd[0]);
                    close(pfd[1]);
                    printf("\tprinting pipe[0]:\n");
                    // while(scanf("%s", str3)){
                    //     printf("%s", str3);
                    // }
                    // while(read(0, str3,1000) != -1){
                    //     printf("%s", str3);
                    // }
                    printf("\n***\n ");
                    lseek(0, SEEK_SET,0);
                    // gets(str3);
                    // printf("str3:%s\n",str3 );
                    printf("\tExecuting Command %d. %s\n", i, ptr->command);
                    execvp(ptr->command,ptr->arglist);
                    exit(0);
                    printf("\tCommand %d exectued\n", i);
                }
                if(cpid2 > 0){   
                    // wait(NULL);
                    printf("\twait()2 call returned from child id:%d\n", cpid2);
                    
                }
            }
            wait(NULL);  
            // printf("\twait()2 call returned.In parent process2\n");
            // printf("Last line of parent of child 1\n");
            
        }
    }
    printf("Out of for loop. All commands executed\n");
}

void highlight () {
  printf("\033[0;45m");
  
}
void reset () {
  printf("\033[0m");
}


int main(int argc, char* argv[]){
    char* PATH = "/home/luffy/anaconda3/bin:/home/luffy/.local/bin:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/usr/games:/usr/local/games:/snap/bin";
    char cwd[100];
    
    // getcwd(cwd, sizeof(cwd));
    if(getcwd(cwd, 100) == NULL){
        perror("getcwd() error\n");
        return 1;
    }
    char* promt = strcat(cwd, "$");
    // printf("cwd:%s\n", cwd);

    while(1){
        char *line = NULL;
        size_t len = 0;
        ssize_t linesize = 0;

        highlight();
        printf("%s", promt);
        reset();
        struct cmd_struct** cmd_list = NULL;
        char* op_list = "";


        linesize = getline(&line, &len, stdin);
        line[linesize-1] = '\0';

        
        int total_commands = 0;
        cmd_list = parse(&op_list, &line, &total_commands);

        
            // printf("\n\nTotal Commands in main():%d\n\n\n", total_commands);
            // printf("*** Printing cmd_list in main()\n");
            // for(int i=0; i<total_commands; i++){
            //     printf("%d=>\n", i);
            //     print_cmd_struct(cmd_list[i]);
            // }
        
        
         execute_commands(cmd_list, total_commands);
    }
    

    
    


    // char* binaryPath = "ls";
    // char* arg1 = "-l";
    // char* arg2 = "/home";
    // char* arglist[] = {binaryPath ,arg1, arg2, NULL};
    
    // execvp(binaryPath, arglist);
    // memset(arglist, 0, sizeof(arglist));
    
    
    return 0;
}
