#include <stdio.h>
#include <sys/socket.h>

#include "../include/serv_stdio_layer.h"

#define BUF_SZE 2048
#define HEADER_LEN 4
#define ARRAY_CONCAT(TYPE, A, An, B, Bn) \
  (TYPE *)array_concat((const void *)(A), (An), (const void *)(B), (Bn), sizeof(TYPE));

void *array_concat(const void *a, size_t an,
        const void *b, size_t bn, size_t s) {
    char *p = malloc(s * (an + bn));
    memcpy(p, a, an * s);
    memcpy(p + an*s, b, bn * s);
    return p;
}

int sendall(int s, char *buf, int *len, int prependHeader) {

    char *data;

    if (prependHeader) {

        char size_buf[HEADER_LEN];
        size_buf[0] = (char) (*len);
        size_buf[1] = (char) (*len >> 8);
        size_buf[2] = (char) (*len >> 16);
        size_buf[3] = (char) (*len >> 24);
        data = ARRAY_CONCAT(char, size_buf, 4, buf, *len);
        *len += 4;

    } else {

        data = buf;

    }

    int total = 0; // how many bytes we've sent
    int bytesleft = *len; // how many we have left to send
    int n = 0;

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

    int recv_cnt = -1;
    long byte_cnt = 0;

    uint32_t msg_size = 0;
    char header_bytes_read = 0;

    struct sock_data *head = NULL;
    struct sock_data *curr = NULL;

    do {
        char *buf = (char *) malloc(BUF_SZE);
        recv_cnt = recv(fd, buf, BUF_SZE, 0);
        if (recv_cnt > 0) {
            struct sock_data *node = (struct sock_data *) malloc(sizeof (struct sock_data));
            int header_i = 0;

            if (header_bytes_read < HEADER_LEN) {
                int i;
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

    int offset = 0;
    struct sock_data *next;

    for (curr = head; curr != NULL; curr = next) {
    
        next = curr->next;

        if (curr->start_i < curr->len) {
            int i;
            for (i = curr->start_i; i < curr->len; i++)
                (*data)[offset + (i - curr->start_i)] = (curr->buffer[i]);
            offset += (curr->len - curr->start_i);
        }

        free(curr->buffer);
        free(curr);
    
    }

    return byte_cnt;
}
