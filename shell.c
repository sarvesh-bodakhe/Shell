#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include<string.h>
#include<fcntl.h> 
#include<stdlib.h>
#include<signal.h>
#include "list.h"

struct job* job_list = NULL;

#define PATH_MAX 30
enum redirection{NO, IN, OUT};

int runnning_proc;
char runnning_proc_name[40];
char* promt;
int stopped_job_counter;

void signal_init_shell(void);
void signal_init_child();
void int_handler(int);
void stop_handler(int);
void cont_handler(int);
void backgound();
void foreground();
void showJobs();


/*
    Sarvesh:    Structure to hold information about single command

*/
struct cmd_struct{
	char* command; 
    int is_in_redirection ;
    int is_out_redirection ;
    int arg_count;
    char** arglist;
    char* file_name;    
    // For miltiple redirections
    char **file_names;	
    char *redirections;
}cmd_struct;


/*
    Sarvesh:    Structure to hold information about signle command line
*/
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
}

/*  Sarvesh:    Make a command structure out of string 
                Check for redierctions/ Files associated
                Returns a pointer to the structre
*/
struct cmd_struct* make_cmd_struct(char *cmd){
    // printf("\tIn make_cmd_struct():\n");
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
                    // printf("\tFile after redirection:%s\n", str);
                    ptr->file_name = (char*)malloc(sizeof(str));
                    ptr->file_name = str;
                    flag=1;
                    continue;
                }
                if(strcmp(str, "<")==0){
                    // printf("\tInput Redirection Detected:%s\n", str);
                    ptr->is_in_redirection = 1;
                    redirection_flag = 1;
                    flag=1;
                    continue;
                }
                if(strcmp(str, ">")==0){
                    // printf("\tOutput Redirection Detected:%s\n", str);
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
                    // printf("\tFile after redirectoin:%s\n", str);
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
    // printf("Total Commands:%d\n", total_commands);
    return total_commands;
}

// a < b > c
// a must be a command which does a scanf()
// b must be a file

/*      Returns a pointer to list of raw unstructured commands      */
struct cmd_struct**  parse(char **line, int *total_commands){
    // printf("\nIn parse():\n");
    // printf("In parse() function. Line:%s\n", *line);
    char pipeline_delim = '|';
    char input_redirection_delim = '<';
    char output_redirection_delin = '>';
    
