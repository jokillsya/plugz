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


#include <glib-2.0/glib.h>
#include <zmq.h>

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include <assert.h>
#include <signal.h>

#include "../include/serv_zmq_layer.h"
#include "../include/server.h"
#include "../include/plugz.h"
#include "../include/zhelpers.h"

extern int s_interrupted;

static gpointer worker_routine(void *context) {
    // Socket to talk to dispatcher
    gpointer receiver = zmq_socket(context, ZMQ_REP);
    zmq_connect(receiver, "inproc://workers");

    while (!s_interrupted) {

        char *string = s_recv(receiver);

        if ((string == NULL) || (*string = '\0')) {
            printf("W: interrupt received, killing server…\n");
            break;
        }

        printf("Received request: [%s]\n", string);
        free(string);
        // Do some 'work'
        sleep(1);
        // Send reply back to client
        s_send(receiver, "World");
    }

    printf("W: interrupt received, killing server…\n");

    zmq_close(receiver);
    return NULL;

}

gpointer serv_init_zmq(void) {
    
    GError *err = NULL;

    gpointer context = zmq_init(1);

    s_catch_signals();

    // Socket to talk to clients
    gpointer clients = zmq_socket(context, ZMQ_ROUTER);
    zmq_bind(clients, "tcp://*:5555");

    // Socket to talk to workers
    gpointer workers = zmq_socket(context, ZMQ_DEALER);
    zmq_bind(workers, "inproc://workers");

    GThreadPool *threadpool = g_thread_pool_new((GFunc)worker_routine, "STDIO Thread Pool", ZMQ_WORK_QUEUE_SIZE, FALSE, &err);
    // Launch pool of worker threads
    int thread_nbr;
    for (thread_nbr = 0; thread_nbr < ZMQ_WORK_QUEUE_SIZE; thread_nbr++) {
        
        g_thread_pool_push(threadpool, context, &err);

    }

    printf("plugz service started successfully...\n\n");

    // Connect work threads to client threads via a queue
    zmq_device(ZMQ_QUEUE, clients, workers);

    // We never get here but clean up anyhow
    zmq_close(clients);
    zmq_close(workers);
    zmq_term(context);
    
    //Never happens!
    return EXIT_SUCCESS;

}
