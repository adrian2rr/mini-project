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
#include "xgpio.h"

#include "config.h"
#include "utils.h"
#include "plot.h"
#include "scoreboard.h"

//====================================================

PALA pala1, pala2;

BALL ball;

int pala_sp;

int top_limit, bot_limit;

RGB color_com, color_cua, fondo;

int g_bucle;

int limit_ty, limit_by, limit_lx, limit_rx;

int b_ini_x, b_ini_y;

xTaskHandle juego[3];

xQueueHandle quenet, quegam;

int online_st;

int on_sw_st;

int fin;

int marc[2];

SCOREBOARD scoreboard;

//====================================================

xQueueHandle getqueue(int n);

xQueueHandle get_queser(int p);

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

	vTaskSuspend( NULL );

	while(g_bucle){

		bt = read_bt();

		if ( bt == 1 ){

			if ( pala1.pos+pala_sp <= bot_limit ){
				pala1.lpos = pala1.pos;
				pala1.pos = pala1.pos+pala_sp;
				plot_pala(pala1, 1);
			}

		}else if ( bt == 8 ){

			if ( pala1.pos-pala_sp >= top_limit ){
				pala1.lpos = pala1.pos;
				pala1.pos = pala1.pos-pala_sp;
				plot_pala(pala1, 1);
			}

		}

		vTaskDelay( 20 / portTICK_RATE_MS );

	}

	vTaskDelete(NULL);

	return;
}


void control_ball(){

	int aux;

	marc[0] = 0;
	marc[1] = 0;

	vTaskSuspend(NULL);

	while(g_bucle){

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
					ball.ysp = 0;
					marc[0]++;
					scoreboard.score_p1++;
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
					ball.ysp = 0;
					marc[1]++;
					scoreboard.score_p2++;
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
		plot_red(color_cua, 7, 20);
		actualizar_scoreboard(scoreboard);

		vTaskDelay( 10 / portTICK_RATE_MS );

		if(marc[0] == 7){
			g_bucle = 0;
		}else if(marc[1] == 7){
			g_bucle = 0;
		}

	}

	vTaskDelete(NULL);

	return;
}

void pala_ia(){

	int center, margin;

	vTaskSuspend(NULL);

	margin = (int)pala2.largo*0.2;

	while(g_bucle){

		center = pala2.pos + (pala2.largo/2);

		if ( center < ball.posy-margin ){

			if ( pala2.pos+pala_sp <= bot_limit ){
				pala2.lpos = pala2.pos;
				pala2.pos = pala2.pos+pala_sp;
				plot_pala(pala2, 2);
			}

		}else if ( center > ball.posy+margin ){

			if ( pala2.pos-pala_sp >= top_limit ){
				pala2.lpos = pala2.pos;
				pala2.pos = pala2.pos-pala_sp;
				plot_pala(pala2, 2);
			}

		}

		vTaskDelay( 30 / portTICK_RATE_MS );

	}

	fin = 0;

	vTaskDelete(NULL);
	return;
}

void empty_task(){

	while(g_bucle)
		vTaskSuspend(NULL);

	vTaskDelete(NULL);
	return;
}

void init_game(){

	plot_back(fondo);
	plot_frame(color_cua);
	plot_red(color_cua, 7, 20);

	pala1 = crear_pala(color_com, FRAME_SIZE, 100, 5);
	pala1.pos = 50;
	pala1.lpos = 50;
	pala1.fondo = fondo;
	plot_pala(pala1, 1);

	pala2 = crear_pala(color_com, FRAME_SIZE, 100, 5);
	pala2.pos = 50;
	pala2.lpos = 50;
	pala2.fondo = fondo;
	plot_pala(pala2, 2);

	pala_sp = PALA_DEF_SP;

	top_limit = FRAME_OFFSET+FRAME_SIZE+1;

	bot_limit = MAP_RES-(FRAME_OFFSET+FRAME_SIZE+pala1.largo);

	g_bucle = 1;
	on_sw_st = 1;

	limit_ty = FRAME_OFFSET+FRAME_SIZE+1;
	limit_by = MAP_RES-(FRAME_OFFSET+FRAME_SIZE+1);
	limit_lx = FRAME_OFFSET+2*FRAME_SIZE+6;
	limit_rx = MAP_RES-(FRAME_OFFSET+2*FRAME_SIZE+6);

	b_ini_x = limit_rx/2;
	b_ini_y = limit_by/2;

	ball = crear_ball(color_com, fondo, 2, b_ini_x, b_ini_y);

	ball.xsp = 5;
	ball.ysp = 0;

	scoreboard = crear_scoreboard();

	return;
}

void game_resume(int t){

	if(t == 1){
		print("\n\n --- Resuming Game --- ");
		vTaskResume(juego[0]);
		vTaskResume(juego[1]);
		vTaskResume(juego[2]);
	}else{
		on_sw_st = 0;
	}

	return;
}

