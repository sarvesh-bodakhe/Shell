#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include<string.h>
#include<fcntl.h> 
#include<stdlib.h>
#include<signal.h>

#define STOPPED 0
#define RUNNING 1


struct job{
    int job_id;
    int pid;
    int status;
    char* name;
    struct job *next;
}job;

void insertJob(struct job** job_list, int job_id, int pid, char* name);
void removeJob(struct job** job_list, int job_id);
int get_proc_to_run(struct job** job_list, int job_no, char* proc_name);
int get_proc_to_run_fg(struct job** job_list, int job_no, char* proc_name);
int check_for_jobs(struct job* job_list, int pid);
void stopJob(struct job** job_list, int pid);
char* get_job_name(struct job* job_list, int job_no);
void make_job_running(struct job** job_list, int job_no);
int get_proc_to_kill(struct job* job_list, int job_no, char* name);