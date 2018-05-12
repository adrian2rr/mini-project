/*
 * plot.c
 *
 *  Created on: May 10, 2018
 *      Author: adrianrodriguezrodri
 */

#include "xgpio.h"

#include "config.h"
#include "utils.h"

void plot_back(RGB color)
{
	PIXEL pixel;
	int x, y;

	pixel = pon_color_pixel(color);

	for (y=0; y<MAP_RES; y++){
		for (x=0; x<MAP_RES; x++){
			pixel.x = x;
			pixel.y = y;
			plot_pixel(pixel);
		}
	}

	return;
}

void plot_frame(RGB color)
{
	PIXEL pixel;

	int x, y;

	pixel = pon_color_pixel(color);

	for (y=FRAME_OFFSET; y<MAP_RES-FRAME_OFFSET; y++){
		pixel.y = y;
		if (y < FRAME_OFFSET+FRAME_SIZE){
			for (x=FRAME_OFFSET; x<MAP_RES-FRAME_OFFSET; x++){
				pixel.x = x;
				plot_pixel(pixel);
			}
		}else if(y > MAP_RES-(FRAME_SIZE+FRAME_OFFSET)){
			for (x=FRAME_OFFSET; x<MAP_RES-FRAME_OFFSET; x++){
							pixel.x = x;
							plot_pixel(pixel);
			}
		}else{
			for (x=FRAME_OFFSET; x < FRAME_OFFSET+FRAME_SIZE; x++){
				pixel.x = x;
				plot_pixel(pixel);
				pixel.x = MAP_RES-x;
				plot_pixel(pixel);
			}
		}
	}

	plot_pixel(pixel);

	return;
}

void plot_red(RGB color, int size, int num)
{
	PIXEL pixel;

	int x, y, e;

	num = (MAP_RES-2*(FRAME_OFFSET+FRAME_SIZE))/2*num;

	pixel = pon_color_pixel(color);

	for (e=FRAME_OFFSET+FRAME_SIZE; e<MAP_RES-(FRAME_OFFSET+FRAME_SIZE); e+=2*num){
		for (y=0; y<num; y++){
			for(x=0; x<size; x++){
				pixel.x = MAP_RES/2-(size/2)+x;
				pixel.y = e+y;
				plot_pixel(pixel);
			}
		}
	}

	return;
}

void plot_pala(PALA pala, int jugador)
{
	PIXEL pixel;

	int x, y, x_ini;

	if (jugador == 1){
		x_ini = FRAME_OFFSET+FRAME_SIZE+pala.margen;
	}else{
		x_ini = MAP_RES-(FRAME_OFFSET+FRAME_SIZE+pala.margen+pala.ancho);
	}

	if (pala.pos > pala.lpos){
		pixel = pon_color_pixel(pala.fondo);
		for (y=pala.lpos; y<pala.pos; y++){
			pixel.y = y;
			for(x = x_ini; x<x_ini+pala.ancho; x++){
				pixel.x = x;
				plot_pixel(pixel);
			}
		}
		pixel = pon_color_pixel(pala.color);
		for (y=pala.pos; y<pala.pos+pala.largo; y++){
			pixel.y = y;
			for(x = x_ini; x<x_ini+pala.ancho; x++){
				pixel.x = x;
				plot_pixel(pixel);
			}
		}
	}
	else{
		pixel = pon_color_pixel(pala.color);
		for(y=pala.pos; y<pala.pos+pala.largo; y++){
			pixel.y = y;
			for(x = x_ini; x<x_ini+pala.ancho; x++){
						pixel.x = x;
						plot_pixel(pixel);
			}
		}
		pixel = pon_color_pixel(pala.fondo);
		for(y=pala.pos+pala.largo; y<pala.lpos; y++){
					pixel.y = y;
					for(x = x_ini; x<x_ini+pala.ancho; x++){
								pixel.x = x;
								plot_pixel(pixel);
					}
		}
	}

	return;
}



