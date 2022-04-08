#include "shell.h"
#include "list.h"
#include "signal.h"
struct job *job_list = NULL;
int runnning_proc = 0;
void print_cmd_struct(struct cmd_struct *ptr)
{
    printf("Printing Command Strcture():\n");
    printf("\t1.Command:\t%s\n", ptr->command);
    printf("\t2.arg_count:\t%d\n", ptr->arg_count);
    printf("\t3.Arguments:\t");
    for (int i = 0; i < ptr->arg_count; i++)
    {
        printf("%s ", ptr->arglist[i]);
    }
    printf("\n");
    if (ptr->is_background)
        printf("\t4.Background Process\n");
    if (ptr->is_in_redirection == 1)
        printf("\t5.Input Redirection\n");
    if (ptr->is_out_redirection == 1)
        printf("\t6.Output Redirection\n");
    if (ptr->file_name)
        printf("\t7.file_name:%s\n", ptr->file_name);
}

/*  Sarvesh:    Make a command structure out of string 
                Check for redierctions/ Files associated
                Returns a pointer to the structre
*/
struct cmd_struct *make_cmd_struct(char *cmd)
{
    // printf("\tIn make_cmd_struct():\n");
    // printf("In make_cmd_struct.cmd=%s\n", cmd);
    int i = 0, j = 0;
    int flag = 1;
    int arg_count = 0;
    int redirection_flag = 0;
    struct cmd_struct *ptr = (struct cmd_struct *)malloc(sizeof(cmd_struct));
    ptr->arglist = (char **)malloc(sizeof(char **) * 10);
    while (cmd[j] != '\0')
    {
        // printf("\t%d:%c\n", j, cmd[j]);
        if (cmd[j] == ' ')
        {
            if (flag == 0)
            {
                // printf("\tWord from size=%d. %d to %d:",j-i+1, i, j);
                int size = j - i;
                char *str = (char *)malloc(sizeof(char) * (size + 1));
                int x = 0;
                for (int k = i; k < j; k++)
                {
                    str[x++] = cmd[k];
                }
                str[x] = '\0';
                // printf("%s\n", str);
                /* Redirection Operator has encountered */
                if (redirection_flag == 1)
                {
                    // printf("\tFile after redirection:%s\n", str);
                    ptr->file_name = (char *)malloc(sizeof(str));
                    ptr->file_name = str;
                    flag = 1;
                    continue;
                }
                if (strcmp(str, "<") == 0)
                {
                    // printf("\tInput Redirection Detected:%s\n", str);
                    ptr->is_in_redirection = 1;
                    redirection_flag = 1;
                    flag = 1;
                    continue;
                }
                if (strcmp(str, ">") == 0)
                {
                    // printf("\tOutput Redirection Detected:%s\n", str);
                    ptr->is_out_redirection = 1;
                    flag = 1;
                    redirection_flag = 1;
                    continue;
                }
                ptr->arglist[arg_count] = (char *)malloc(sizeof(str));
                ptr->arglist[arg_count] = str;
                arg_count++;
                flag = 1;
            }
            flag = 1;
        }
        else
        {
            if (flag == 1)
            {
                flag = 0;
                i = j;
            }
        }
        j++;
    }
    if (flag == 0)
    {
        char *str = (char *)malloc(sizeof(char) * (j - i + 1));
        int x = 0;
        for (int k = i; k < j; k++)
        {
            str[x++] = cmd[k];
        }
        str[x] = '\0';
        if (redirection_flag == 1)
        {
            // printf("\tFile after redirectoin:%s\n", str);
            ptr->file_name = (char *)malloc(sizeof(str));
            ptr->file_name = str;
            flag = 1;
        }
        else
        {
            ptr->arglist[arg_count] = (char *)malloc(sizeof(str));
            ptr->arglist[arg_count] = str;
            arg_count++;
        }
    }
    // printf("Command:%s\n", ptr->arglist[0]);
    ptr->command = ptr->arglist[0];
    ptr->arg_count = arg_count;
    if (strcmp(ptr->arglist[ptr->arg_count - 1], "&") == 0)
    {
        // printf("in make struct. this is background process\n");
        ptr->is_background = 1;
        ptr->arg_count -= 1;
    }
    // print_cmd_struct(ptr);
    return ptr;
}

