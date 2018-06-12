/*
 * server.c
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


//====================================================

static unsigned serv_port = 8080;

int sw_state;

//====================================================


void process_request(int sd)
{

	int new_sw, nwrote, i;
	int BUF_SIZE = 20;
	char tx_buf[BUF_SIZE];


	while(1){

		/*new_sw = read_sw();

		if( (new_sw & 1) != (sw_state & 1) ){

			sw_state = new_sw;

			//xil_printf("DIP Switch Status %x\r\n", sw_state);*/

			//tx_buf[0] = sw_state;
			tx_buf[0] = 0;
			tx_buf[1] = '\0';

			if ((nwrote = write(sd, tx_buf, BUF_SIZE)) < 0) {

				printf("%s: ERROR responding to client. received = %d, written = %d\r\n",
									__FUNCTION__, nwrote);
				printf("Closing socket %d\r\n", sd);
				break;

			}
			print("Enviado \n");

			if ((i = read(sd, tx_buf, BUF_SIZE)) < 0) {
									vTaskDelay( 10 / portTICK_RATE_MS );
			}else{
				print("Recibido \n");

			}


			vTaskDelay( 1000 / portTICK_RATE_MS );

		//}

	}

	close(sd);
	vTaskDelete(NULL);
	return;
}



//====================================================


void tx_data()
{

		int sock, new_sd;
		struct sockaddr_in address, remote;
		int size;

		/* initialize devices */
		//platform_init_gpios();

		/* create a TCP socket */
		if ((sock = lwip_socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		vTaskDelete(NULL);
			return;
		}

		/* bind to port 80 at any interface */
		address.sin_family = AF_INET;
		address.sin_port = htons(serv_port);
		address.sin_addr.s_addr = INADDR_ANY;
		if (lwip_bind(sock, (struct sockaddr *)&address, sizeof (address)) < 0) {
		vTaskDelete(NULL);
			return;
		}

		//sw_state = read_sw();

		/* listen for incoming connections */
		lwip_listen(sock, 0);

		size = sizeof(remote);
		while (1) {
			new_sd = lwip_accept(sock, (struct sockaddr *)&remote, (socklen_t *)&size);
			/* spawn a separate handler for each request */
			sys_thread_new("sever", (void(*)(void*))process_request, (void*)new_sd,
	                        THREAD_STACKSIZE,
	                        DEFAULT_THREAD_PRIO);
		}

		return;
}
