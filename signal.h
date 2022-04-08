#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <signal.h>

extern struct job *job_list;

void signal_init_shell(void);
void signal_init_child();
void int_handler(int);
void stop_handler(int);
void cont_handler(int);
void background();
void foreground();
void add_background_running_job(int runnning_proc, char *runnning_proc_name);
void showJobs();
void kill_job(int job_no);
void kill_all_jobs(void);