void game_susp(int t){

	if(t == 1){
		print("\n\n --- Pausing Game ---");
		vTaskSuspend(juego[0]);
		vTaskSuspend(juego[1]);
		vTaskSuspend(juego[2]);
	}else{
		on_sw_st = 1;
	}

	return;
}

void game_del(int t){

	if(t == 1){
		print("\n\n --- Exiting -- \n");
		vTaskDelete(juego[0]);
		vTaskDelete(juego[1]);
		vTaskDelete(juego[2]);
		fin = 0;
	}else{
		on_sw_st = 2;
	}

	return;
}

void resume_online(int c){

	if(c == 0){
		print("\n\n --- Resuming Game Local --- ");
		if(online_st == 1){
			vTaskResume(juego[1]);
			vTaskResume(juego[2]);
			online_st = 0;
		}else if(online_st == 3){
			online_st = 2;
		}
	}else{
		print("\n\n --- Resuming Game Remote --- ");
		if(online_st == 2){
			vTaskResume(juego[1]);
			vTaskResume(juego[2]);
			online_st = 0;
		}else if(online_st == 3){
			online_st = 1;
		}
	}

	return;
}

void susp_online(int c){

	if(c == 0){
		print("\n\n --- Pausing Game Local ---");
		if(online_st == 0){
			vTaskSuspend(juego[1]);
			vTaskSuspend(juego[2]);
			online_st = 1;
		}else if(online_st == 2){
			online_st = 3;
		}
	}else{
		print("\n\n --- Pausing Game Remote ---");
		if(online_st == 0){
			vTaskSuspend(juego[1]);
			vTaskSuspend(juego[2]);
			online_st = 2;
		}else if(online_st == 1){
			online_st = 3;
		}
	}

	return;
}

void send_client(int s){
	MESS mess;

	mess.id = 3;
	mess.subid = s;
	mess.p = 0;
	mess.xb = 0;
	mess.yb = 0;
	mess.l = 0;
	mess.v = 0;

	xQueueSendToBack( quenet, &mess, ( portTickType ) 10 );

	return;
}

void paint_rem(){

	MESS mess;
	int ch_st;
	int xball, yball, pala;

	quenet = getqueue(0);
	quegam = getqueue(1);

	ch_st = 1;

	online_st = 3;

	while(g_bucle){

		if( xQueueReceive( quegam, &( mess ), ( portTickType ) 10 ) ){

			if(mess.id == 2){
				if(mess.subid < 3){

					xQueueSendToBack( quenet, &mess, ( portTickType ) 10 );

					vTaskDelay( 1 / portTICK_RATE_MS );

					switch(mess.subid){
					case 0:
						resume_online(1);
						break;
					case 1:
						susp_online(1);
						break;
					case 2:
						print("\n\n --- Exiting remote -- \n");
						g_bucle = 0;
						break;
					}

				}else{

					xball = mess.xb;
					yball = mess.yb;
					pala = mess.p;
					marc[1] = mess.l;
					marc[0] = mess.v;

					mess.p = pala1.pos;

					xQueueSendToBack( quenet, &mess, ( portTickType ) 10 );

					vTaskDelay( 1 / portTICK_RATE_MS );

					pala2.lpos = pala2.pos;
					pala2.pos = pala;
					ball.lposx = ball.posx;
					ball.lposy = ball.posy;
					ball.posx = xball;
					ball.posy = yball;

					plot_ball(ball);
					plot_red(color_cua, 7, 20);
					plot_pala(pala2, 2);

				}
			}else{

				switch(mess.subid){
				case 0:
					resume_online(0);
					break;
				case 1:
					susp_online(0);
					break;
				case 2:
					print("\n\n --- Exiting Local-- \n");
					g_bucle = 0;
					break;
				}

			}

		}

		if ( ch_st != on_sw_st ){

			ch_st = on_sw_st;

			send_client(ch_st);

		}

	}

	fin = 0;
	vTaskDelete(NULL);
	return;
}

void send_serv(int s){
	MESS mess;

	mess.id = 2;
	mess.subid = s;
	mess.p = 0;
	mess.xb = 0;
	mess.yb = 0;
	mess.l = 0;
	mess.v = 0;

	xQueueSendToBack( quenet, &mess, ( portTickType ) 10 );

	return;
}

