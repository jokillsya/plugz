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
#include <glib.h>
#include <stdlib.h>

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
void s_signal_handler(int signal_value) {
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
            
            GThread *gt_zmq_server, *gt_stdio_server;
            
            char *zmq_msg = "ZMQ Server Thread";
            char *std_msg = "STDIO Server Thread";
            
            GError *zmq_err = NULL;
            GError *std_err = NULL;
            
            if(g_thread_supported()) {
                
                g_thread_init(NULL);
                
            } else {
                
                printf("SEVERE ERROR: GThreads not supported!");
                exit(-1);
                
            }
            
            if((gt_zmq_server = g_thread_create((GThreadFunc)serv_init_zmq, (void *) zmq_msg, TRUE, &zmq_err)) == NULL) {
                
                printf("SEVERE ERROR: Unable to initialize ZMQ Server.");
                exit(-1);
                
            }
            
            if((gt_stdio_server = g_thread_create((GThreadFunc)serv_init_stdio, (void *) std_msg, TRUE, &std_err)) == NULL) {
                
                printf("SEVERE ERROR: Unable to initialize STDIO Server.");
                exit(-1);
                
            }
            
            g_thread_join(gt_zmq_server);
            g_thread_join(gt_stdio_server);
            
            return;

        }
        
    }
    
}
