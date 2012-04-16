#include <pthread.h>
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

static void * worker_routine(void *context) {
    // Socket to talk to dispatcher
    void *receiver = zmq_socket(context, ZMQ_REP);
    zmq_connect(receiver, "inproc://workers");

    while (!s_interrupted) {

        P_STRING string = s_recv(receiver);

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

void serv_init_zmq() {

    void *context = zmq_init(1);

    s_catch_signals();

    // Socket to talk to clients
    void *clients = zmq_socket(context, ZMQ_ROUTER);
    zmq_bind(clients, "tcp://*:5555");

    // Socket to talk to workers
    void *workers = zmq_socket(context, ZMQ_DEALER);
    zmq_bind(workers, "inproc://workers");

    // Launch pool of worker threads
    P_INT thread_nbr;
    for (thread_nbr = 0; thread_nbr < 5; thread_nbr++) {
        pthread_t worker;
        pthread_create(&worker, NULL, worker_routine, context);
    }

    printf("plugz service started successfully...\n\n");

    // Connect work threads to client threads via a queue
    zmq_device(ZMQ_QUEUE, clients, workers);

    // We never get here but clean up anyhow
    zmq_close(clients);
    zmq_close(workers);
    zmq_term(context);

}
