/*
 * network.c
 *
 *  Created on: May 10, 2018
 *      Author: RdelaRosa
 *      Class that connects both clients to their respective server via ethernet port
 *      Once connection is established:
 *      	Server will send periodically data about paddle position, ball position (MAYBE NOT NEEDED)
 *      	Client will receive the data and invoke the function to update and print on-screen info
 *
 */
#include "xparameters.h"
#include "xgpio.h"

#include "portmacro.h"
#include "platform_config.h"
#include <stdlib.h>
#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"
#include "lwipopts.h"
#include "lwip/inet.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "netif/xadapter.h"
#include "queue.h"

#include "config.h"
#include "utils.h"
#include "plot.h"

//===================== VARIABLES ===============================

struct sockaddr_in serv_addr;
static unsigned server_port = 8080;

#define BUFFER_SIZE 11
//static char rx_buffer[BUFFER_SIZE];

struct netif server_netif;

int conections;
int conections_s;
//u32_t conections_a;

xQueueHandle quenet;
xQueueHandle quegam;

xTaskHandle recev;

int mod;

//===================== FUNCIONES ===============================

void game_thread();

void network_thread()
{
	struct netif *netif;
	struct ip_addr ipaddr, netmask, gw;
	/* the mac address of the board. this should be unique per board */
	unsigned char mac_ethernet_address[] = { 0x00, 0x0a, 0x35, 0x00, 0x01, 0x02 };

	netif = &server_netif;

	/* initialize IP addresses to be used */

	IP4_ADDR(&ipaddr,  192, 168, 1, 11);
	IP4_ADDR(&netmask, 255, 255, 255, 0);
	IP4_ADDR(&gw,      192, 168, 1, 20);

	/* print out IP settings of the board
	print("\r\n\r\n");
	print("-----lwIP Socket Mode Demo Application ------\r\n");
	print_ip_settings(&ipaddr, &netmask, &gw);
	 print all application headers */


	/* Add network interface to the netif_list, and set it as default */
	if (!xemac_add(netif, &ipaddr, &netmask, &gw, mac_ethernet_address, PLATFORM_EMAC_BASEADDR)) {
		xil_printf("Error adding N/W interface\r\n");
		return;
	}

	netif_set_default(netif);
	/* specify that the network if is up */
	netif_set_up(netif);

	/* start packet receive thread - required for lwIP operation */
	sys_thread_new("xemacif_input_thread", (void(*)(void*))xemacif_input_thread, netif,
			THREAD_STACKSIZE,
			DEFAULT_THREAD_PRIO);
	return;
}

int send_ack(int sd, int c){

	int nwrote, i;
	char tx_buffer[BUFFER_SIZE];

	for(i=0;i<BUFFER_SIZE-1; i++){ tx_buffer[i] = 0; }
	tx_buffer[BUFFER_SIZE-1] = '\0';
	tx_buffer[0] = c;

	if((nwrote = lwip_send(sd, tx_buffer, BUFFER_SIZE, MSG_DONTWAIT)) < 0)
		nwrote = 1;
	else
		nwrote = 0;

	return nwrote;
}

void resetQueue(xQueueHandle *q){
	vQueueDelete(*q);
	*q = xQueueCreate( 20, sizeof( struct MESS ) );
}

xQueueHandle getqueue(int n){

	if(n == 0){

		return quenet;

	}else{

		return quegam;
	}

}

//======================= CLIENT ==================================

int contr_ini(int sd, int c){

	if(c == 1){

		mod = 2;

		sys_thread_new("local_game", (void(*)(void*))game_thread, &mod, THREAD_STACKSIZE, DEFAULT_THREAD_PRIO);

		return send_ack(sd, c);

	}else if(c == 2){

		return 1;
	}
	return 1;
}

void contr_gam(char *rx_bufer){

	MESS mess;
	int subid;

	subid = rx_bufer[1];

	if (subid < 3){
		mess.id = 2;
		mess.subid = subid;
		xQueueSendToBack( quegam, &mess, ( portTickType ) 10 );
	}else{
		mess.id = 2;
		mess.subid = 3;
		mess.xb = rx_bufer[2]*256 + rx_bufer[3];
		mess.yb = rx_bufer[4]*256 + rx_bufer[5];
		mess.p = rx_bufer[6]*256 + rx_bufer[7];
		mess.l = rx_bufer[8];
		mess.v = rx_bufer[9];

		xQueueSendToBack( quegam, &mess, ( portTickType ) 10 );
	}

	return;
}

