/*
 * network.h
 *
 *  Created on: May 10, 2018
 *      Author: adrianrodriguezrodri
 */

#ifndef NETWORK_H_
#define NETWORK_H_

extern void network_thread();

extern void receiver();

extern void client_rx_data();

extern void transmitter(void *p);

extern void server_tx_data();

extern void network_main();

extern int run_command(MESS mess);

#define BUFFER_SIZE 11

extern int get_sd();

extern int get_sd_c();

extern MESS check_rx_se(char *rx_bufer);

extern MESS check_rx(char *rx_bufer);

extern int send_mss(MESS mess);

#endif /* NETWORK_H_ */
