/*
 * scoreboard.c
 *
 *  Created on: 28/05/2018
 *      Author: david.llacer
 *
 */

#include "xgpio.h"
#include <stdlib.h>
#include <stdio.h>

#include "config.h"
#include "plot.h"
#include "utils.h"
#include "words.h"

//===================== VARIABLES ===============================

int* number_pointer;

int one[60] = {0,0,1,1,0,0,
			   0,0,1,1,0,0,
			   1,1,1,1,0,0,
			   1,1,1,1,0,0,
			   0,0,1,1,0,0,
			   0,0,1,1,0,0,
			   0,0,1,1,0,0,
			   0,0,1,1,0,0,
			   1,1,1,1,1,1,
			   1,1,1,1,1,1};

int two[60] = {1,1,1,1,1,1,
				  1,1,1,1,1,1,
				  0,0,0,0,1,1,
				  0,0,0,0,1,1,
				  1,1,1,1,1,1,
				  1,1,1,1,1,1,
				  1,1,0,0,0,0,
				  1,1,0,0,0,0,
				  1,1,1,1,1,1,
				  1,1,1,1,1,1};

int three[60] = {1,1,1,1,1,1,
				1,1,1,1,1,1,
				0,0,0,0,1,1,
				0,0,0,0,1,1,
				0,0,1,1,1,1,
				0,0,1,1,1,1,
				0,0,0,0,1,1,
				0,0,0,0,1,1,
				1,1,1,1,1,1,
				1,1,1,1,1,1};

int four[60] = {1,1,0,0,1,1,
				1,1,0,0,1,1,
				1,1,0,0,1,1,
				1,1,0,0,1,1,
				1,1,1,1,1,1,
				1,1,1,1,1,1,
				0,0,0,0,1,1,
				0,0,0,0,1,1,
				0,0,0,0,1,1,
		  		0,0,0,0,1,1};

int five[60] = {1,1,1,1,1,1,
					     1,1,1,1,1,1,
					     1,1,0,0,0,0,
					     1,1,0,0,0,0,
					     1,1,1,1,1,1,
					     1,1,1,1,1,1,
					     0,0,0,0,1,1,
				         0,0,0,0,1,1,
				         1,1,1,1,1,1,
		  		         1,1,1,1,1,1};

int six[60] = {1,1,1,1,1,1,
					    1,1,1,1,1,1,
					    1,1,0,0,0,0,
					    1,1,0,0,0,0,
					    1,1,1,1,1,1,
					    1,1,1,1,1,1,
					    1,1,0,0,1,1,
					    1,1,0,0,1,1,
					    1,1,1,1,1,1,
					    1,1,1,1,1,1};

int seven[60] = {1,1,1,1,1,1,
					      1,1,1,1,1,1,
					      0,0,0,0,1,1,
					      0,0,0,0,1,1,
					      0,0,0,0,1,1,
					      0,0,0,0,1,1,
					      0,0,0,0,1,1,
					      0,0,0,0,1,1,
					      0,0,0,0,1,1,
					      0,0,0,0,1,1};

int eight[60] = {1,1,1,1,1,1,
					      1,1,1,1,1,1,
					      1,1,0,0,1,1,
					      1,1,0,0,1,1,
					      1,1,1,1,1,1,
					      1,1,1,1,1,1,
					      1,1,0,0,1,1,
					      1,1,0,0,1,1,
					      1,1,1,1,1,1,
					      1,1,1,1,1,1};


int nine[60] = {1,1,1,1,1,1,
					     1,1,1,1,1,1,
					     1,1,0,0,1,1,
					     1,1,0,0,1,1,
					     1,1,1,1,1,1,
					     1,1,1,1,1,1,
					     0,0,0,0,1,1,
				         0,0,0,0,1,1,
				         1,1,1,1,1,1,
		  		         1,1,1,1,1,1};

int zero[60] = {1,1,1,1,1,1,
					     1,1,1,1,1,1,
					     1,1,0,0,1,1,
					     1,1,0,0,1,1,
					    1,1,0,0,1,1,
					     1,1,0,0,1,1,
					     1,1,0,0,1,1,
					     1,1,0,0,1,1,
					     1,1,1,1,1,1,
					     1,1,1,1,1,1};


//===================== FUNCIONES ===============================

int * get_number (int n){

	switch(n){
	case 1:
		return one;
	case 2:
		return two;
	case 3:
		return three;
	case 4:
		return four;
	case 5:
		return five;
	case 6:
		return six;
	case 7:
		return seven;
	case 8:
		return eight;
	case 9:
		return nine;
	default:
		return zero;
	}
}

void paint_num(int x_player, int s){

	PIXEL p1, p_back;
	RGB color;

	color = pon_color(0, 191, 255);

	p_back.red = 0;
	p_back.green = 0;
	p_back.blue = 0;


	p1 = pon_color_pixel(color);

	int x, y, i, e;
	//posicion 4,1 --> x + y*(x_max +1) = 13
	for(y=0; y<10; y++){
		for(e = 0; e < s; e++){
			for(x=0; x<6; x++){
				for(i = 0; i < s; i++){
					if(number_pointer[6*y+x] == 1){
						p1.x = x*s + i + x_player;
						p1.y = y*s + e + Y_SCOREBOARD;
						plot_pixel(p1);
					}else{
						p_back.x = x*s + i + x_player;
						p_back.y = y*s + e + Y_SCOREBOARD;
						plot_pixel(p_back);
					}
				}
			}
		}
	}
}

void actualizar_scoreboard(SCOREBOARD scoreboard){

	int score_p1 = scoreboard.score_p1;
	int score_p2 = scoreboard.score_p2;

	//pintar score 1
	number_pointer = get_number(score_p1);
	paint_num(scoreboard.posx_p1,scoreboard.size);
	//pintar score 2
	number_pointer = get_number(score_p2);
	paint_num(scoreboard.posx_p2,scoreboard.size);
}

