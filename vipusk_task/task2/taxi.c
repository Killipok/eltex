#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <errno.h>

#define MAX_DRIVERS 1024
#define BUF_SIZE 128

typedef enum { AVAILABLE, BUSY } driver_state_t;

struct driver_info {
    pid_t pid;
    int pipe_cli_to_driver[2];  
    int pipe_driver_to_cli[2];  
    driver_state_t state;
    int busy_timer;             
};

struct driver_info drivers[MAX_DRIVERS];
int driver_count = 0;


void driver_process(int read_fd, int write_fd) {
    char buf[BUF_SIZE];
    driver_state_t state = AVAILABLE;
    int busy_timer = 0;

    fd_set readfds;
    struct timeval tv;

    while (1) {
        FD_ZERO(&readfds);
        FD_SET(read_fd, &readfds);

        tv.tv_sec = 1;
        tv.tv_usec = 0;

        int ret = select(read_fd + 1, &readfds, NULL, NULL, &tv);
        if (ret > 0 && FD_ISSET(read_fd, &readfds)) {
            ssize_t n = read(read_fd, buf, sizeof(buf)-1);
            if (n <= 0) exit(0); 
            buf[n] = 0;

            if (strncmp(buf, "TASK ", 5) == 0) {
                int task_time = atoi(buf + 5);
                if (state == BUSY) {
                    char reply[BUF_SIZE];
                    snprintf(reply, sizeof(reply), "Busy %d\n", busy_timer);
                    write(write_fd, reply, strlen(reply));
                } else {
                    state = BUSY;
                    busy_timer = task_time;
                    char reply[BUF_SIZE];
                    snprintf(reply, sizeof(reply), "Started %d\n", busy_timer);
                    write(write_fd, reply, strlen(reply));
                }
            } else if (strcmp(buf, "STATUS") == 0) {
                char reply[BUF_SIZE];
                if (state == AVAILABLE)
                    snprintf(reply, sizeof(reply), "Available\n");
                else
                    snprintf(reply, sizeof(reply), "Busy %d\n", busy_timer);
                write(write_fd, reply, strlen(reply));
            }
        }

        // Update timer
        if (state == BUSY) {
            busy_timer--;
            if (busy_timer <= 0) {
                state = AVAILABLE;
                busy_timer = 0;
            }
        }
    }
}


struct driver_info* find_driver(pid_t pid) {
    for (int i = 0; i < driver_count; i++)
        if (drivers[i].pid == pid) return &drivers[i];
    return NULL;
}

void cmd_create_driver() {
    if (driver_count >= MAX_DRIVERS) {
        printf("Max drivers reached\n");
        return;
    }

    int pipe_cli_to_driver[2];
    int pipe_driver_to_cli[2];

    if (pipe(pipe_cli_to_driver) < 0 || pipe(pipe_driver_to_cli) < 0) {
        perror("pipe");
        return;
    }

    pid_t pid = fork();
    if (pid < 0) { perror("fork"); return; }
    if (pid == 0) {
        
        close(pipe_cli_to_driver[1]);
        close(pipe_driver_to_cli[0]);
        driver_process(pipe_cli_to_driver[0], pipe_driver_to_cli[1]);
        exit(0);
    } else {
     
        close(pipe_cli_to_driver[0]);
        close(pipe_driver_to_cli[1]);
        drivers[driver_count].pid = pid;
        drivers[driver_count].pipe_cli_to_driver[1] = pipe_cli_to_driver[1];
        drivers[driver_count].pipe_driver_to_cli[0] = pipe_driver_to_cli[0];
        drivers[driver_count].state = AVAILABLE;
        drivers[driver_count].busy_timer = 0;
        driver_count++;
        printf("Created driver with PID %d\n", pid);
    }
}

void cmd_send_task(pid_t pid, int task_timer) {
    struct driver_info *drv = find_driver(pid);
    if (!drv) { printf("No such driver %d\n", pid); return; }

    char cmd[BUF_SIZE];
    snprintf(cmd, sizeof(cmd), "TASK %d", task_timer);
    write(drv->pipe_cli_to_driver[1], cmd, strlen(cmd));

    char buf[BUF_SIZE];
    ssize_t n = read(drv->pipe_driver_to_cli[0], buf, sizeof(buf)-1);
    if (n > 0) { buf[n] = 0; printf("%s", buf); }
}

void cmd_get_status(pid_t pid) {
    struct driver_info *drv = find_driver(pid);
    if (!drv) { printf("No such driver %d\n", pid); return; }

    write(drv->pipe_cli_to_driver[1], "STATUS", 6);

    char buf[BUF_SIZE];
    ssize_t n = read(drv->pipe_driver_to_cli[0], buf, sizeof(buf)-1);
    if (n > 0) { buf[n] = 0; printf("%s", buf); }
}

void cmd_get_drivers() {
    for (int i = 0; i < driver_count; i++) {
        printf("PID %d: ", drivers[i].pid);
        cmd_get_status(drivers[i].pid);
    }
}


int main() {
    char line[BUF_SIZE];
    printf("Taxi CLI started. Commands: create_driver, send_task <pid> <time>, get_status <pid>, get_drivers\n");

    while (1) {
        printf("> "); fflush(stdout);
        if (!fgets(line, sizeof(line), stdin)) break;
        line[strcspn(line, "\n")] = 0;

        if (strcmp(line, "create_driver") == 0) {
            cmd_create_driver();
        } else if (strncmp(line, "send_task ", 10) == 0) {
            pid_t pid; int t;
            if (sscanf(line+10, "%d %d", &pid, &t) == 2)
                cmd_send_task(pid, t);
            else
                printf("Usage: send_task <pid> <task_timer>\n");
        } else if (strncmp(line, "get_status ", 11) == 0) {
            pid_t pid;
            if (sscanf(line+11, "%d", &pid) == 1)
                cmd_get_status(pid);
            else
                printf("Usage: get_status <pid>\n");
        } else if (strcmp(line, "get_drivers") == 0) {
            cmd_get_drivers();
        } else if (strcmp(line, "exit") == 0) {
            break;
        } else {
            printf("Unknown command\n");
        }
    }

    
    for (int i = 0; i < driver_count; i++) {
        kill(drivers[i].pid, SIGTERM);
        waitpid(drivers[i].pid, NULL, 0);
    }

    printf("CLI exited\n");
    return 0;
}
