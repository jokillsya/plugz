/* 
 * File:   serv_stdio_layer.h
 * Author: johnathan
 *
 * Created on 16 April 2012, 11:05 PM
 */

#ifndef SERV_STDIO_LAYER_H
#define	SERV_STDIO_LAYER_H

struct sock_data {

    char *buffer;
    int start_i;
    int len;
    struct sock_data *next;

};

#endif	/* SERV_STDIO_LAYER_H */

