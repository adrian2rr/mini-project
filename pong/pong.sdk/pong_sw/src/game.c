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

PALA pala1, pala2;

int pala_sp;

int top_limit, bot_limit;

//====================================================

int ball_pala(BALL ball, PALA pala){

	float aux;

	if((ball.posy < pala.pos+pala.largo) && (ball.posy+ball.size*BALL_SIZE > pala.pos)){
		aux = ball.posy - (pala.pos+pala.largo/2);
		aux = aux*0.15;
		return (int)aux;
	}else{
		return 999;
	}

}

void control_pala(){

	int bt;
	bt = 0;

	while(1){

		bt = read_bt();

		//printf("%d \n",bt);

		if ( bt == 1 ){

			if ( pala1.pos+pala_sp <= bot_limit ){
				pala1.lpos = pala1.pos;
				pala1.pos = pala1.pos+pala_sp;
				plot_pala(pala1, 1);
			}

		}else if ( bt == 2 ){

			if ( pala1.pos-pala_sp >= top_limit ){
				pala1.lpos = pala1.pos;
				pala1.pos = pala1.pos-pala_sp;
				plot_pala(pala1, 1);
			}

		}

		if ( bt == 4 ){

					if ( pala2.pos+pala_sp <= bot_limit ){
						pala2.lpos = pala2.pos;
						pala2.pos = pala2.pos+pala_sp;
						plot_pala(pala2, 2);
					}

				}else if ( bt == 8 ){

					if ( pala2.pos-pala_sp >= top_limit ){
						pala2.lpos = pala2.pos;
						pala2.pos = pala2.pos-pala_sp;
						plot_pala(pala2, 2);
					}

				}

		vTaskDelay( 5 / portTICK_RATE_MS );

	}

	vTaskDelete(NULL);

	return;
}


void control_ball(){

	int limit_ty, limit_by, limit_lx, limit_rx;

	limit_ty = FRAME_OFFSET+FRAME_SIZE+1;
	limit_by = MAP_RES-(FRAME_OFFSET+FRAME_SIZE+1);
	limit_lx = FRAME_OFFSET+2*FRAME_SIZE+6;
	limit_rx = MAP_RES-(FRAME_OFFSET+2*FRAME_SIZE+6);

	int b_ini_x = 100;
	int b_ini_y = 100;

	int aux;

	BALL ball;

	ball = crear_ball(pon_color(129, 243, 232), pon_color(0,0,0), 2, b_ini_x, b_ini_y);

	ball.xsp = 2;
	ball.ysp = -3;

	while(1){

		aux = 1000;

		if (ball.xsp > 0){

			if ( ball.posx+ball.size*BALL_SIZE+ball.xsp < limit_rx){
				ball.lposx = ball.posx;
				ball.posx += ball.xsp;
			}else{
				aux = ball_pala(ball,pala2);
				if(aux == 999){
					ball.lposx = ball.posx;
					ball.posx = b_ini_x;
				}else{
					ball.lposx = ball.posx;
					ball.posx += ball.xsp;
					ball.xsp = -ball.xsp;
				}
			}

		}else{

			if ( ball.posx+ball.xsp > limit_lx){
				ball.lposx = ball.posx;
				ball.posx += ball.xsp;
			}else{
				aux = ball_pala(ball,pala1);
				if(aux == 999){
					ball.lposx = ball.posx;
					ball.posx = b_ini_x;
				}else{
					ball.lposx = ball.posx;
					ball.posx += ball.xsp;
					ball.xsp = -ball.xsp;
				}
			}

		}


		if ( aux == 999 ){

			ball.lposy = ball.posy;
			ball.posy = b_ini_y;

		}else{

			if ( aux != 1000 )
				ball.ysp = aux;

			if ( ball.ysp > 0){

				if( ball.posy+ball.size*BALL_SIZE+ball.ysp < limit_by ){
					ball.lposy = ball.posy;
					ball.posy += ball.ysp;
				}else{
					ball.ysp = -ball.ysp;
					ball.lposy = ball.posy;
					ball.posy += ball.ysp;
				}

			}else{

				if ( ball.posy+ball.ysp > limit_ty ){
					ball.lposy = ball.posy;
					ball.posy += ball.ysp;
				}else{
					ball.ysp = -ball.ysp;
					ball.lposy = ball.posy;
					ball.posy += ball.ysp;
				}

			}

		}

		plot_ball(ball);
		plot_red(pon_color(144,195,0), 7, 20);

		vTaskDelay( 10 / portTICK_RATE_MS );

	}

	vTaskDelete(NULL);

	return;
}

//====================================================

void game_thread(){

	pala1 = crear_pala(pon_color(129, 243, 232), FRAME_SIZE, 100, 5);
	pala1.pos = 50;
	pala1.lpos = 50;
	pala1.fondo = pon_color(0,0,0);
	plot_pala(pala1, 1);

	pala2 = crear_pala(pon_color(129, 243, 232), FRAME_SIZE, 100, 5);
	pala2.pos = 50;
	pala2.lpos = 50;
	pala2.fondo = pon_color(0,0,0);
	plot_pala(pala2, 2);

	pala_sp = PALA_DEF_SP;

	top_limit = FRAME_OFFSET+FRAME_SIZE+1;

	bot_limit = MAP_RES-(FRAME_OFFSET+FRAME_SIZE+pala1.largo);

	sys_thread_new("control_pala", (void(*)(void*))control_pala, 0,
							THREAD_STACKSIZE,
						    DEFAULT_THREAD_PRIO);

	sys_thread_new("control_ball", (void(*)(void*))control_ball, 0,
							THREAD_STACKSIZE,
						    DEFAULT_THREAD_PRIO);

	while(1){

	}


	vTaskDelete(NULL);

	return;
}
