#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <signal.h>

struct cmd_struct
{
    char *command;
    int is_in_redirection;
    int is_out_redirection;
    int arg_count;
    char **arglist;
    char *file_name;
    // For miltiple redirections
    char **file_names;
    char *redirections;
    int is_background;
} cmd_struct;

/* Structure to hold information about signle command line */
struct cmd_line_struct
{
    struct cmd_struct *cmd_list;
    int no_of_commands;

} cmd_line_struct;


enum redirection
{
    NO,
    IN,
    OUT
};

int runnning_proc;
char runnning_proc_name[40];
char *promt;
int stopped_job_counter;
 

void print_cmd_struct(struct cmd_struct *ptr);
struct cmd_struct *make_cmd_struct(char *cmd);
int find_no_commands(char *line);
struct cmd_struct **parse(char **line, int *total_commands);
int check_for_write(int command_no, int no_of_pipes);
int check_for_write(int command_no, int no_of_pipes);
int check_for_read(int command_no, int no_of_pipes);
void execute_single_command(struct cmd_struct *ptr);
void execute_commands_list(struct cmd_struct **cmd_list, int total_commands);
void highlight();
void reset();