int clien_sen(int i){
	MESS mess;

	mess.id = 3;
	mess.subid = i;

	xQueueSendToBack( quegam, &mess, ( portTickType ) 10 );

	if (mess.subid == 2){
		return 1;
	}else{
		return 0;
	}
}


int check_rx(int sd, char *rx_bufer){

	int fin;

	fin = 0;

	//printf("\n recibido: %d %d %d %d %d %d %d",rx_bufer[0],rx_bufer[1],rx_bufer[2]*256 + rx_bufer[3],rx_bufer[4]*256 + rx_bufer[5],rx_bufer[6]*256 + rx_bufer[7],rx_bufer[8],rx_bufer[9]);

	switch(rx_bufer[0]){
	case 0:
		fin = send_ack(sd, 0);
		break;
	case 1:
		fin = contr_ini(sd, rx_bufer[1]);
	case 2:
		contr_gam(rx_bufer);
		break;
	case 3:
		fin = clien_sen(rx_bufer[1]);
		break;
	default:
		fin = 1;
		break;
	}

	return fin;
}

int send_mss(int sd, MESS mess){

	int nwrote, i;
	char tx_buffer[BUFFER_SIZE];

	for(i=0;i<BUFFER_SIZE-1; i++){ tx_buffer[i] = 0; }
	tx_buffer[BUFFER_SIZE-1] = '\0';
	tx_buffer[0] = mess.id;
	tx_buffer[1] = mess.subid;
	tx_buffer[2] = mess.xb/256;
	tx_buffer[3] = mess.xb%256;
	tx_buffer[4] = mess.yb/256;
	tx_buffer[5] = mess.yb%256;
	tx_buffer[6] =  mess.p/256;
	tx_buffer[7] = mess.p%256;
	tx_buffer[8] = mess.l;
	tx_buffer[9] = mess.v;

	//printf("\n transmitido: %d %d %d %d %d %d %d",tx_buffer[0],tx_buffer[1],tx_buffer[2]*256 + tx_buffer[3],tx_buffer[4]*256 + tx_buffer[5],tx_buffer[6]*256 + tx_buffer[7],tx_buffer[8],tx_buffer[9]);

	if((nwrote = lwip_send(sd, tx_buffer, BUFFER_SIZE, MSG_DONTWAIT)) < 0)
		nwrote = 1;
	else
		nwrote = 0;

	return nwrote;
}

