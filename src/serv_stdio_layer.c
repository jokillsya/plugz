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


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <limits.h>
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
//#define HEADER_LEN 4 ->> Don't need this due to new message format...
#define ARRAY_CONCAT(TYPE, A, An, B, Bn) \
  (TYPE *)array_concat((const void *)(A), (An), (const void *)(B), (Bn), sizeof(TYPE));

void *array_concat(const void *a, size_t an,
        const void *b, size_t bn, size_t s) {
    P_CHAR *p = malloc(s * (an + bn));
    memcpy(p, a, an * s);
    memcpy(p + an*s, b, bn * s);
    return p;
}

int sendall(P_INT s, P_STRING buf, P_INT *len, P_INT prependHeader, P_INT hd_len) {

    char *data;

    if (prependHeader) {

        P_CHAR size_buf[hd_len];
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

P_INT std_sock_recv_max(P_INT *fd, char **data, uint32_t max) {

    *data = malloc((size_t)max);

    P_INT i = 0, r = 0;

    do {
        
        r = recv(*fd, data[i], max, 0);
        
        if(r > 0) {
            
            i += r;
            
        } else {
            
            break;
            
        }

    } while (i <= max);

    return i;
    
}

/**
 * This function still needs to be enhanced to hook the module socket
 * up to the *fd in a buf by buf stream of data -- otherwise memory
 * would be a bit of a gamble...
 * 
 */
P_INT std_sock_recv(P_INT *fd, char **data, P_INT hd_len) {
    
    P_STRING h_buf;
    //Still need to chunk this bit...
    P_INT recv_cnt = std_sock_recv_max(fd, &h_buf, hd_len);

    if (recv_cnt == hd_len) {

        P_INT i;
        uint32_t msg_size = 0;
        
        for (i = 0; i < hd_len; i++) {
            
            msg_size += (h_buf[i] << (i * 8));
            
        }
        
        free(h_buf);
        
        return std_sock_recv_max(fd, data, msg_size);

    }
    
    return -1;
    
}

void std_sock_worker(P_INT *fd) {
    
    P_STRING hd_data;
    P_STRING data;
    
    //This is all very theoretical - but here we go...
    
    /**
     * Ok - the first 9 bytes consist of an 8 byte module code, followed
     * by a 1 byte pre-header bitmap, that bitmap will be divided into
     * 2 chunks... the first of these nibbles will define the length
     * of the header(starting at byte 10) describing the length of the data.
     * 
     * That way we can handle up to 8192 petabytes as the message length.
     * 
     * So - pull 9 bytes...
     * 
     */
    P_INT bytes = std_sock_recv_max(fd, &hd_data, 9);
    
    //TODO: Check length errors...
    
    //Get the pre-header bitmap, 9'th byte...
    P_CHAR bitmap = hd_data[strlen(hd_data) - 1];
    
    /*
     * Here be dragons!
     * 
     * Extract the length of the header starting at the 10'th byte
     * We use the first 3 bits to determine the length...
     * 
     * so 4 + 2 + 1 is max value (7) -- we add one to give us a max of
     * 8 (sizeof(long long)) --> and ignore 0
     */
    P_INT hd_len = BIT_EXTR(0, 3, bitmap) + 1;
    
    //Copy plug code out of header...
    P_CHAR plug_t[8]; 
    strncpy(&plug_t[0], hd_data, 8);
    
    free(hd_data);
    
    P_LLONG d_bytes = std_sock_recv(fd, &data, hd_len);
    
    printf("Data: %s\n", data);
    free(data);
    
    //TODO: Check length errors...
    
    
    
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

        listen(sockfd, 1000);
        
        addr_size = sizeof their_addr;

        //Block...
        P_INT new_fd = accept(sockfd, (struct sockaddr *) &their_addr, &addr_size);

        /**
         * Add a fd to the work queue - worker needs to handle it... 
         */
        thpool_add_work(threadpool, (void*) std_sock_worker, (void*) &new_fd);

    }
    
    printf("HOW THE FUCK!!!!!\n");

}

void *serv_init_stdio(void) {

    //Listen for client connections...
    std_sock_listen();
    
    //Never returns!
    return EXIT_SUCCESS;
}
