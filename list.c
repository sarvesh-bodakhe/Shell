#include "list.h"
void insertJob(struct job** job_list, int job_id, int pid, char* name){
    // printf("In insert job\n");
    // printf("pid:%d name:%s\n", pid, name);
    struct job *newjob= (struct job*)malloc(sizeof(job)*1);
    newjob->job_id = job_id;
    newjob->pid = pid;
    newjob->name = (char*)malloc(sizeof(char)*strlen(name));
    strcpy(newjob->name,name);
    newjob->status = 0;
    newjob->next = NULL;
    if(!(*job_list)){
        *job_list  = newjob;
    }else{
        struct job* temp = *job_list;
        while(temp->next != NULL){
            temp = temp->next;
        }
        temp->next = newjob;
    }
    return;
}



void removeJob(struct job** job_list, int job_id){
    if(*job_list == NULL){
        return;
    }
    struct job *temp = *job_list, *prev = NULL;
    if(temp != NULL && temp->job_id== job_id){
        *job_list = temp->next;
        free(temp);
        return;
    }
    else{
        while(temp!=NULL && temp->job_id != job_id){
             prev = temp;
            temp = temp->next;
        }
    }

    if (temp == NULL)
        return;
    
    prev->next = temp->next;
    free(temp);

    
}


int get_proc_to_run_bg(struct job** job_list, int *job_no, char* proc_name){
    // printf("In get_proc_to_run. job_no:%d\n", job_no);
    if(*job_list == NULL){
        // printf("No jobs pending\n");
        return -1;
    }
    struct job* temp = *job_list;
    int ret = -1;
    if(*job_no == -1){
        while(temp != NULL){
            if(temp->status == STOPPED){
                *job_no = temp->job_id;
                temp->status = RUNNING;
                strcpy(proc_name, temp->name);
                // printf("in get_proc_to_run. proc name:%s\n", temp->name);
                return temp->pid;
                
            }
            temp = temp->next;
        }
    }
    else{
        while(temp != NULL){
            if(temp->job_id == *job_no){
                if(temp->status == STOPPED){
                    // printf("Job found with job_no:%d\n", job_no);
                    temp->status = RUNNING;   
                    strcpy(proc_name, temp->name);
                    return temp->pid;
                }else{          // Job is already running
                    return -1;
                }
            }
            temp = temp->next;
        }
    }

    // printf("After get_proc_to_run .Job List:\n");
    // temp = *job_list;
    // while(temp!=NULL){
    //     printf("%d %d %s ", temp->job_id, temp->pid, temp->name);
    //     if(temp->status) printf("RUNNING\n");
    //     else printf("STOPPED\n");
    //     temp = temp->next;
    // }

    return ret;
}

int get_proc_to_run_fg(struct job** job_list, int *job_no, char* proc_name){
    if(*job_list == NULL){
        // printf("No jobs pending\n");
        return -1;
    }
    struct job* temp = *job_list;
    int ret = -1;
    if(*job_no == -1){
        while(temp != NULL){
            if(temp->status == RUNNING || temp->status==STOPPED){
                temp->status = RUNNING;
                *job_no = temp->job_id;
                // printf("in get_proc_to_run_fg. proc name:%s\n", temp->name);
                strcpy(proc_name, temp->name);
                return temp->pid;
            }
            temp = temp->next;
        }
    }
    else{
        while(temp != NULL){
            if(temp->job_id == *job_no){
                if(temp->status == RUNNING){
                    // printf("Job found with job_no:%d\n", job_no);
                    // temp->status = RUNNING;   
                    // printf("in get_proc_to_run_fg. proc name:%s\n", temp->name);
                    strcpy(proc_name, temp->name);
                    return temp->pid;
                }else{          // Make job running
                    temp->status = RUNNING;
                    // printf("in get_proc_to_run_fg. proc name:%s\n", temp->name);
                    strcpy(proc_name, temp->name);
                    return temp->pid;
                    // return -1;
                }
            }
            temp = temp->next;
        }
    }
    *job_no = -1;
}

int check_for_jobs(struct job* job_list, int pid){
    // printf("In check_for_jobs(). pid:%d\n", pid);
    struct job *temp = job_list;
    while(temp){
        if(temp->pid == pid){
            // printf("Job found.jobid:%d\n", temp->job_id);
            return 1;
        }
        temp = temp->next;
    }
    return 0;
}


void stopJob(struct job** job_list, int pid){
    // printf("In stopJob with pid:%d\n",pid);
    struct job *temp = *job_list;
    while(temp){
        if(temp->pid == pid){
            // printf("Making job:%d as STOPPED\n", temp->job_id);
            temp->status = STOPPED;
            return;
        }
    }
    return;
}

char* get_job_name(struct job* job_list, int job_no){
    struct job* temp = job_list;
    while(temp){
        if(temp->job_id == job_no){
            return temp->name;
        }
        temp = temp->next;
    }

}

void make_job_running(struct job** job_list, int job_no){
    struct job *temp = *job_list;
    while(temp!=NULL){
        if(temp->job_id == job_no){
            temp->status = RUNNING;
            return ;
        }
        temp = temp->next;
    }
    return ;
}

int get_proc_to_kill(struct job *job_list,int job_no, char* name){
    struct job* temp = job_list;
    while(temp){
        if(temp->job_id == job_no){
            strcpy( name, temp->name);
            // printf("Process name to kill:%s %d\n", name, temp->pid);
            return temp->pid;
        }
        temp = temp->next;
    }
    return -1;
}  