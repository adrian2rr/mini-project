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

#define BUFFER_SIZE 9
//static char rx_buffer[BUFFER_SIZE];

struct netif server_netif;

int conections;
int conections_a[8];

xQueueHandle quenet;
xQueueHandle quegam;

xQueueHandle conections_q[8];

xTaskHandle recev;
xTaskHandle serv[4];

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

	for(i=0;i<BUFFER_SIZE-1; i++){ tx_buffer[i] = '0'; }
	tx_buffer[BUFFER_SIZE-1] = '\0';
	tx_buffer[0] = c;

	if((nwrote = write(sd, tx_buffer, BUFFER_SIZE)) < 0)
		nwrote = 1;
	else
		nwrote = 0;

	return nwrote;
}

void resetQueue(xQueueHandle *q){
	vQueueDelete(*q);
	*q = xQueueCreate( 5, sizeof( struct MESS ) );
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

	subid = (int) rx_bufer[1];

	if (subid < 3){
		mess.id = 2;
		mess.subid = subid;
		xQueueSendToBack( quegam, &mess, ( portTickType ) 0 );
	}else{
		mess.id = 2;
		mess.subid = 3;
		mess.xb = (int)rx_bufer[2]*256 + (int)rx_bufer[3];
		mess.yb = (int)rx_bufer[4]*256 + (int)rx_bufer[5];
		mess.p = (int)rx_bufer[6]*256 + (int)rx_bufer[7];

		xQueueSendToBack( quegam, &mess, ( portTickType ) 0 );
	}

	return;
}

void clien_sen(int i){
	MESS mess;

	mess.id = 3;
	mess.subid = i;

	xQueueSendToBack( quegam, &mess, ( portTickType ) 0 );

	return;
}


