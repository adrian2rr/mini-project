/*
 * game.c
 *
 *  Created on: May 10, 2018
 *      Author: adrianrodriguezrodri
 */

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

//====================================================

PALA pala1;

int pala_sp;

int top_limit, bot_limit;

//====================================================

void control_pala(){

	int bt;
	bt = 0;

	while(1){

		bt = read_bt();

		if ( (bt & 1) == 1 ){

			if ( pala1.pos+pala_sp < bot_limit+pala1.largo ){
				pala1.pos = pala1.pos+pala_sp;
				plot_pala(pala1, 1);
			}

		}else if ( (bt & 2) == 1){

			if ( pala1.pos-pala_sp > top_limit ){
				pala1.pos = pala1.pos-pala_sp;
				plot_pala(pala1, 1);
			}

		}

		vTaskDelay( 100 / portTICK_RATE_MS );

	}

	vTaskDelete(NULL);

	return;
}


void control_general(){

	vTaskDelete(NULL);

	return;
}


//====================================================

void game_thread(){

	pala1 = crear_pala(pon_color(100,100,100) , FRAME_SIZE, 100, 10);
	pala1.pos = 300;

	pala_sp = PALA_DEF_SP;

	top_limit = FRAME_OFFSET+FRAME_SIZE;

	bot_limit = MAP_RES-(FRAME_OFFSET+FRAME_SIZE);

	sys_thread_new("control_pala", (void(*)(void*))control_pala, 0,
							THREAD_STACKSIZE,
						    DEFAULT_THREAD_PRIO);

	sys_thread_new("control_general", (void(*)(void*))control_pala, 0,
							THREAD_STACKSIZE,
						    DEFAULT_THREAD_PRIO);

	while(1){

	}


	vTaskDelete(NULL);

	return;
}