    /* Maxmimum Of 10 Commnads in a single line     */
    struct cmd_struct** cmd_list = (struct cmd_struct**)malloc(sizeof(struct cmd_struct**)*10);
    int cmd_count = 0;
    char *token = strtok(*line, "|");
    // printf("\tTravesring All Commands\n");
    while(token != NULL){
        // printf("\tCommand No.%d => %s\n", cmd_count+1, token);
        // Make a command struct from string pointed by ptr
        struct cmd_struct *ptr = (struct cmd_struct*)malloc(sizeof(struct cmd_struct));
        ptr = make_cmd_struct(token);   
        /*
        for multiple redirections
        
        */
        cmd_list[cmd_count] = (struct cmd_struct*) malloc(sizeof(struct cmd_struct*));
        cmd_list[cmd_count] = ptr;
        
        token = strtok(NULL, "|");     
        cmd_count++;
    }
    // printf("Total commands In Parse():%d\n", cmd_count);
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




/*   Check if particular command writes into the pipe       */
int check_for_write(int command_no, int no_of_pipes){
    if(no_of_pipes == 0)
        return 0;
    if(command_no==0)
        return 1;
    if(command_no < no_of_pipes)
        return 1;
    return 0;
}

/*   Check if particular command reads from  the pipe       */
int check_for_read(int command_no, int no_of_pipes){
    if(no_of_pipes == 0)
        return 0;
    if(command_no == 0)
        return 0;
    if(command_no < no_of_pipes+1)
        return 1;
    return 0;
}

/*   Utility function to execture signle function represented by command structure   */
void execute_single_command(struct cmd_struct *ptr){
    if(ptr->is_out_redirection){
        /*Closing Screen Output*/
        // printf("\toutput redirection.calling close(1)\n");
        close(1);
        int fd_temp = open(ptr->file_name, O_CREAT|O_RDWR,0666);
        // printf("\tclose(1) finished\n");
        // printf("\tNew File Descriptor:%d\n", fd_temp);      
    }            
    if(ptr->is_in_redirection){
        // printf("\tinput redirection.calling close(0)\n");
        close(0);
        int fd = open(ptr->file_name, O_RDONLY);
        // printf("\tnew file descriptor fd=%d\n", fd);
    }

    if(execvp(ptr->command,ptr->arglist) == -1){
        printf("%s : No such command\n", ptr->command);
    }
}



/*      Execute list of commands       */
void execute_commands_list(struct cmd_struct** cmd_list, int total_commands){
    int no_of_pipes = total_commands-1;
    // printf("\n\nIn execute_commands():Total Commands:%d\n\tNumber Of pipes required:%d\n\n", total_commands, no_of_pipes);
    int flag_write_into_pipe=0, flag_read_from_pipe=0, pipe_flag=0;
    int pfd[2];
    int status_child;
    // code for debugging purpose
    // for(int i=0; i<total_commands; i++){
    //     struct cmd_struct *ptr =   cmd_list[i];
    //     print_cmd_struct(ptr);
    // }printf("\n");
    
    // if(strcmp(cmd_list[0]->command, "jobs") == 0){
    //     showJobs();
    //     return;
    // }
    if(strcmp(cmd_list[0]->command, "jobs") == 0){ showJobs(); return; }

    if(strcmp(cmd_list[0]->command, "exit") == 0 || strcmp(cmd_list[0]->command, "quit") == 0){ exit(0); }

    if(strcmp(cmd_list[0]->command, "bg") == 0){
        if(cmd_list[0]->arg_count == 1){
            backgound(-1);
            return;
        }else if (cmd_list[0]->arg_count == 2){
            int job_no = cmd_list[0]->arglist[1][1] - '0';
            backgound(job_no);
            return;
        }else{
            return;
        }        
    }
    if(strcmp(cmd_list[0]->command, "fg") == 0){
        if(cmd_list[0]->arg_count == 1){
            foreground(-1);
            return;
        }else if (cmd_list[0]->arg_count == 2){
            int job_no = cmd_list[0]->arglist[1][1] - '0';
            foreground(job_no);
            return;
        }else{
            return;
        }        
    }

    /*Initialize Pipes*/
    int pipe_count = 0;
    int **pipe_arr = (int**)malloc(sizeof(int*) * no_of_pipes);
    int *process_arr = (int*)malloc(sizeof(int)*total_commands);

    for(int i=0; i<no_of_pipes; i++){
        pipe_arr[i] = (int*)malloc(sizeof(int)*2);
        pipe(pipe_arr[i]);
    }

    for(int i=0; i<total_commands; i++){
        process_arr[i] = fork();
        if(process_arr[i] == -1){
            perror("process_arr[i] = fork() failed\n");
            exit(1);
        }  
        if(process_arr[i] == 0){
            signal_init_child();
                        
            // printf("in child process: process_arr[%d]\n", i);
            if(check_for_read(i, no_of_pipes) == 1){
                // printf("\tprocess_arr[%d] should read  from pipe no[%d]\n", i, i-1);
                close(0);
                char str3[100];
                dup(pipe_arr[i-1][0]);
                close(pipe_arr[i-1][1]);
            }
            if(check_for_write(i, no_of_pipes) == 1){
                // printf("\tprocess_arr[%d] should write into pipe no[%d]\n", i, i);
                // printf("\tpipe[%d] created\n", i);
                close(1);
                dup(pipe_arr[i][1]);
                close(pipe_arr[i][0]);
            }
            
            struct cmd_struct *ptr =   cmd_list[i];
            execute_single_command(ptr);
            exit(0);
        }
        if(process_arr[i] > 0){
            runnning_proc = process_arr[i];
            strcpy(runnning_proc_name, cmd_list[i]->command);
            int child_id = process_arr[i];
            waitpid(runnning_proc, 0, WUNTRACED);
            runnning_proc = 0;
            if(check_for_read(i, no_of_pipes) == 1)
                close(pipe_arr[i-1][0]);
            if(check_for_write(i, no_of_pipes) == 1)
                close(pipe_arr[i][1]);
            // printf("\tin parent of process[%d]\n", i);
        }
    }
    // printf("Out Of the for loop\n");
}



void highlight () {
  printf("\033[0;45m");
  
}
void reset () {
  printf("\033[0m");
}

int main(int argc, char* argv[]){

    char cwd[100];

    signal_init_shell();                // Enable shell signal handler


    if(getcwd(cwd, 100) == NULL){
        perror("getcwd() error\n");
        return 1;
    }
    promt = strcat(cwd, "$");
    // printf("cwd:%s\n", cwd);

    while(1){
        if(getcwd(cwd, 100) == NULL){
            perror("getcwd() error\n");
            return 1;
        }
        promt = strcat(cwd, "$");
        char *line = NULL;
        size_t len = 0;
        ssize_t linesize = 0;

        highlight();
        printf("%s", promt);
        reset();printf(" ");

        struct cmd_struct** cmd_list = NULL;
        char* op_list = "";


        linesize = getline(&line, &len, stdin);
        line[linesize-1] = '\0';

        int total_commands = 0;
        cmd_list = parse( &line, &total_commands);

        
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

/*      SIGNAL HANDLING CODE                                   */

void signal_init_shell(void){
    struct sigaction sigint, sigtstp;
    sigint.sa_handler = int_handler;
    sigtstp.sa_handler = stop_handler;
    sigint.sa_flags = sigtstp.sa_flags = SA_RESTART;
    sigemptyset(&sigtstp.sa_mask);
    sigemptyset(&sigint.sa_mask);
    sigaction(SIGINT, &sigint, NULL);
    sigaction(SIGTSTP, &sigtstp, NULL);
}

void int_handler(int signo){
    if(runnning_proc==0){       // Parent Shell
        // highlight();
        // printf("%s", promt);
        // reset();printf(" ");
        printf("\n");
        fflush(stdout);
        exit(0); //TODO: Comment this before submission
        return;
    }
    else{                       // Child process 
        kill(runnning_proc, SIGINT);
        runnning_proc=0;
        printf("\n");
        fflush(stdout);
    }
}

void stop_handler(int signo){
    if(runnning_proc == 0){
        exit(0);
    }else{
        // printf("in stop hadler:runnning_proc:%d runnning_proc_name:%s\n",runnning_proc, runnning_proc_name);
        if(check_for_jobs(job_list, runnning_proc) == 1){ // Already present in job list
            // printf("Job Already present in job list. Do not increase counter\n");
            stopJob(&job_list, runnning_proc);
        }else{
            stopped_job_counter++;
            insertJob( &job_list,stopped_job_counter, runnning_proc, runnning_proc_name);
        }

        printf("\n[%d]+ Stopped", stopped_job_counter); printf("\t\t%s\n", runnning_proc_name);
        kill(runnning_proc, SIGTSTP);
        fflush(stdout);
        runnning_proc = 0;
        strcpy(runnning_proc_name,"");
    }
}



void signal_init_child(){
    signal(SIGINT, SIG_DFL);
    signal(SIGTSTP, SIG_DFL);
}

void backgound(int job_no){
    // printf("In background(). job_no:%d\n", job_no);
    int cpid;
    int job_index = job_no!=-1 ? job_no : 1;
    char curr_proc_name[20];
    cpid = get_proc_to_run( &job_list,job_no, curr_proc_name);

    if(cpid == -1){
        // printf("No jobs pending\n");
        return;
    }
    // printf("Process found. job index:%d, pid:%d name:%s\n", job_index, cpid, curr_proc_name);
    runnning_proc = cpid;
    kill(cpid, SIGCONT);
    return;
}

void foreground(int job_no){
    // printf("In forground(). job_no:%d\n", job_no);
    int cpid;
    int job_index = job_no!=-1 ? job_no : 1;
    char curr_proc_name[20];
    cpid = get_proc_to_run( &job_list,job_no, curr_proc_name);

    if(cpid == -1){
        // printf("No jobs pending\n");
        return;
    }
    // printf("Process found. job index:%d, pid:%d name:%s\n", job_index, cpid, curr_proc_name);
    runnning_proc = cpid;
    kill(cpid, SIGCONT);
    waitpid(runnning_proc, 0, WUNTRACED);                       // Suspend shell process and wait for this forground process to finish
    runnning_proc = 0;
    return;
}

void showJobs(){
    // printf("In showjobs()\n");
    struct job* temp = job_list;
    while(temp!=NULL){
        // printf("%d %d %s ", temp->job_id, temp->pid, temp->name);
        printf("[%d]+ ", temp->job_id);
        if(temp->status == RUNNING){
            printf("Running");
        } 
        else {
            printf("Stopped");
        }
        printf("\t\t%s\n", temp->name);
        temp = temp->next;
    }
}