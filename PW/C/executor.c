#include "err.h"
#include "utils.h"
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <unistd.h>
#include <assert.h>

#define MAX_LINE_LEN 1022
#define MAX_COMM_LEN 512
#define MAX_TASKS 4096

typedef struct _task {
    int id;
    pid_t pid;
    int stdout_pipe[2];
    int stderr_pipe[2];
    char* stdout_lastline;
    char* stderr_lastline;
    bool working;
} task_t;

task_t** tasks;
int num_of_tasks = 0;
bool working = true;

task_t* find_task(int task_id)
{
    for (int i = 0; i < MAX_TASKS; i++) {
        if (tasks[i] != NULL && tasks[i]->id == task_id)
            return tasks[i];
    }
    return NULL;
}

void last_line_reader(int fd, char* buffer)
{   
    FILE* fp = fdopen(fd, "r");
    char *line = malloc(MAX_LINE_LEN * sizeof(char));

    while(read_line(line, MAX_LINE_LEN, fp)){
        strncpy(buffer, line, MAX_LINE_LEN);
    }
    free(line);
    close(fd);
    exit(0);
}

void run_handle(char** args)
{
    if (num_of_tasks == MAX_TASKS) {
        printf("Error: maximum number of tasks reached\n");
        return;
    }
    int id;
    for (int i = 0; i < MAX_TASKS; i++) {
        if (tasks[i] == NULL) {
            id = i;
            break;
        }
    }
    tasks[id] = malloc(sizeof(task_t));
    tasks[id]->id = id;
    tasks[id]->working = true;

    tasks[id]->stdout_lastline = mmap(NULL, MAX_LINE_LEN * sizeof(char), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    tasks[id]->stderr_lastline = mmap(NULL, MAX_LINE_LEN * sizeof(char), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    tasks[id]->stdout_lastline[0] = '\0';
    tasks[id]->stderr_lastline[0] = '\0';

    pipe(tasks[id]->stdout_pipe);
    pipe(tasks[id]->stderr_pipe);

    if(!fork()){ // stdout reader
        munmap(tasks[id]->stderr_lastline, MAX_LINE_LEN * sizeof(char));
        close(tasks[id]->stdout_pipe[1]);
        close(tasks[id]->stderr_pipe[1]);
        close(tasks[id]->stderr_pipe[0]);
        last_line_reader(tasks[id]->stdout_pipe[0], tasks[id]->stdout_lastline);
    }
    if(!fork()){ // stderr reader
        munmap(tasks[id]->stdout_lastline, MAX_LINE_LEN * sizeof(char));
        close(tasks[id]->stdout_pipe[1]);
        close(tasks[id]->stderr_pipe[1]);
        close(tasks[id]->stdout_pipe[0]);
        last_line_reader(tasks[id]->stderr_pipe[0], tasks[id]->stderr_lastline);
    }
    if ((tasks[id]->pid = fork()) == 0) {
        munmap(tasks[id]->stdout_lastline, MAX_LINE_LEN * sizeof(char));
        munmap(tasks[id]->stderr_lastline, MAX_LINE_LEN * sizeof(char));

        close(tasks[id]->stdout_pipe[0]);
        dup2(tasks[id]->stdout_pipe[1], STDOUT_FILENO);
        close(tasks[id]->stdout_pipe[1]);

        close(tasks[id]->stderr_pipe[0]);
        dup2(tasks[id]->stderr_pipe[1], STDERR_FILENO);
        close(tasks[id]->stderr_pipe[1]);

        execv(args[1], &args[1]);
        execvp(args[1], &args[1]);
        printf("Error: executing program %s failed\n", args[1]);
        exit(EXIT_FAILURE);
    } else {
        close(tasks[id]->stdout_pipe[1]);
        close(tasks[id]->stderr_pipe[1]);
        close(tasks[id]->stdout_pipe[0]);
        close(tasks[id]->stderr_pipe[0]);
        num_of_tasks++;
        printf("Task %d started: pid %d\n", tasks[id]->id, tasks[id]->pid);
    }
}

int num_of_args(char** args)
{
    int cnt = 0;
    while (args[cnt] != NULL)
        cnt++;
    return cnt;
}

void out_handle(char** input)
{
    if (num_of_args(input) != 2) {
        printf("Error: incorrect number of arguments\n");
        return;
    }
    int id = atoi(input[1]);
    task_t* task = find_task(id);
    if (!task) {
        printf("Error: task %d does not exist\n", id);
        return;
    }

    char* line = task->stdout_lastline;
    printf("Task %d stdout: '%s'.\n", id, line);
}

void err_handle(char** input)
{
    if (num_of_args(input) != 2) {
        printf("Error: incorrect number of arguments\n");
        return;
    }
    int id = atoi(input[1]);
    task_t* task = find_task(id);
    if (!task) {
        printf("Error: task %d does not exist\n", id);
        return;
    }

    char* line = task->stderr_lastline;
    printf("Task %d stderr: '%s'.\n", id, line);
}

void kill_handle(char** input)
{
    if (num_of_args(input) != 2) {
        printf("Error: incorrect number of arguments\n");
        return;
    }
    int id = atoi(input[1]);
    task_t* task = find_task(id);
    if (!task) {
        printf("Error: task %d does not exist\n", id);
        return;
    }
    kill(task->pid, SIGINT);
}

void sleep_handle(char** input)
{
    if (num_of_args(input) != 2) {
        printf("Error: incorrect number of arguments\n");
        return;
    }
    int milisecs = atoi(input[1]);
    usleep(milisecs * 1000);
}

void handle_end_of_task(pid_t pid, int status)
{
    int id;
    for (int i = 0; i < MAX_TASKS; i++) {
        if (tasks[i] != NULL && tasks[i]->pid == pid) {
            id = tasks[i]->id;
            break;
        }
    }
    tasks[id]->working = false;

    if (WIFEXITED(status))
        printf("Task %d ended: status %d.\n", id, WEXITSTATUS(status));
    else if (WIFSIGNALED(status))
        printf("Task %d ended: signalled.\n", id);
    else
        printf("Task %d ended: unexpected end result.\n", id);
}

void quit_handle()
{
    for (int i = 0; i < MAX_TASKS; i++) {
        if (tasks[i] == NULL)
            continue;
        task_t* task = tasks[i];
        kill(task->pid, SIGKILL);
        kill(-task->pid, SIGKILL);
        wait(NULL);
        close(task->stdout_pipe[0]);
        close(task->stderr_pipe[0]);
        int id = task->id;
        if(task->working)
            printf("Task %d ended: signalled.\n", task->id);
        free(task);
        tasks[id] = NULL;
        num_of_tasks--;
    }
    working = false;
    free(tasks);
}

void parse_input(char** input)
{
    if (strcmp(input[0], "run") == 0)
        run_handle(input);
    else if (strcmp(input[0], "out") == 0)
        out_handle(input);
    else if (strcmp(input[0], "err") == 0)
        err_handle(input);
    else if (strcmp(input[0], "kill") == 0)
        kill_handle(input);
    else if (strcmp(input[0], "sleep") == 0)
        sleep_handle(input);
    if (strcmp(input[0], "quit") == 0 || feof(stdin))
        quit_handle();
}

void check_children_status(int signal)
{
    assert(signal == SIGCHLD);
    if (num_of_tasks == 0)
        return;
    int status;
    for (int i = 0; i < MAX_TASKS; i++) {
        if (tasks[i] == NULL)
            continue;
        if (!tasks[i]->working)
            continue;
        pid_t pid = tasks[i]->pid;
        if (waitpid(pid, &status, WNOHANG) != 0)
            handle_end_of_task(pid, status);
    }
}

int main()
{
    char command[MAX_COMM_LEN];
    tasks = malloc(MAX_TASKS * sizeof(task_t));

    for (int i = 0; i < MAX_TASKS; i++)
        tasks[i] = NULL;

    struct sigaction chld_handle;
    chld_handle.sa_handler = check_children_status;
    sigemptyset(&chld_handle.sa_mask);
    sigaddset(&chld_handle.sa_mask, SIGCHLD);
    chld_handle.sa_flags = SA_RESTART;
    sigaction(SIGCHLD, &chld_handle, NULL);

    sigset_t blocked;
    sigemptyset(&blocked);
    sigaddset(&blocked, SIGCHLD);

    setbuf(stdin, 0);
    setbuf(stdout, 0);

    while (working) {
        read_line(command, MAX_COMM_LEN, stdin);
        char** comm_parsed = split_string(command);

        sigprocmask(SIG_BLOCK, &blocked, NULL);
        parse_input(comm_parsed);
        sigprocmask(SIG_UNBLOCK, &blocked, NULL);

        free_split_string(comm_parsed);
    }
}