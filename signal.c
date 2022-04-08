#include "signal.h"
#include "shell.h"
#include "list.h"
extern int runnning_proc;

void signal_init_shell(void)
{
    struct sigaction sigint, sigtstp;
    sigint.sa_handler = int_handler;
    sigtstp.sa_handler = stop_handler;
    sigint.sa_flags = sigtstp.sa_flags = SA_RESTART;
    sigemptyset(&sigtstp.sa_mask);
    sigemptyset(&sigint.sa_mask);
    sigaction(SIGINT, &sigint, NULL);
    sigaction(SIGTSTP, &sigtstp, NULL);
}

void int_handler(int signo)
{
     /* Parent Shell */
    if (runnning_proc == 0)
    {
        highlight();
        printf("%s", promt);
        reset();
        printf(" ");
        printf("\n");
        fflush(stdout);
        return;
    }
    /* Child process */
    else
    { 

        kill(runnning_proc, SIGINT);
        runnning_proc = 0;
        fflush(stdout);
    }
}

void stop_handler(int signo)
{
    if (runnning_proc == 0)
    {
        kill(runnning_proc, SIGSTOP);
        return;
    }
    else
    {
        /*  Already present in job list
            Do not increase counter\n");
        */
        if (check_for_jobs(job_list, runnning_proc) == 1)
        { 
            stopJob(&job_list, runnning_proc);
        }
        else
        {
            stopped_job_counter++;
            insertJob(&job_list, stopped_job_counter, runnning_proc, runnning_proc_name);
        }

        printf("\n[%d]+ Stopped", stopped_job_counter);
        printf("\t\t%s\n", runnning_proc_name);
        kill(runnning_proc, SIGSTOP);
        runnning_proc = 0;
        strcpy(runnning_proc_name, "SHELL");
        fflush(stdout);
    }
}

void signal_init_child()
{
    signal(SIGINT, SIG_DFL);
    signal(SIGTSTP, SIG_DFL);
}

void add_background_running_job(int runnning_proc, char *runnning_proc_name)
{
    stopped_job_counter++;
    printf("[%d] %d\n", stopped_job_counter, runnning_proc);
    insertJob(&job_list, stopped_job_counter, runnning_proc, runnning_proc_name); // Inserts stopped job in job list
    make_job_running(&job_list, stopped_job_counter);
}

void background(int job_no)
{
    int cpid;
    char curr_proc_name[20];
    cpid = get_proc_to_run_bg(&job_list, &job_no, curr_proc_name);

    if (cpid == -1)
    {
        return;
    }
    printf("[%d]+   %s  &\n", job_no, curr_proc_name);
    runnning_proc = 0;
    strcpy(runnning_proc_name, "SHELL");
    kill(cpid, SIGCONT);
    return;
}

void foreground(int job_no)
{
    if (runnning_proc != 0)
    {
        return;
    }
    int cpid;
    int job_index = job_no;

    char curr_proc_name[20];
    cpid = get_proc_to_run_fg(&job_list, &job_index, curr_proc_name);
    if (job_index == -1)
    {
        return;
    }
    if (cpid == -1)
    {
        return;
    }
    runnning_proc = cpid;
    strcpy(runnning_proc_name, curr_proc_name);
    printf("[%d]+\t%d\t\t%s\n", job_index, runnning_proc, runnning_proc_name);
    removeJob(&job_list, job_index);
    kill(cpid, SIGCONT);
    waitpid(runnning_proc, 0, WUNTRACED);
    runnning_proc = 0;
    strcpy(runnning_proc_name, "SHELL");
    return;
}

void showJobs()
{
    struct job *temp = job_list;
    while (temp != NULL)
    {
        printf("[%d]+ ", temp->job_id);
        if (temp->status == RUNNING)
        {
            printf("Running");
        }
        else
        {
            printf("Stopped");
        }
        printf("\t\t%d", temp->pid);
        printf("\t\t%s\n", temp->name);

        temp = temp->next;
    }
}

void kill_job(int job_no)
{
    char name[20];
    int pid = get_proc_to_kill(job_list, job_no, name);
    if (pid != -1)
    {
        printf("[%d]+  Killed\t\t%d\t\t%s\n", job_no, pid, name);
        kill(9, pid);
        removeJob(&job_list, job_no);
    }
}

void kill_all_jobs()
{
    for (int i = 0; i <= stopped_job_counter; i++)
    {
        kill_job(i);
    }
}
