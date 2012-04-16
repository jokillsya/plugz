
#include <unistd.h>
#include <signal.h>
#include <stdio.h>

#include "../include/serv_zmq_layer.h"
#include "../include/server.h"
#include "../include/plugz.h"
#include "../include/lock_util.h"

int s_interrupted = 0;
 
void s_signal_handler(P_INT signal_value) {
    s_interrupted = 1;
}
 
void s_catch_signals(void) {
    struct sigaction action;
    action.sa_handler = s_signal_handler;
    action.sa_flags = 0;
    sigemptyset(&action.sa_mask);
    sigaction(SIGINT, &action, NULL);
    sigaction(SIGTERM, &action, NULL);
    sigaction(SIGKILL, &action, NULL);
}

void stop_server() {
 
    pid_t pid = get_locking_pid();
 
    if (pid == 0) {
 
        printf("server not running.\n");
 
    } else {
 
        kill(pid, SIGKILL);
        printf("plugz killed.\n\n");
 
    }
}

void start_server() {
 
    pid_t pid;
    pid = fork();
 
    if (pid == 0) {
 
        if (!lock(pid)) {
 
            pid_t lpid = get_locking_pid();
 
            printf("plugz service already running on pid: %d\n", lpid);
 
        } else {
            
            serv_init_zmq();
 
        }
    }
}
