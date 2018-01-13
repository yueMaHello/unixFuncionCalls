//
//  a1shell.c
//  Created by Yue Ma on 2017-09-28.
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/times.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <time.h>

void a1monitorProcess(int interval);/*child process prototype*/
void a1shellProcess(void);/*parent process prototypr*/
int changeDirectory(char* path);
int execute_bash(char* cmd);
void exe(char* userCommand);
void printMask(void);
//main function
int main(int argc, char *argv[]){
    int interval = atoi(argv[1]);
    // cpu time limit in seconds (10*60 secs)
    struct rlimit cpu_lim = {600, 600};
    if(setrlimit(RLIMIT_CPU, &cpu_lim) == -1) {
        perror("setrlimit: failed");
        return -1;
    }
    //fork a child proess a1monitor
    pid_t pid;
    pid = fork();
    if(pid == 0){
        a1monitorProcess(interval);/*child process*/
    }
    else if(pid>0){
        //wait until child process prints everything
        sleep(1);
        a1shellProcess();/*parent process*/
    }
    else{
        printf("Error: fork() failed. Please try again!");
    }
    return 0;
}
void a1monitorProcess(int interval){
    pid_t selfPid = getpid();//a1monitor pid
    pid_t ppid = getppid(); //a1shell pid
    pid_t ppid_curr = ppid;//current parent's pid
    FILE* loadavg;
    static float one, fifteen, five;
    static char procs[5];
    //execute until ppid_cur!=ppid
    while(ppid_curr == ppid) {
        //observe the parent pid and renew it
        ppid_curr = getppid();
        /*get current time*/
        if(ppid_curr != ppid)
            _Exit(EXIT_SUCCESS);
        time_t t = time(NULL);
        struct tm *tmp = localtime(&t);
        char s[64];
        printf("\n--------a1monitor--------------\n");
        // displays the time and date
        strftime(s, 64, "Time and date: %r, %a %b %d, %Y", tmp);
        printf("%s\n", s);
        if((loadavg = fopen("/proc/loadavg", "r")) == NULL)/*read /proc/loadavg*/
            perror("popen failed\nError ");
        fscanf(loadavg, "%f %f %f %s", &one, &five, &fifteen, procs);
        fclose(loadavg);
        //load average and number of running processes
        printf("Load average:  %.3f %.3f %.3f\na1monitor>> Processes:  %s\n", one, five, fifteen, procs);
        printf("--------------------------------\n\n");
        sleep(interval);
    }
    //parent has been killed
    //the child process should also exist
    kill(selfPid,SIGKILL);
}
//parent process
void a1shellProcess(void){
    while(1){
        char userCommand[80];
        userCommand[0] = '\0';
        //let user input some command
        printf("a1shell%%");
        //get the first char in stdout
        scanf("%s",userCommand);
        exe(userCommand);
    }
}
//cd .....
int changeDirectory(char* path){
    scanf("%s", path);
    if (path[0] == '$'){
        memmove(path, path+1, strlen(path));
        if(chdir(getenv(path))<0){
            
            //printf("chdir failed1\n");
            return 0;
        }
        else{
            //printf("chdir success1\n");
            return 1;
        }
        
    }
    else{
        if(chdir(path)<0){
            //printf("chdir failed2\n");
            return 0;
        }
        else{
            //printf("chdir success2\n");
            return 1;
        }
    }
}
// Umask
void printMask(void) {
    mode_t mask = umask(0); // always suceeds
    printf("a1shell>> current mask: %04o\n", mask);
    printf("a1shell>> S_IRWXU: %04o\n", S_IRWXU);
    printf("a1shell>> S_IRWXG: %04o\n", S_IRWXG);
    printf("a1shell>> S_IRWXO: %04o\n", S_IRWXO);
}
// other commands will be executed by Bash
int execute_bash(char* cmd){
    //args
    char args[1024];
    char cmd_args[1024];
    fgets(args,sizeof(args),stdin);
    strcpy(cmd_args, cmd);
    strcat(cmd_args, args);
    struct tms tmpstart,tmpend;
    clock_t start,end;
    double clktck = sysconf(_SC_CLK_TCK);
    start = times(&tmpstart);
    //fork a child process: executeBash
    pid_t pid;
    pid = fork();
    int status;
    
    //child process: executeBash
    if(pid == 0){
        execl("/bin/bash", "bash", "-c",cmd_args, (char*) 0);
        //execl only returns failure
        perror("executeBash>> failed");
        return -1;
    }
    else if(pid == -1){
        printf("executeBash>> fork failed\n");
        _exit(EXIT_FAILURE);
        
    }
    //parent process
    else{
        pid = waitpid(pid, &status, WNOHANG);
        while(pid == 0) {
            pid = waitpid(pid, &status, WNOHANG);
            if(pid == -1)
                perror("-a1shell: waitpid on execl failed");
        }
        end = times(&tmpend);
        //print time in a proper format
        printf("---bash command executing time---\n");
        //parent process
        printf("a1shell(parent)>> total time elapsed: %lf\n",(end-start)/clktck);
        printf("a1shell(parent)>> total user time: %lf\n",(tmpend.tms_utime-tmpstart.tms_utime)/clktck);
        printf("a1shell(parent)>> total cpu time: %lf\n",(tmpend.tms_stime-tmpstart.tms_stime)/clktck);
        //child process
        printf("executeBash(child)>> total user time: %lf\n",(tmpend.tms_cutime-tmpstart.tms_cutime)/clktck);
        printf("executeBash(child)>> total cpu time: %lf\n",(tmpend.tms_cstime-tmpstart.tms_cstime)/clktck);
        printf("---------------------------------\n");
    }
    return 0;
    
}
void exe(char* userCommand){
    //if "cd ......"
    if(strcmp(userCommand, "cd") == 0){
        char path[80];
        path[0] = '\0';
        int changeDir = changeDirectory(path);
    }
    //if "pwd"
    else if(strcmp(userCommand, "pwd") == 0){
        //check current working directory
        char cwd[1024];
        if (getcwd(cwd, sizeof(cwd)) != NULL)
            fprintf(stdout, "a1shell>> Current working dir: %s\n", cwd);
        else
            perror("a1shell>> getcwd() error");
    }
    //if "umask"
    else if(strcmp(userCommand, "umask") == 0){
        printMask();
    }
    //if "done"
    else if(strcmp(userCommand, "done") == 0){
        _exit(EXIT_SUCCESS);
    }
    //if other commands
    else{
        if (execute_bash(userCommand) == -1)
            _exit(EXIT_FAILURE);
    }
}
