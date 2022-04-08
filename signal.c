#include "signal.h"
#include "shell.h"
#include "list.h"
extern int runnning_proc;
/*      SIGNAL HANDLING CODE                                   */

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
    if (runnning_proc == 0)
    { // Parent Shell
        highlight();
        printf("%s", promt);
        reset();
        printf(" ");
        printf("\n");
        fflush(stdout);
        // exit(0); //TODO: Comment this before submission
        return;
    }
    else
    { // Child process
        kill(runnning_proc, SIGINT);
        runnning_proc = 0;
        fflush(stdout);
    }
}

void stop_handler(int signo)
{
    // printf("stop handler\n");
    // printf("running proc:%d\t running proc name:%s\n", runnning_proc, runnning_proc_name);
    if (runnning_proc == 0)
    {
        // kill(runnning_proc, SIGTSTP);
        kill(runnning_proc, SIGSTOP);
        return;
    }
    else
    {
        // printf("in stop hadler:runnning_proc:%d runnning_proc_name:%s\n",runnning_proc, runnning_proc_name);
        if (check_for_jobs(job_list, runnning_proc) == 1)
        { // Already present in job list
            // printf("Job Already present in job list. Do not increase counter\n");
            stopJob(&job_list, runnning_proc);
        }
        else
        {
            stopped_job_counter++;
            insertJob(&job_list, stopped_job_counter, runnning_proc, runnning_proc_name);
        }

        printf("\n[%d]+ Stopped", stopped_job_counter);
        printf("\t\t%s\n", runnning_proc_name);
        // kill(runnning_proc, SIGTSTP);
        kill(runnning_proc, SIGSTOP);
        runnning_proc = 0;
        strcpy(runnning_proc_name, "SHELL");
        fflush(stdout);
        // printf("running proc:%d\t running proc name:%s\n", runnning_proc, runnning_proc_name);
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
    // printf("In background(). job_no:%d\n", job_no);
    // printf("running proc:%d\t running proc name:%s\n", runnning_proc, runnning_proc_name);
    int cpid;
    char curr_proc_name[20];
    cpid = get_proc_to_run_bg(&job_list, &job_no, curr_proc_name);

    if (cpid == -1)
    {
        // printf("No jobs pending\n");
        return;
    }
    printf("[%d]+   %s  &\n", job_no, curr_proc_name);
    // printf("Process found. job index:%d, pid:%d proc name:%s\n", job_index, cpid, curr_proc_name);
    runnning_proc = 0;
    strcpy(runnning_proc_name, "SHELL");
    // printf("running proc:%d\t running proc name:%s\n", runnning_proc, runnning_proc_name);

    kill(cpid, SIGCONT);
    return;
}

void foreground(int job_no)
{
    // printf("In forground(). job_no:%d\n", job_no);
    if (runnning_proc != 0)
    { // If running process is not shell then return
        return;
    }

    int cpid;
    int job_index = job_no;

    char curr_proc_name[20];
    cpid = get_proc_to_run_fg(&job_list, &job_index, curr_proc_name);
    if (job_index == -1)
    {
        // No job of index = job_index

        return;
    }
    if (cpid == -1)
    {
        // printf("No jobs pending\n");
        return;
    }
    // printf("Process found. job index:%d, pid:%d name:%s\n", job_index, cpid, curr_proc_name);
    runnning_proc = cpid;
    strcpy(runnning_proc_name, curr_proc_name);
    printf("[%d]+\t%d\t\t%s\n", job_index, runnning_proc, runnning_proc_name);
    // printf("running proc:%d\t running proc name:%s\n", runnning_proc, runnning_proc_name);
    removeJob(&job_list, job_index);
    kill(cpid, SIGCONT);
    waitpid(runnning_proc, 0, WUNTRACED); // Suspend shell process and wait for this forground process to finish
    runnning_proc = 0;
    strcpy(runnning_proc_name, "SHELL");
    return;
}

void showJobs()
{
    // printf("In showjobs()\n");
    struct job *temp = job_list;
    while (temp != NULL)
    {
        // printf("%d %d %s ", temp->job_id, temp->pid, temp->name);
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
        // printf("Sending kill signal to process:%d name:%s\n", pid, name);
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