void paint_lo(){

	MESS mess;
	int ch_st;
	int x, g_bucle_l;

	quenet = getqueue(0);
	quegam = getqueue(1);

	mess.id = 0;
	mess.subid = 0;
	mess.p = 0;
	mess.xb = 0;
	mess.yb = 0;
	mess.l = 0;
	mess.v = 0;

	ch_st = 1;

	online_st = 3;

	x = 0;

	g_bucle_l = 1;

	do{

		if( xQueueReceive( quegam, &( mess ), ( portTickType ) 10 ) ){

			if(mess.id == 1){

			}else{
				xQueueSendToBack( quenet, &mess, ( portTickType ) 10 );
				g_bucle = 0;
				g_bucle_l = 0;
			}

			x = 1;

		}else{
			vTaskDelay( 1 / portTICK_RATE_MS );
		}


	}while(x == 0);

	while(g_bucle_l){

		if( xQueueReceive( quegam, &( mess ), ( portTickType ) 10 ) ){

			if(mess.id == 2){
				if(mess.subid < 3){

					switch(mess.subid){
					case 0:
						resume_online(0);
						break;
					case 1:
						susp_online(0);
						break;
					case 2:
						print("\n\n --- Exiting Local -- \n");
						g_bucle_l = 0;
						break;
					}

				}else{

					pala2.lpos = pala2.pos;
					pala2.pos = mess.p;

					plot_pala(pala2, 2);

				}

			}else{

				xQueueSendToBack( quenet, &mess, ( portTickType ) 10 );

				vTaskDelay( 1 / portTICK_RATE_MS );

				switch(mess.subid){
				case 0:
					resume_online(1);
					break;
				case 1:
					susp_online(1);
					break;
				case 2:
					print("\n\n --- Exiting Remote -- \n");
					g_bucle_l = 0;
					break;
				}

			}
		}

		if ( online_st == 0 ){

			if(mess.xb != ball.posx){
				mess.id = 2;
				mess.subid = 3;
				mess.p = pala1.pos;
				mess.xb = MAP_RES - ball.posx;
				mess.yb = ball.posy;
				mess.l = marc[0];
				mess.v = marc[1];

				xQueueSendToBack( quenet, &mess, ( portTickType ) 10 );
			}

		}

		if ( ch_st != on_sw_st ){

			ch_st = on_sw_st;

			send_serv(ch_st);

		}

		if(g_bucle == 0){
			send_serv(2);
		}

	}

	fin = 0;
	vTaskDelete(NULL);
	return;
}

//====================================================

void game_thread(int* c){

	int nsw, lsw, t;

	color_com = pon_color(255,255,255);
	color_cua = pon_color(255,255,255);
	fondo = pon_color(0,0,0);

	t = *c;

	juego[1] = sys_thread_new("control_pala", (void(*)(void*))control_pala, 0, THREAD_STACKSIZE, DEFAULT_THREAD_PRIO);

	switch(t){
	case 1:
		fin = 1;
		init_game();
		juego[2] = sys_thread_new("pala_ia", (void(*)(void*))pala_ia, 0, THREAD_STACKSIZE, DEFAULT_THREAD_PRIO);
		juego[0] = sys_thread_new("control_ball", (void(*)(void*))control_ball, 0, THREAD_STACKSIZE, DEFAULT_THREAD_PRIO);
		break;
	case 2:
		fin = 2;
		init_game();
		juego[2] = sys_thread_new("empty", (void(*)(void*))empty_task, 0, THREAD_STACKSIZE, DEFAULT_THREAD_PRIO);
		juego[0] = sys_thread_new("paint_rem", (void(*)(void*))paint_rem, 0, THREAD_STACKSIZE, DEFAULT_THREAD_PRIO);
		break;
	default:
		fin = 3;
		init_game();
		juego[0] = sys_thread_new("paint_lo", (void(*)(void*))paint_lo, 0, THREAD_STACKSIZE, DEFAULT_THREAD_PRIO);
		juego[2] = sys_thread_new("control_ball", (void(*)(void*))control_ball, 0, THREAD_STACKSIZE, DEFAULT_THREAD_PRIO);
		break;
	}

	lsw = read_sw();

	if(lsw == 0 && fin != 0){
		print("\n\n --- Starting Game --- \n\n");
		vTaskDelay( 100 / portTICK_RATE_MS );
		game_resume(t);
	}else if(lsw != 0 && fin != 0){
		print("\n\n --- Game Ready waiting player1 resume the game -- \n\n Off all switches");
		vTaskDelay( 100 / portTICK_RATE_MS );
	}

	while(lsw != 0){
		lsw = read_sw();
		if(lsw == 0){
			print("\n\n --- Starting Game --- \n\n");
			game_resume(t);
		}
		vTaskDelay( 100 / portTICK_RATE_MS );
	}

	while(fin != 0){

		vTaskDelay( 100 / portTICK_RATE_MS );

		nsw = read_sw();

		if(nsw != lsw){
			if(nsw == 0){
				game_resume(t);
			}else if(nsw == 1 && lsw == 0){
				game_susp(t);
			}else if(nsw == 2 && lsw == 0){
				game_susp(t);
				print("\n\n -- Game paused, finish game?");
			}else if(nsw == 3 && lsw == 2){
				game_del(t);
			}else{
				print("\n\n - Command not known -");
			}
			lsw = nsw;
		}
	}

	printf("\n Resultado: %d - %d \n", marc[0], marc[1]);

	*c = 0;

	vTaskDelete(NULL);

	return;
}
