/*
 * client.c
 *
 *  Created on: Apr 13, 2018
 *      Author: adrianrodriguezrodri
 */


#include <stdio.h>
#include <string.h>
#include "lwip/inet.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwipopts.h"

#include "config_apps.h"
#include "FreeRTOS.h"
#include "task.h"
#include "xil_printf.h"
#include "xgpio.h"

#include "utils.h"

//====================================================

struct sockaddr_in serv_addr;
xTaskHandle handle;

//====================================================

void receiver()
{

	int i, sock;
	int BUF_SIZE = 20;
	char rx_buf[BUF_SIZE];

	if ((sock = lwip_socket(AF_INET, SOCK_STREAM, 0)) < 0) {
				printf("error creating socket\r\n");
				handle = NULL;
				vTaskDelete(NULL);
				return;
			}

	if (lwip_connect(sock, (struct sockaddr *)&serv_addr, sizeof (serv_addr)) < 0) {
				printf("error in connect\r\n");
				close(sock);
				handle = NULL;
				vTaskDelete(NULL);
				return;
	}
	print("Connected\n\r");

	int timeout = 10000;
	int nwrote;

	while (1) {

			if ((i = recv(sock, rx_buf, BUF_SIZE, MSG_DONTWAIT)) < 0) {
						timeout = timeout - 10;
						if(timeout <= 0){
							printf("%s: error reading from socket %d, closing socket\r\n", __FUNCTION__, sock);
							close(sock);
							break;
						}
						vTaskDelay( 10 / portTICK_RATE_MS );
			}else{
				print("Recibido \n");
				timeout = 10000;
				//write_led(rx_buf[0]);
				if((nwrote = lwip_send(sock, rx_buf, BUF_SIZE, MSG_DONTWAIT)) < 0){
					print("Error \n");
				}

			}

	}

	handle = NULL;
	vTaskDelete(NULL);
	return;

}

void rx_data()
{

		struct ip_addr servaddr;
		int serv_port;

		serv_port = 8080;

		/* Set here the host (PC) IP address */
		IP4_ADDR(&servaddr,  192, 168,   1, 11);

		memset((void*)&serv_addr, 0, sizeof serv_addr);
		serv_addr.sin_family = AF_INET;
		serv_addr.sin_port = htons(serv_port);
		serv_addr.sin_addr.s_addr = servaddr.addr;

	    //print_ip("connect to  ", &servaddr);
	    printf("... \n");


	    //xTaskCreate( receiver, "receiver", 1000, NULL, 1, &handle );

	    handle = sys_thread_new("receiver", (void(*)(void*))receiver, NULL,
	    			                        THREAD_STACKSIZE,
	    			                        DEFAULT_THREAD_PRIO);

	    int boton;

	while(1){

		boton = read_bt();

		if ( (handle == NULL) && ((boton & 1) == 1) ){

			handle = sys_thread_new("receiver", (void(*)(void*))receiver, NULL,
				    			                        THREAD_STACKSIZE,
				    			                        DEFAULT_THREAD_PRIO);

			do{
				vTaskDelay( 500 / portTICK_RATE_MS );
				boton = read_bt();
			}while((boton & 1) == 1);

		}

	}

	vTaskDelete(NULL);
	return;

}



