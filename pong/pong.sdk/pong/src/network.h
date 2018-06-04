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


#endif /* NETWORK_H_ */
