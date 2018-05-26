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

#include "config.h"
#include "utils.h"
#include "plot.h"

//===================== VARIABLES ===============================

struct sockaddr_in serv_addr;
static unsigned server_port = 10;

#define BUFFER_SIZE (20)
static char rx_buffer[BUFFER_SIZE];

//===================== FUNCIONES ===============================


void network_thread()
{
	struct netif *netif;
	struct ip_addr ipaddr, netmask, gw;
	/* the mac address of the board. this should be unique per board */
	unsigned char mac_ethernet_address[] = { 0x00, 0x0a, 0x35, 0x00, 0x01, 0x02 };

	netif = &server_netif;

	/* initialize IP addresses to be used */

	IP4_ADDR(&ipaddr,  192, 168, 1, 10);
	IP4_ADDR(&netmask, 255, 255, 255, 0);
	IP4_ADDR(&gw,      192, 168, 1, 20);

	/* print out IP settings of the board */
	print("\r\n\r\n");
	print("-----lwIP Socket Mode Demo Application ------\r\n");
	print_ip_settings(&ipaddr, &netmask, &gw);
	/* print all application headers */


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

//======================= CLIENT ==================================

void receiver() {
	int i, sock;
	char rx_buffer[BUFFER_SIZE];
	int ancho, largo, pos, margen;

	if ((sock = lwip_socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		printf("error creating socket\r\n");
		vTaskDelete(NULL);
		return;
	}

	if (lwip_connect(sock, (struct sockaddr *)&serv_addr, sizeof (serv_addr)) < 0) {
		printf("error in connect\r\n");
		close(sock);
		vTaskDelete(NULL);
		return;
	}
	print("Connected\n\r");

	while (1) {

		if ((i = read(sock, rx_buffer, BUFFER_SIZE)) < 0) {
			printf("%s: error reading from socket %d, closing socket\r\n", __FUNCTION__, sock);
			close(sock);
			break;
		}

		write_led(rx_buffer[0]);

		ancho = (int) rx_buffer[0];
		largo = (int) rx_buffer[1];
		pos = (int) rx_buffer[2];
		margen = (int) rx_buffer[3];

		//TODO SIN IMPLEMENTAR AUN
		//actualizarParamPala2(ancho, largo, pos, margen);
	}

}

void client_rx_data() {
	struct ip_addr servaddr;
	int serv_port;

	//TODO CAMBIAR PUERTOS Y IPs
	serv_port = server_port;

	/* Set here the host (PC) IP address */
	IP4_ADDR(&servaddr,  192, 168,   1, 10);

	memset((void*)&serv_addr, 0, sizeof serv_addr);
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(serv_port);
	serv_addr.sin_addr.s_addr = servaddr.addr;

	//print_ip("connect to  ", &servaddr);
	printf("... \n");

	sys_thread_new("receiver", (void(*)(void*))receiver, NULL,
			THREAD_STACKSIZE,
			DEFAULT_THREAD_PRIO);

	int switch0 = read_sw();
	int i = switch0%2; //[1 or 0]
	xTaskHandle handle;

	while(1){

		switch0 = read_sw();
		handle = xTaskGetHandle( "receiver" );

		//In case connection cannot be completed, conmute switch0 to retry
		if ( (handle == NULL) && (switch0%2 != i) ){
			int i = switch0%2; // To avoid going inside several times

			sys_thread_new("receiver", (void(*)(void*))receiver, NULL,
					THREAD_STACKSIZE,
					DEFAULT_THREAD_PRIO);
			vTaskDelay( psMS_TO_TICKS(100));
		}
	}
	vTaskDelete(NULL);
	return;
}

//======================= SERVER ==================================
void transmitter(void *p){
	int TX_NUM = 10;
	int i;
	int sd = (int)p;

	int* infoP = getPala1Pos();
	/* ancho	*(infoP)
	 * largo 	*(infoP+1)
	 * pos 		*(infoP+2)
	 * margen 	*(infoP+3)
	 */

	// int* infoB = getPelPos(); (Not implemented yet)

	char tx_buffer[BUFFER_SIZE];

	for(i=0;i<BUFFER_SIZE; i++){ tx_buffer[i] = '\0'; }

	while(1){
		int* infoP = getPala1Pos();
		tx_buffer[0] = (char) *(infoP+0); // Ancho Pala
		tx_buffer[1] = (char) *(infoP+1); // Largo Pala
		tx_buffer[2] = (char) *(infoP+2); // Pos Pala
		tx_buffer[3] = (char) *(infoP+3); // Margen Pala

		write(sd, tx_buffer, BUFFER_SIZE);

		// TODO QUE MAS ENVIAR? INFO PELOTA? MARCADOR? ESTADO JUEGO?
		// OTROS PARAMETROS (VELOCIDAD BOLA, ETC)?
		// ESPERANDO A LA IMPLEMENTACIÓN DE LA LÓGICA DEL JUEGO
		vTaskDelay(psMS_TO_TICKS(100));
	}
}

void server_tx_data() {
	int sock, new_sd;
	struct sockaddr_in address, remote;
	int size;

	if ((sock = lwip_socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		xil_printf("SERVER error creating socket\r\n");
		print("SERVER ahahfhosfuuh"+sock);
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
		if (new_sd > 0){
			/* spawn a separate handler for each request */
			sys_thread_new("receiver", (void(*)(void*))receiver(), (void*)new_sd,
					THREAD_STACKSIZE,
					DEFAULT_THREAD_PRIO);
		}
	}

}

//======================= MAIN ==================================

void network_main(){

	network_thread();

	sys_thread_new("client_thrd", (void(*)(void*))client_rx_data(), 0,
			THREAD_STACKSIZE,
			DEFAULT_THREAD_PRIO);

	sys_thread_new("server_thrd", (void(*)(void*))server_tx_data(), 0,
			THREAD_STACKSIZE,
			DEFAULT_THREAD_PRIO);
	return;
}