void receiver() {
	int i, sock;
	char rx_buffer[BUFFER_SIZE];
	int fin, timeout;
	MESS mess;

	timeout = 6000;
	fin = 0;

	if ((sock = lwip_socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		printf("error creating socket\r\n");
		recev = NULL;
		vTaskDelete(NULL);
		return;
	}

	if (lwip_connect(sock, (struct sockaddr *)&serv_addr, sizeof (serv_addr)) < 0) {
		printf("error in connect\r\n");
		close(sock);
		recev = NULL;
		vTaskDelete(NULL);
		return;
	}
	print("Connected\n\r");

	resetQueue(&quenet);
	resetQueue(&quegam);

	while (fin == 0) {

		if ((i = recv(sock, rx_buffer, BUFFER_SIZE, MSG_DONTWAIT)) < 0) {
					timeout = timeout - 1;
					if(timeout <= 0){
						printf("%s: error reading from socket %d, closing socket\r\n", __FUNCTION__, sock);
						fin = 1;
						break;
					}
					vTaskDelay( 1 / portTICK_RATE_MS );
		}else{

			timeout = 6000;
			fin = check_rx(sock, rx_buffer);
			//printf("\n %d",fin);
		}

		if( xQueueReceive( quenet, &( mess ), ( portTickType ) 10 ) ){

			if(mess.id < 2 || (mess.id == 2 && mess.subid < 3)){
				fin = send_ack(sock, mess.id);
			}else{
				fin = send_mss(sock, mess);
			}

		}

	}

	close(sock);
	recev = NULL;
	vTaskDelete(NULL);
	return;

}

void client_rx_data() {
	struct ip_addr servaddr;
	int serv_port, ips[4];

	serv_port = server_port;

	int ok;
	char an[3];

	ok = 0;

	do{
		print("Enter IP of server term by term: ");

		scanf("%d", &ips[0]);
		scanf("%d", &ips[1]);
		scanf("%d", &ips[2]);
		scanf("%d", &ips[3]);

		printf("\n - Is the IP correct? %d.%d.%d.%d \n[yes - no]: ", ips[0], ips[1], ips[2], ips[3]);

		scanf("%s", an);

		if(strcmp(an,"yes") == 0){
			ok = 1;
		}

	}while(ok == 0);

	/* Set here the host (PC) IP address */
	IP4_ADDR(&servaddr, ips[0], ips[1], ips[2], ips[3]);

	memset((void*)&serv_addr, 0, sizeof serv_addr);
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(serv_port);
	serv_addr.sin_addr.s_addr = servaddr.addr;

	//print_ip("connect to  ", &servaddr);
	//printf("... \n");

	recev = sys_thread_new("receiver", (void(*)(void*))receiver, NULL, THREAD_STACKSIZE, DEFAULT_THREAD_PRIO);

	while(recev != NULL){
		vTaskDelay( 1000 / portTICK_RATE_MS );
	}

	print("\nEXITING \n");

	return;
}

//======================= SERVER ==================================

void con_clien(int p){

	MESS mess;

	mess.id = 1;
	mess.subid = p;
	mess.p = 0;
	mess.xb = 0;
	mess.yb = 0;
	mess.l = 0;
	mess.v = 0;

	xQueueSendToBack( quenet, &mess, ( portTickType ) 10 );

	return;
}

int run_command(int sd, MESS mess){

	char tx_buffer[BUFFER_SIZE];
	int i, nwrote;

	for(i=0;i<BUFFER_SIZE-1; i++){ tx_buffer[i] = 0; }
	tx_buffer[BUFFER_SIZE-1] = '\0';
	tx_buffer[0] = mess.id;
	tx_buffer[1] = mess.subid;
	tx_buffer[2] = mess.xb/256;
	tx_buffer[3] = mess.xb%256;
	tx_buffer[4] = mess.yb/256;
	tx_buffer[5] = mess.yb%256;
	tx_buffer[6] = mess.p/256;
	tx_buffer[7] = mess.p%256;
	tx_buffer[8] = mess.l;
	tx_buffer[9] = mess.v;

	//printf("\n transmitido: %d %d %d %d %d %d %d",tx_buffer[0],tx_buffer[1],tx_buffer[2]*256 + tx_buffer[3],tx_buffer[4]*256 + tx_buffer[5],tx_buffer[6]*256 + tx_buffer[7],tx_buffer[8],tx_buffer[9]);


	if((nwrote = lwip_send(sd, tx_buffer, BUFFER_SIZE, MSG_DONTWAIT)) < 0)
		nwrote = 1;
	else
		nwrote = 0;

	return nwrote;

}

int check_rx_se(int sd, char *rx_bufer, xQueueHandle gam){

	int fin;
	MESS mess;

	fin = 0;

	//printf("\n recibido: %d %d %d %d %d %d %d",rx_bufer[0],rx_bufer[1],rx_bufer[2]*256 + rx_bufer[3],rx_bufer[4]*256 + rx_bufer[5],rx_bufer[6]*256 + rx_bufer[7],rx_bufer[8],rx_bufer[9]);

	switch(rx_bufer[0]){
	case 0:
		break;
	case 1:
		mess.id = 1;
		mess.subid = rx_bufer[1];
		mess.xb = rx_bufer[2]*256 + rx_bufer[3];
		mess.yb = rx_bufer[4]*256 + rx_bufer[5];
		mess.p = rx_bufer[6]*256 + rx_bufer[7];
		mess.l = rx_bufer[8];
		mess.v = rx_bufer[9];
		xQueueSendToBack( gam, &mess, ( portTickType ) 10 );
		break;
	case 2:
		mess.id = 2;
		mess.subid = rx_bufer[1];
		mess.xb = rx_bufer[2]*256 + rx_bufer[3];
		mess.yb = rx_bufer[4]*256 + rx_bufer[5];
		mess.p = rx_bufer[6]*256 + rx_bufer[7];
		mess.l = rx_bufer[8];
		mess.v = rx_bufer[9];
		xQueueSendToBack( gam, &mess, ( portTickType ) 10 );
		if(mess.subid == 2)
			fin = 1;
		break;
	case 3:
		mess.id = 3;
		mess.subid = rx_bufer[1];
		mess.xb = rx_bufer[2]*256 + rx_bufer[3];
		mess.yb = rx_bufer[4]*256 + rx_bufer[5];
		mess.p = rx_bufer[6]*256 + rx_bufer[7];
		mess.l = rx_bufer[8];
		mess.v = rx_bufer[9];
		xQueueSendToBack( gam, &mess, ( portTickType ) 10 );
		break;
	default:
		fin = 1;
		break;
	}

	return fin;

}

void transmitter(){

	int i, timeout, fin, finl, sd, timeout2;
	char rx_buffer[BUFFER_SIZE];
	MESS mess;

	resetQueue(&quenet);
	resetQueue(&quegam);

	sd = conections_s;

	mess.id = 0;
	mess.subid = 0;
	mess.p = 0;
	mess.xb = 0;
	mess.yb = 0;
	mess.l = 0;
	mess.v = 0;

	fin = 0;

	while(fin == 0){

		vTaskDelay( 1000 / portTICK_RATE_MS );

		timeout = 500;

		run_command(sd, mess);

		while( finl == 0 ){
			if ((i = recv(sd, rx_buffer, BUFFER_SIZE, MSG_DONTWAIT)) < 0) {
				timeout = timeout - 10;
				if(timeout <= 0){
					printf("%s: error reading from socket %d, closing socket\r\n", __FUNCTION__, sd);
					fin = 1;
					finl = 1;
					break;
				}
				vTaskDelay( 10 / portTICK_RATE_MS );
			}else{
				if(rx_buffer[0] == 3 && rx_buffer[1] == 3){
					fin = 2;
				}
				finl = 1;
			}
		}

		if( xQueueReceive( quenet, &( mess ), ( portTickType ) 10 ) ){

			run_command(sd, mess);

			if(mess.subid == 1){
				fin = 1;
			}else{
				fin = 2;
			}

		}
	}

	if ( fin == 1 ){
		timeout = 500;

		finl = 0;
		fin = 0;

		while( finl == 0 ){
					if ((i = recv(sd, rx_buffer, BUFFER_SIZE, MSG_DONTWAIT)) < 0) {
						timeout = timeout - 10;
						if(timeout <= 0){
							printf("%s: error reading from socket %d, closing socket\r\n", __FUNCTION__, sd);
							fin = 1;
							finl = 1;
							break;
						}
						vTaskDelay( 10 / portTICK_RATE_MS );
					}else{
						fin = check_rx_se(sd, rx_buffer, quegam);
						if(rx_buffer[0] == 3 && rx_buffer[1]== 3){
							fin = 2;
						}
						finl = 1;
					}
				}

	}

	timeout = 1000;

	while(fin == 0){

		if ((i = recv(sd, rx_buffer, BUFFER_SIZE, MSG_DONTWAIT)) < 0) {
			timeout = timeout - 1;
			if(timeout <= 0){
				printf("%s: error reading from socket %d, closing socket\r\n", __FUNCTION__, sd);
				fin = 1;
				break;
			}
			vTaskDelay( 1 / portTICK_RATE_MS );
		}else{

			timeout = 1000;
			fin = check_rx_se(sd, rx_buffer, quegam);
		}

		if( xQueueReceive( quenet, &( mess ), ( portTickType ) 10 ) ){
			timeout2 = 1500;
			if(mess.id == 3 && mess.subid == 2)
				fin = 1;

			run_command(sd, mess);

		}else{
			timeout2 = timeout2 - 1;
			if(timeout2 <= 0){
				mess.id = 0;
				run_command(sd, mess);
				timeout2 = 1500;
			}
		}

	}

	close(sd);
	conections--;
	write_led(expon(2,conections)-1);
	vTaskDelete(NULL);
	return;
}

void server_tx_data() {
	int sock, new_sd;
	struct sockaddr_in address, remote;
	int size;

	network_thread();

	quenet = xQueueCreate( 5, sizeof( struct MESS ) );
	quegam = xQueueCreate( 5, sizeof( struct MESS ) );

	conections = 0;

	if ((sock = lwip_socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		xil_printf("SERVER error creating socket\r\n");
		lwip_close(sock);
		vTaskDelete(NULL);
		return;
	}

	//Bind to port  *server_port*
	address.sin_family = AF_INET;
	address.sin_port = htons(server_port);
	address.sin_addr.s_addr = INADDR_ANY;

	if (lwip_bind(sock, (struct sockaddr *)&address, sizeof (address)) < 0) {
		vTaskDelete(NULL);
		return;
	}

	/* listen for incoming connections */
	lwip_listen(sock, 0);
	size = sizeof(remote);

	while (1) {
		new_sd = lwip_accept(sock, (struct sockaddr *)&remote, (socklen_t *)&size);
		if (new_sd > 0 && conections < 1){
			/* spawn a separate handler for each request */
			//write_led(expon(2,conections+1)-1);

			printf(" -- New connection --\n Socket: %d -- Addr: %d",new_sd ,(int)remote.sin_addr.s_addr);

			conections_s = new_sd;

			sys_thread_new("n_client", (void(*)(void*))transmitter, 0, THREAD_STACKSIZE, DEFAULT_THREAD_PRIO);

			conections++;
		}else if(new_sd > 0){

		}
	}

	vTaskDelete(NULL);
	return;
}

