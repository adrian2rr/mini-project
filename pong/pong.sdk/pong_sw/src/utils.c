/*
 * utils.c
 *
 *  Created on: May 10, 2018
 *      Author: adrianrodriguezrodri
 */

#include "xgpio.h"
#include <stdlib.h>
#include <stdio.h>

#include "config.h"
#include "plot.h"

XGpio addgre, redblu, leds, bots, sws;


void init_gpios()
{

	XGpio_Initialize(&addgre, XPAR_ADD_GRE_DEVICE_ID);
	XGpio_SetDataDirection(&addgre, 1, 0x00000000);
	XGpio_SetDataDirection(&addgre, 2, 0x00000000);

	XGpio_Initialize(&redblu, XPAR_RED_BLUE_DEVICE_ID);
	XGpio_SetDataDirection(&redblu, 1, 0x00000000);
	XGpio_SetDataDirection(&redblu, 2, 0x00000000);

	XGpio_Initialize(&bots, XPAR_BTN_DEVICE_ID);
	XGpio_SetDataDirection(&bots, 1, 0xffffffff);

	XGpio_Initialize(&leds, XPAR_LED_DEVICE_ID);
	XGpio_SetDataDirection(&leds, 1, 0x00000000);

	XGpio_Initialize(&sws, XPAR_SW_DEVICE_ID);
	XGpio_SetDataDirection(&sws, 1, 0xffffffff);

	return;
}

void write_led(int psb_led)
{
	XGpio_DiscreteWrite(&leds, 1, psb_led);

	return;
}


int read_bt()
{

	int psb_check = XGpio_DiscreteRead(&bots, 1);

	return psb_check;
}

int read_sw()
{

	int sws_check = XGpio_DiscreteRead(&sws, 1);

	return sws_check;
}

void plot_pixel(PIXEL pixel)
{

	float aux;
	int addrpos, r, b, g;

	aux = (pixel.y*Y_RES/MAP_RES)*(512);
	aux = aux + pixel.x*X_RES/MAP_RES;
	addrpos =(int)aux;
	//addrpos = pixel.y*512+pixel.x;
	r = pixel.red;
	b = pixel.blue;
	g = pixel.green;

	//printf("%d - %d : %d\n",pixel.x,pixel.y,addrpos);
	//printf("%d - %d : %d %f\n",pixel.x,pixel.y,addrpos,aux);

	XGpio_DiscreteWrite(&redblu, 1, r);
	XGpio_DiscreteWrite(&redblu, 2, b);
	XGpio_DiscreteWrite(&addgre, 2, g);
	XGpio_DiscreteWrite(&addgre, 1, addrpos);
	//asm ("nop");
	//asm ("nop");
	XGpio_DiscreteWrite(&addgre, 1, 123300);

	return;
}

RGB pon_color(int r, int g, int b)
{
	RGB color;
	float aux;

	aux = b*B_RES/COL_RES;
	color.blue = (int)aux;
	aux = g*G_RES/COL_RES;
	color.green = (int)aux;
	aux = r*R_RES/COL_RES;
	color.red = (int)aux;

	return color;
}

PIXEL pon_color_pixel(RGB color)
{
	PIXEL pixel;

	pixel.blue = color.blue;
	pixel.green = color.green;
	pixel.red = color.red;

	return pixel;
}

PALA crear_pala(RGB color, int ancho, int largo, int margen){

	PALA pala;

	pala.color = color;
	pala.ancho = ancho;
	pala.largo = largo;
	pala.margen = margen;

	return pala;
}

BALL crear_ball(RGB color, RGB fondo, int size, int x, int y){

	BALL ball;

	ball.color = color;
	ball.fondo = fondo;
	ball.size = size;
	ball.posx = x;
	ball.posy = y;

	return ball;
}

BALL mover_ball(BALL ball, int x, int y){

	ball.lposx = ball.posx;
	ball.lposy = ball.posy;
	ball.posx += x;
	ball.posy += y;

	plot_ball(ball);

	return ball;
}