int check_rx(int sd, char *rx_bufer){

	int fin;

	fin = 0;

	printf("\n recibido: %d %d %d %d %d",(int)rx_bufer[0],(int)rx_bufer[1],(int)rx_bufer[2]*256 + (int)rx_bufer[3],(int)rx_bufer[4]*256 + (int)rx_bufer[5],(int)rx_bufer[6]*256 + (int)rx_bufer[7]);

	switch((int)rx_bufer[0]){
	case 0:
		fin = send_ack(sd, 0);
		break;
	case 1:
		fin = contr_ini(sd, (int)rx_bufer[1]);
	case 2:
		contr_gam(rx_bufer);
		break;
	case 3:
		clien_sen((int)rx_bufer[1]);
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

	for(i=0;i<BUFFER_SIZE-1; i++){ tx_buffer[i] = '0'; }
	tx_buffer[BUFFER_SIZE-1] = '\0';
	tx_buffer[0] = mess.id;
	tx_buffer[1] = mess.subid;
	tx_buffer[2] = mess.xb/256;
	tx_buffer[3] = mess.xb%256;
	tx_buffer[4] = mess.yb/256;
	tx_buffer[5] = mess.yb%256;
	tx_buffer[6] =  mess.p/256;
	tx_buffer[7] = mess.p%256;

	printf("\n transmitido: %d %d %d %d %d",(int)tx_buffer[0],(int)tx_buffer[1],(int)tx_buffer[2]*256 + (int)tx_buffer[3],(int)tx_buffer[4]*256 + (int)tx_buffer[5],(int)tx_buffer[6]*256 + (int)tx_buffer[7]);

	if((nwrote = write(sd, tx_buffer, BUFFER_SIZE)) < 0)
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
					timeout = timeout - 5;
					if(timeout <= 0){
						printf("%s: error reading from socket %d, closing socket\r\n", __FUNCTION__, sock);
						fin = 1;
						break;
					}
					vTaskDelay( 5 / portTICK_RATE_MS );
		}else{

			timeout = 6000;
			fin = check_rx(sock, rx_buffer);

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

	//TODO CAMBIAR PUERTOS Y IPs
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

	while(recev != NULL);

	print("\nEXITING \n");

	return;
}

//======================= SERVER ==================================

xQueueHandle get_queser(int p){

	return conections_q[p];
}

void close_clien(int p){

	xQueueHandle queue_g;
	MESS mess;

	queue_g = conections_q[p*2+1];

	mess.id = 1;
	mess.subid = 2;
	mess.p = 0;
	mess.xb = 0;
	mess.yb = 0;

	xQueueSendToBack( queue_g, &mess, ( portTickType ) 0 );

	return;
}

void print_clien(){

	int i;

	for(i = 0;i < 3; i++){

		printf("\n - %d) Socket: %d - IP: %d",i+1,conections_a[i*2],conections_a[i*2+1]);
	}
	print("\n Choose one and select 1 to play, 0 to cancel\n Press 0 to exit\n");

	return;
}

void delete_connec(int sd){

	int i, c;

	c = 0;

	for(i = 0; i < 3; i++){

		if(conections_a[i*2] == sd){
			c = 1;
		}
		if ( c == 1 ){
			conections_a[i*2] = conections_a[(i+1)*2];
			conections_a[i*2+1] = conections_a[(i+1)*2+1];
			conections_q[i*2] = conections_q[(i+1)*2];
			conections_q[i*2+1] = conections_q[(i+1)*2+1];
		}
	}

	conections_a[6] = 0;
	conections_a[7] = 0;
	conections_q[6] = 0;
	conections_q[7] = 0;

	return;
}

int run_command(int sd, MESS mess){

	char tx_buffer[BUFFER_SIZE];
	int i, nwrote;

	for(i=0;i<BUFFER_SIZE-1; i++){ tx_buffer[i] = '0'; }
	tx_buffer[BUFFER_SIZE-1] = '\0';
	tx_buffer[0] = mess.id;
	tx_buffer[1] = mess.subid;
	tx_buffer[2] = mess.xb/256;
	tx_buffer[3] = mess.xb%256;
	tx_buffer[4] = mess.yb/256;
	tx_buffer[5] = mess.yb%256;
	tx_buffer[6] =  mess.p/256;
	tx_buffer[7] = mess.p%256;

	printf("\n transmitido: %d %d %d %d %d",(int)tx_buffer[0],(int)tx_buffer[1],(int)tx_buffer[2]*256 + (int)tx_buffer[3],(int)tx_buffer[4]*256 + (int)tx_buffer[5],(int)tx_buffer[6]*256 + (int)tx_buffer[7]);


	if((nwrote = write(sd, tx_buffer, BUFFER_SIZE)) < 0)
		nwrote = 1;
	else
		nwrote = 0;

	return nwrote;

}

int check_rx_se(int sd, char *rx_bufer, xQueueHandle gam){

	int fin;
	MESS mess;

	fin = 0;

	printf("\n recibido: %d %d %d %d %d",(int)rx_bufer[0],(int)rx_bufer[1],(int)rx_bufer[2]*256 + (int)rx_bufer[3],(int)rx_bufer[4]*256 + (int)rx_bufer[5],(int)rx_bufer[6]*256 + (int)rx_bufer[7]);

	switch((int)rx_bufer[0]){
	case 0:
		break;
	case 1:
		mess.id = 1;
		mess.subid = (int) rx_bufer[1];
		mess.xb = (int)rx_bufer[2]*256 + (int)rx_bufer[3];
		mess.yb = (int)rx_bufer[4]*256 + (int)rx_bufer[5];
		mess.p = (int)rx_bufer[6]*256 + (int)rx_bufer[7];
		xQueueSendToBack( gam, &mess, ( portTickType ) 0 );
		break;
	case 2:
		mess.id = 2;
		mess.subid = (int) rx_bufer[1];
		mess.xb = (int)rx_bufer[2]*256 + (int)rx_bufer[3];
		mess.yb = (int)rx_bufer[4]*256 + (int)rx_bufer[5];
		mess.p = (int)rx_bufer[6]*256 + (int)rx_bufer[7];
		xQueueSendToBack( gam, &mess, ( portTickType ) 0 );
		if(mess.subid == 2)
			fin = 1;
		break;
	case 3:
		mess.id = 3;
		mess.subid = (int) rx_bufer[1];
		mess.xb = (int)rx_bufer[2]*256 + (int)rx_bufer[3];
		mess.yb = (int)rx_bufer[4]*256 + (int)rx_bufer[5];
		mess.p = (int)rx_bufer[6]*256 + (int)rx_bufer[7];
		xQueueSendToBack( gam, &mess, ( portTickType ) 0 );
		break;
	default:
		fin = 1;
		break;
	}

	return fin;

}

void transmitter(void *p){

	int i, timeout, fin, finl, sd, timeout2;
	int pun = (int)p;
	char rx_buffer[BUFFER_SIZE];
	xQueueHandle queue_g, queue_n;
	MESS mess;

	queue_g = xQueueCreate( 5, sizeof( struct MESS ) );
	queue_n = xQueueCreate( 5, sizeof( struct MESS ) );

	conections_q[pun*2] = queue_g;
	conections_q[pun*2+1] = queue_n;

	sd = conections_a[pun*2];

	mess.id = 0;
	mess.subid = 0;
	mess.p = 0;
	mess.xb = 0;
	mess.yb = 0;

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
				if(rx_buffer[0] == 3 && rx_buffer[1]== 3){
					fin = 2;
				}
				finl = 1;
			}
		}

		if( xQueueReceive( queue_n, &( mess ), ( portTickType ) 10 ) ){

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
						fin = check_rx_se(sd, rx_buffer, queue_g);
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
			timeout = timeout - 5;
			if(timeout <= 0){
				printf("%s: error reading from socket %d, closing socket\r\n", __FUNCTION__, sd);
				fin = 1;
				break;
			}
			vTaskDelay( 5 / portTICK_RATE_MS );
		}else{

			timeout = 1000;
			fin = check_rx_se(sd, rx_buffer, queue_g);
		}

		if( xQueueReceive( queue_n, &( mess ), ( portTickType ) 10 ) ){
			timeout2 = 1500;
			if(mess.id == 3 && mess.subid == 2)
				fin = 1;

			run_command(sd, mess);

		}else{
			timeout2 = timeout2 - 5;
			if(timeout2 <= 0){
				mess.id = 0;
				run_command(sd, mess);
				timeout2 = 1500;
			}
		}

	}

	close(sd);
	delete_connec(sd);
	conections--;
	write_led(expon(2,conections)-1);
	vTaskDelete(NULL);
	return;
}

void server_tx_data() {
	int sock, new_sd;
	struct sockaddr_in address, remote;
	int size;

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
		if (new_sd > 0 && conections < 4){
			/* spawn a separate handler for each request */
			write_led(expon(2,conections+1)-1);

			printf(" -- New connection --\n Socket: %d\n Addr: %d",new_sd ,(int)remote.sin_addr.s_addr);

			conections_a[conections*2] = new_sd;
			conections_a[conections*2+1] = (int)remote.sin_addr.s_addr;

			sys_thread_new("n_client", (void(*)(void*))transmitter, (void*)conections, THREAD_STACKSIZE, DEFAULT_THREAD_PRIO);

			conections++;
		}
	}

	vTaskDelete(NULL);
	return;
}

//======================= MAIN ==================================

void network_main(){

	network_thread();

	quenet = xQueueCreate( 5, sizeof( struct MESS ) );
	quegam = xQueueCreate( 5, sizeof( struct MESS ) );

	sys_thread_new("server_thrd", (void(*)(void*))server_tx_data, 0, THREAD_STACKSIZE, DEFAULT_THREAD_PRIO);

	while(1){

	}


	vTaskDelete(NULL);

	return;
}