int find_no_commands(char *line)
{
    int total_commands = 0;
    char *token1 = strtok(line, "|");
    while (token1 != NULL)
    {
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
struct cmd_struct **parse(char **line, int *total_commands)
{
    // printf("\nIn parse():\n");
    // printf("In parse() function. Line:%s\n", *line);
    char pipeline_delim = '|';
    char input_redirection_delim = '<';
    char output_redirection_delin = '>';

    /* Maxmimum Of 10 Commnads in a single line     */
    struct cmd_struct **cmd_list = (struct cmd_struct **)malloc(sizeof(struct cmd_struct **) * 10);
    int cmd_count = 0;
    char *token = strtok(*line, "|");
    // printf("\tTravesring All Commands\n");
    while (token != NULL)
    {
        // printf("\tCommand No.%d => %s\n", cmd_count+1, token);
        // Make a command struct from string pointed by ptr
        struct cmd_struct *ptr = (struct cmd_struct *)malloc(sizeof(struct cmd_struct));
        ptr = make_cmd_struct(token);
        /*
        for multiple redirections
        
        */
        cmd_list[cmd_count] = (struct cmd_struct *)malloc(sizeof(struct cmd_struct *));
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
int check_for_write(int command_no, int no_of_pipes)
{
    if (no_of_pipes == 0)
        return 0;
    if (command_no == 0)
        return 1;
    if (command_no < no_of_pipes)
        return 1;
    return 0;
}

/*   Check if particular command reads from  the pipe       */
int check_for_read(int command_no, int no_of_pipes)
{
    if (no_of_pipes == 0)
        return 0;
    if (command_no == 0)
        return 0;
    if (command_no < no_of_pipes + 1)
        return 1;
    return 0;
}

/*   Utility function to execture signle function represented by command structure   */
void execute_single_command(struct cmd_struct *ptr)
{
    if (ptr->is_out_redirection)
    {
        /*Closing Screen Output*/
        // printf("\toutput redirection.calling close(1)\n");
        close(1);
        int fd_temp = open(ptr->file_name, O_CREAT | O_RDWR, 0666);
        // printf("\tclose(1) finished\n");
        // printf("\tNew File Descriptor:%d\n", fd_temp);
    }
    if (ptr->is_in_redirection)
    {
        // printf("\tinput redirection.calling close(0)\n");
        close(0);
        int fd = open(ptr->file_name, O_RDONLY);
        // printf("\tnew file descriptor fd=%d\n", fd);
    }

    if (execvp(ptr->command, ptr->arglist) == -1)
    {
        printf("%s : No such command\n", ptr->command);
    }
}

/*      Execute list of commands       */
void execute_commands_list(struct cmd_struct **cmd_list, int total_commands)
{
    int no_of_pipes = total_commands - 1;
    // printf("\n\nIn execute_commands():Total Commands:%d\n\tNumber Of pipes required:%d\n\n", total_commands, no_of_pipes);
    int flag_write_into_pipe = 0, flag_read_from_pipe = 0, pipe_flag = 0;
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
    if (strcmp(cmd_list[0]->command, "jobs") == 0)
    {
        showJobs();
        return;
    }

    if (strcmp(cmd_list[0]->command, "exit") == 0 || strcmp(cmd_list[0]->command, "quit") == 0)
    {
        kill_all_jobs();
        exit(0);
    }

    if (strcmp(cmd_list[0]->command, "bg") == 0)
    {
        if (cmd_list[0]->arg_count == 1)
        {
            background(-1);
            return;
        }
        else if (cmd_list[0]->arg_count == 2)
        {
            int job_no = cmd_list[0]->arglist[1][1] - '0';
            background(job_no);
            return;
        }
        else
        {
            return;
        }
    }
    if (strcmp(cmd_list[0]->command, "fg") == 0)
    {
        if (cmd_list[0]->arg_count == 1)
        {
            foreground(-1);
            return;
        }
        else if (cmd_list[0]->arg_count == 2)
        {
            int job_no = cmd_list[0]->arglist[1][1] - '0';
            foreground(job_no);
            return;
        }
        else
        {
            return;
        }
    }
    if (strcmp(cmd_list[0]->command, "kill") == 0 && cmd_list[0]->arglist[1][0] == '%')
    {
        // printf("Kill Job Command");
        int job_no = cmd_list[0]->arglist[1][1] - '0';
        kill_job(job_no);
        return;
    }

    /*Initialize Pipes*/
    int pipe_count = 0;
    int **pipe_arr = (int **)malloc(sizeof(int *) * no_of_pipes);
    int *process_arr = (int *)malloc(sizeof(int) * total_commands);

    for (int i = 0; i < no_of_pipes; i++)
    {
        pipe_arr[i] = (int *)malloc(sizeof(int) * 2);
        pipe(pipe_arr[i]);
    }

    for (int i = 0; i < total_commands; i++)
    {
        int is_background = 0;
        if (cmd_list[i]->is_background == 1)
            is_background = 1;

        process_arr[i] = fork();
        if (process_arr[i] == -1)
        {
            perror("process_arr[i] = fork() failed\n");
            exit(1);
        }
        if (process_arr[i] == 0)
        {
            signal_init_child();

            // printf("in child process: process_arr[%d]\n", i);
            if (check_for_read(i, no_of_pipes) == 1)
            {
                // printf("\tprocess_arr[%d] should read  from pipe no[%d]\n", i, i-1);
                close(0);
                char str3[100];
                dup(pipe_arr[i - 1][0]);
                close(pipe_arr[i - 1][1]);
            }
            if (check_for_write(i, no_of_pipes) == 1)
            {
                // printf("\tprocess_arr[%d] should write into pipe no[%d]\n", i, i);
                // printf("\tpipe[%d] created\n", i);
                close(1);
                dup(pipe_arr[i][1]);
                close(pipe_arr[i][0]);
            }

            struct cmd_struct *ptr = cmd_list[i];
            execute_single_command(ptr);
            exit(0);
        }
        if (process_arr[i] > 0)
        {
            runnning_proc = process_arr[i];
            strcpy(runnning_proc_name, cmd_list[i]->command);
            int child_id = process_arr[i];
            if (!is_background)
            {
                waitpid(runnning_proc, 0, WUNTRACED);
            }
            else
            { // Background process . Do Not wait
                // printf("background process");
                add_background_running_job(runnning_proc, runnning_proc_name);
            }
            runnning_proc = 0;
            strcpy(runnning_proc_name, "");
            if (check_for_read(i, no_of_pipes) == 1)
                close(pipe_arr[i - 1][0]);
            if (check_for_write(i, no_of_pipes) == 1)
                close(pipe_arr[i][1]);
            // printf("\tin parent of process[%d]\n", i);
        }
    }
    // printf("Out Of the for loop\n");
}

void highlight()
{
    printf("\033[0;45m");
}
void reset()
{
    printf("\033[0m");
}


