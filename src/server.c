/**
 * This file is part of plugz
 * 
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>. 
 */


#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <pthread.h>

#include "../include/serv_zmq_layer.h"
#include "../include/serv_stdio_layer.h"
#include "../include/server.h"
#include "../include/plugz.h"
#include "../include/lock_util.h"

int s_interrupted = 0;

/**
 * I'll add some better handling later...
 * 
 * @param signal_value
 */
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

            pthread_t pt_zmq_server, pt_stdio_server;
            pthread_create(&pt_zmq_server, NULL, serv_init_zmq, NULL);
            pthread_create(&pt_stdio_server, NULL, serv_init_stdio, NULL);
            
            (void) pthread_join(pt_zmq_server, NULL);
            
            
            (void) pthread_join(pt_stdio_server, NULL);

        }
    }
}
