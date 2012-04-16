/* 
 * File:   server.h
 * Author: johnathan
 *
 * Created on 31 March 2012, 1:22 PM
 */

#ifndef SERVER_H
#define	SERVER_H

static int s_interrupted = 0;

static void s_catch_signals(void);

void start_server();
void stop_server();

#endif	/* SERVER_H */

