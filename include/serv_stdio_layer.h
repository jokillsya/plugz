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

#define STD_PORT                "3456"
#define STD_WORK_QUEUE_SIZE     100

void *serv_init_stdio();

#endif	/* SERV_STDIO_LAYER_H */

