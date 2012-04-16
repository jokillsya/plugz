#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

#include "../include/plugz.h"
#include "../include/serv_stdio_layer.h"
#include "../include/thpool.h"

#define BUF_SZE 2048
#define HEADER_LEN 4
#define ARRAY_CONCAT(TYPE, A, An, B, Bn) \
  (TYPE *)array_concat((const void *)(A), (An), (const void *)(B), (Bn), sizeof(TYPE));

void *array_concat(const void *a, size_t an,
        const void *b, size_t bn, size_t s) {
    P_CHAR *p = malloc(s * (an + bn));
    memcpy(p, a, an * s);
    memcpy(p + an*s, b, bn * s);
    return p;
}

int sendall(P_INT s, P_STRING buf, P_INT *len, P_INT prependHeader) {

    char *data;

    if (prependHeader) {

        P_CHAR size_buf[HEADER_LEN];
        size_buf[0] = (char) (*len);
        size_buf[1] = (char) (*len >> 8);
        size_buf[2] = (char) (*len >> 16);
        size_buf[3] = (char) (*len >> 24);
        data = ARRAY_CONCAT(P_CHAR, size_buf, 4, buf, *len);
        *len += 4;

    } else {

        data = buf;

    }

    P_INT total = 0; // how many bytes we've sent
    P_INT bytesleft = *len; // how many we have left to send
    P_INT n = 0;

    while (total < *len) {

        n = send(s, data + total, bytesleft, 0);

        if (n == -1) {
            break;
        }

        total += n;
        bytesleft -= n;
    }

    *len = total; // return number actually sent here

    if (prependHeader) free(data);

    return n == -1 ? -1 : 0; // return -1 on failure, 0 on success
}

int assemble_stdio_data(int fd, char **data) {

    P_INT recv_cnt = -1;
    P_LONG byte_cnt = 0;

    uint32_t msg_size = 0;
    P_CHAR header_bytes_read = 0;

    struct sock_data *head = NULL;
    struct sock_data *curr = NULL;

    do {
        P_STRING buf = (P_STRING) malloc(BUF_SZE);
        recv_cnt = recv(fd, buf, BUF_SZE, 0);
        if (recv_cnt > 0) {
            struct sock_data *node = (struct sock_data *) malloc(sizeof (struct sock_data));
            P_INT header_i = 0;

            if (header_bytes_read < HEADER_LEN) {
                P_INT i;
                for (i = 0; i < HEADER_LEN; i++) {
                    if (header_bytes_read == i && header_i < recv_cnt) {
                        msg_size += (buf[header_i] << (i * 8));
                        header_i++;
                        header_bytes_read++;
                    }
                }
            }

            node->buffer = buf;
            node->start_i = header_i;
            node->len = recv_cnt;
            node->next = NULL;

            if (curr == NULL) {
                head = curr = node;
            } else {
                curr->next = node;
                curr = node;
            }

            byte_cnt += recv_cnt;

        } else {

            free(buf);
            break;

        }

    } while (header_bytes_read < HEADER_LEN || byte_cnt < (msg_size + HEADER_LEN));

    if (byte_cnt < HEADER_LEN) {

        *data = malloc(0);
        return 0;

    }

    byte_cnt -= HEADER_LEN;
    *data = malloc(byte_cnt);

    P_INT offset = 0;
    struct sock_data *next;

    for (curr = head; curr != NULL; curr = next) {

        next = curr->next;

        if (curr->start_i < curr->len) {
            P_INT i;
            for (i = curr->start_i; i < curr->len; i++)
                (*data)[offset + (i - curr->start_i)] = (curr->buffer[i]);
            offset += (curr->len - curr->start_i);
        }

        free(curr->buffer);
        free(curr);

    }

    return byte_cnt;
}

void std_sock_worker(P_INT *fd) {
    
    /**
     * Just close the FD for now - till I can figure out the DB stuff...
     */
    close(*fd);

}

void *std_sock_listen() {

    struct sockaddr_storage their_addr;
    socklen_t addr_size;
    struct addrinfo hints;
    struct addrinfo *res;
    P_INT sockfd;

    // first, load up address structs with getaddrinfo():

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC; // use IPv4 or IPv6, whichever
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // fill in my IP for me

    getaddrinfo(NULL, STD_PORT, &hints, &res);

    // make a socket, bind it, and listen on it:

    sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    bind(sockfd, res->ai_addr, res->ai_addrlen);

    thpool_t* threadpool;
    threadpool = thpool_init(STD_WORK_QUEUE_SIZE);

    while (1) {

        addr_size = sizeof their_addr;

        //Block...
        P_INT new_fd = accept(sockfd, (struct sockaddr *) &their_addr, &addr_size);

        /**
         * Add a fd to the work queue - worker needs to handle it... 
         */
        thpool_add_work(threadpool, (void*) std_sock_worker, (void*) &new_fd);

    }

}

void serv_init_stdio(void) {

    //Listen for client connections...
    std_sock_listen();

}
