
#include "../include/zhelpers.h"
#include "../include/plugz.h"
#include "../include/lock_util.h"
#include "../include/server.h"

#include <pthread.h>
#include <zmq.h>
#include <unistd.h>
#include <signal.h>
 
static void s_signal_handler(P_INT signal_value) {
    s_interrupted = 1;
}
 
static void s_catch_signals(void) {
    struct sigaction action;
    action.sa_handler = s_signal_handler;
    action.sa_flags = 0;
    sigemptyset(&action.sa_mask);
    sigaction(SIGINT, &action, NULL);
    sigaction(SIGTERM, &action, NULL);
    sigaction(SIGKILL, &action, NULL);
}
 

static void * worker_routine(void *context) {
    // Socket to talk to dispatcher
    void *receiver = zmq_socket(context, ZMQ_REP);
    zmq_connect(receiver, "inproc://workers");
 
    while (!s_interrupted) {
 
        P_CHAR *string = s_recv(receiver);
       
        if((string == NULL) || (*string = '\0')) {
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
            
            void *context = zmq_init(1);
 
            s_catch_signals();
 
            // Socket to talk to clients
            void *clients = zmq_socket(context, ZMQ_ROUTER);
            zmq_bind(clients, "tcp://*:5555");
 
            // Socket to talk to workers
            void *workers = zmq_socket(context, ZMQ_DEALER);
            zmq_bind(workers, "inproc://workers");
 
            // Launch pool of worker threads
            int thread_nbr;
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
    }
}
