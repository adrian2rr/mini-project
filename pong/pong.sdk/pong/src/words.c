/*
 * words.c
 *
 *  Created on: Jun 14, 2018
 *      Author: adrianrodriguezrodri
 */

#include "xgpio.h"
#include <stdlib.h>
#include <stdio.h>

#include "config.h"
#include "plot.h"
#include "utils.h"
#include "words.h"

int* number_pointer;

int a[60] =  {1,1,1,1,1,1,
	      1,1,1,1,1,1,
	      1,1,0,0,1,1,
	      1,1,0,0,1,1,
	      1,1,0,0,1,1,
              1,1,1,1,1,1,
	      1,1,1,1,1,1,
              1,1,0,0,1,1,
	      1,1,0,0,1,1,
	      1,1,0,0,1,1};

int b[60] =  {1,1,1,1,1,1,
	      1,1,1,1,1,1,
	      1,1,0,0,1,1,
	      1,1,0,0,1,1,
	      1,1,1,1,1,0,
              1,1,1,1,1,0,
	      1,1,0,0,1,1,
              1,1,0,0,1,1,
	      1,1,1,1,1,1,
	      1,1,1,1,1,1};

int c[60] =  {1,1,1,1,1,1,
	      1,1,1,1,1,1,
	      1,1,0,0,0,0,
	      1,1,0,0,0,0,
	      1,1,0,0,0,0,
              1,1,0,0,0,0,
	      1,1,0,0,0,0,
              1,1,0,0,0,0,
	      1,1,1,1,1,1,
	      1,1,1,1,1,1};

int d[60] =  {1,1,1,1,1,0,
	      1,1,1,1,1,1,
	      1,1,0,0,1,1,
	      1,1,0,0,1,1,
	      1,1,0,0,1,1,
              1,1,0,0,1,1,
	      1,1,0,0,1,1,
              1,1,0,0,1,1,
	      1,1,1,1,1,1,
	      1,1,1,1,1,0};

int e[60] =  {1,1,1,1,1,1,
	      1,1,1,1,1,1,
	      1,1,0,0,0,0,
	      1,1,0,0,0,0,
	      1,1,1,1,0,0,
              1,1,1,1,0,0,
	      1,1,0,0,0,0,
              1,1,0,0,0,0,
	      1,1,1,1,1,1,
	      1,1,1,1,1,1};

int f[60] =  {1,1,1,1,1,1,
	      1,1,1,1,1,1,
	      1,1,0,0,0,0,
	      1,1,0,0,0,0,
	      1,1,1,1,0,0,
              1,1,1,1,0,0,
	      1,1,0,0,0,0,
              1,1,0,0,0,0,
	      1,1,0,0,0,0,
	      1,1,0,0,0,0,};

int g[60] =  {1,1,1,1,1,1,
	      1,1,1,1,1,1,
	      1,1,0,0,0,0,
	      1,1,0,0,0,0,
	      1,1,0,1,1,1,
              1,1,0,1,1,1,
	      1,1,0,0,1,1,
              1,1,0,0,1,1,
	      1,1,1,1,1,1,
	      1,1,1,1,1,1};

int h[60] =  {1,1,0,0,1,1,
	      1,1,0,0,1,1,
	      1,1,0,0,1,1,
	      1,1,0,0,1,1,
	      1,1,1,1,1,1,
              1,1,1,1,1,1,
	      1,1,0,0,1,1,
              1,1,0,0,1,1,
	      1,1,0,0,1,1,
	      1,1,0,0,1,1};

int i[60] =  {0,1,1,1,1,0,
	      0,1,1,1,1,0,
	      0,0,1,1,0,0,
	      0,0,1,1,0,0,
	      0,0,1,1,0,0,
              0,0,1,1,0,0,
	      0,0,1,1,0,0,
	      0,0,1,1,0,0,
	      0,1,1,1,1,0,
	      0,1,1,1,1,0,};

int j[60] =  {0,0,0,0,1,1,
	      0,0,0,0,1,1,
	      0,0,0,0,1,1,
	      0,0,0,0,1,1,
	      0,0,0,0,1,1,
              0,0,0,0,1,1,
	      0,0,0,0,1,1,
	      1,1,0,0,1,1,
	      1,1,1,1,1,1,
	      0,1,1,1,1,0,};

int k[60] =  {1,1,0,0,1,1,
	      1,1,0,0,1,1,
	      1,1,0,0,1,1,
	      1,1,0,0,1,1,
	      1,1,1,1,1,0,
              1,1,1,1,1,0,
	      1,1,0,0,1,1,
              1,1,0,0,1,1,
	      1,1,0,0,1,1,
	      1,1,0,0,1,1};

int l[60] =  {1,1,0,0,0,0,
	      1,1,0,0,0,0,
	      1,1,0,0,0,0,
	      1,1,0,0,0,0,
	      1,1,0,0,0,0,
              1,1,0,0,0,0,
	      1,1,0,0,0,0,
              1,1,0,0,0,0,
	      1,1,1,1,1,1,
	      1,1,1,1,1,1};

int m[60] =  {1,0,0,0,0,1,
	      1,1,0,0,1,1,
	      1,1,1,1,1,1,
	      1,1,1,1,1,1,
	      1,0,1,1,0,1,
              1,1,0,0,1,1,
	      1,1,0,0,1,1,
              1,1,0,0,1,1,
	      1,1,0,0,1,1,
	      1,1,0,0,1,1};

int n[60] =  {1,0,0,0,1,1,
	      1,1,0,0,1,1,
	      1,1,0,0,1,1,
	      1,1,1,0,1,1,
	      1,1,1,1,1,1,
              1,1,1,1,1,1,
	      1,1,0,1,1,1,
              1,1,0,0,1,1,
	      1,1,0,0,1,1,
	      1,1,0,0,0,1};

int n2[60] =  {0,0,1,0,1,0,
	      0,1,0,1,0,0,
	      0,0,0,0,0,0,
	      1,0,0,0,1,1,
	      1,1,0,0,1,1,
              1,1,1,0,1,1,
	      1,1,1,1,1,1,
              1,1,0,1,1,1,
	      1,1,0,0,1,1,
	      1,1,0,0,0,1};

int o[60] =  {0,1,1,1,1,0,
	      1,1,1,1,1,1,
	      1,1,0,0,1,1,
	      1,1,0,0,1,1,
	      1,1,0,0,1,1,
              1,1,0,0,1,1,
	      1,1,0,0,1,1,
              1,1,0,0,1,1,
	      1,1,1,1,1,1,
	      0,1,1,1,1,0};

int p[60] =  {1,1,1,1,1,0,
	      1,1,1,1,1,1,
	      1,1,0,0,1,1,
	      1,1,0,0,1,1,
	      1,1,1,1,1,1,
              1,1,1,1,1,0,
	      1,1,0,0,0,0,
	      1,1,0,0,0,0,
	      1,1,0,0,0,0,
	      1,1,0,0,0,0,};

int q[60] =  {0,1,1,1,1,0,
	      1,1,1,1,1,1,
	      1,1,0,0,1,1,
	      1,1,0,0,1,1,
	      1,1,0,0,1,1,
              1,1,0,0,1,1,
	      1,1,0,0,1,1,
              1,1,0,1,1,1,
	      1,1,1,1,1,0,
	      0,1,1,1,0,1};

int r[60] =  {1,1,1,1,1,0,
	      1,1,1,1,1,1,
	      1,1,0,0,1,1,
	      1,1,0,0,1,1,
	      1,1,0,0,1,1,
              1,1,1,1,1,0,
	      1,1,1,1,0,0,
	      1,1,0,1,1,0,
	      1,1,0,0,1,1,
	      1,1,0,0,0,1,};

int s[60] =  {0,1,1,1,1,1,
	      1,1,1,1,1,1,
	      1,1,0,0,0,0,
	      1,1,0,0,0,0,
	      1,1,1,1,1,0,
              1,1,1,1,1,1,
	      0,0,0,0,1,1,
              0,0,0,0,1,1,
	      1,1,1,1,1,1,
	      1,1,1,1,1,0};

int t[60] =  {1,1,1,1,1,1,
	      1,1,1,1,1,1,
	      0,0,1,1,0,0,
	      0,0,1,1,0,0,
	      0,0,1,1,0,0,
              0,0,1,1,0,0,
	      0,0,1,1,0,0,
	      0,0,1,1,0,0,
	      0,0,1,1,0,0,
	      0,0,1,1,0,0,};

int u[60] =  {1,1,0,0,1,1,
	      1,1,0,0,1,1,
	      1,1,0,0,1,1,
	      1,1,0,0,1,1,
	      1,1,0,0,1,1,
              1,1,0,0,1,1,
	      1,1,0,0,1,1,
              1,1,0,0,1,1,
	      1,1,1,1,1,1,
	      0,1,1,1,1,0};

int v[60] =  {1,1,0,0,1,1,
	      1,1,0,0,1,1,
	      1,1,0,0,1,1,
	      1,1,0,0,1,1,
	      1,1,0,0,1,1,
              1,1,0,0,1,1,
	      1,1,0,0,1,1,
              0,1,1,1,1,0,
	      0,0,1,1,0,0,
	      0,0,1,1,0,0};

int w[60] =  {1,1,0,0,1,1,
	      1,1,0,0,1,1,
	      1,1,0,0,1,1,
	      1,1,0,0,1,1,
	      1,1,0,0,1,1,
              1,0,1,1,0,1,
	      1,1,1,1,1,1,
              1,1,1,1,1,1,
	      1,1,0,0,1,1,
	      1,0,0,0,0,1,};

int x[60] =  {1,1,0,0,1,1,
	      1,1,0,0,1,1,
	      0,1,0,0,1,0,
	      0,1,1,1,1,0,
	      0,0,1,1,0,0,
              0,0,1,1,0,0,
	      0,1,1,1,1,0,
              0,1,0,0,1,0,
	      1,1,0,0,1,1,
	      1,1,0,0,1,1};

int y[60] =  {1,1,0,0,1,1,
	      1,1,0,0,1,1,
	      1,1,0,0,1,1,
	      0,1,1,1,1,0,
	      0,0,1,1,0,0,
              0,0,1,1,0,0,
	      0,0,1,1,0,0,
              0,0,1,1,0,0,
	      0,0,1,1,0,0,
	      0,0,1,1,0,0,};

int z[60] =  {1,1,1,1,1,1,
	      1,1,1,1,1,1,
	      0,0,0,0,1,1,
	      0,0,0,1,1,1,
	      0,0,1,1,1,0,
              0,1,1,1,0,0,
	      1,1,1,0,0,0,
              1,1,0,0,0,0,
	      1,1,1,1,1,1,
	      1,1,1,1,1,1};

// simbolos

int exclam1[60] =  {0,0,1,1,0,0,
	            0,0,1,1,0,0,
	            0,0,1,1,0,0,
	            0,0,1,1,0,0,
	            0,0,1,1,0,0,
                    0,0,1,1,0,0,
	            0,0,1,1,0,0,
	            0,0,0,0,0,0,
	            0,0,1,1,0,0,
	            0,0,1,1,0,0,};

int exclam2[60] =  {0,0,1,1,0,0,
	            0,0,1,1,0,0,
	            0,0,0,0,0,0,
	            0,0,1,1,0,0,
	            0,0,1,1,0,0,
                    0,0,1,1,0,0,
	            0,0,1,1,0,0,
	            0,0,1,1,0,0,
	            0,0,1,1,0,0,
	            0,0,1,1,0,0,};

int interr1[60] =  {1,1,1,1,1,1,
	            1,1,1,1,1,1,
	            0,0,0,0,1,1,
	            0,0,0,0,1,1,
	            1,1,1,1,1,1,
                    1,1,1,1,1,1,
	            1,1,0,0,0,0,
	            0,0,0,0,0,0,
	            1,1,0,0,0,0,
	            1,1,0,0,0,0,};

int interr2[60] =  {0,0,0,0,1,1,
	            0,0,0,0,1,1,
	            0,0,0,0,0,0,
	            0,0,0,0,1,1,
	            1,1,1,1,1,1,
                    1,1,1,1,1,1,
	            1,1,0,0,0,0,
	            1,1,0,0,0,0,
	            1,1,1,1,1,1,
	            1,1,1,1,1,1,};

int period[60] =  {0,0,0,0,0,0,
	           0,0,0,0,0,0,
	           0,0,0,0,0,0,
	           0,0,0,0,0,0,
	           0,0,0,0,0,0,
	           0,0,0,0,0,0,
	           0,0,0,0,0,0,
	           0,0,0,0,0,0,
	           1,1,0,0,0,0,
	           1,1,0,0,0,0,};

int comma[60] =  {0,0,0,0,0,0,
	          0,0,0,0,0,0,
	          0,0,0,0,0,0,
	          0,0,0,0,0,0,
	          0,0,0,0,0,0,
	          0,0,0,0,0,0,
	          0,0,0,0,0,0,
	          1,1,0,0,0,0,
	          1,1,0,0,0,0,
	          0,1,0,0,0,0,};

int colon[60] =  {0,0,0,0,0,0,
	          0,0,0,0,0,0,
	          1,1,0,0,0,0,
	          1,1,0,0,0,0,
	          0,0,0,0,0,0,
	          0,0,0,0,0,0,
	          0,0,0,0,0,0,
	          0,0,0,0,0,0,
	          1,1,0,0,0,0,
	          1,1,0,0,0,0,};

int semi_colon[60] =  {0,0,0,0,0,0,
	               0,0,0,0,0,0,
	               1,1,0,0,0,0,
	               1,1,0,0,0,0,
	               0,0,0,0,0,0,
	               0,0,0,0,0,0,
	               0,0,0,0,0,0,
	               1,1,0,0,0,0,
	               1,1,0,0,0,0,
	               0,1,0,0,0,0,};

int dash[60] =  {0,0,0,0,0,0,
	         0,0,0,0,0,0,
	         0,0,0,0,0,0,
	         0,0,0,0,0,0,
	         1,1,1,1,1,0,
	         1,1,1,1,1,0,
	         0,0,0,0,0,0,
	         0,0,0,0,0,0,
	         0,0,0,0,0,0,
	         0,0,0,0,0,0,};

int underscore[60] =  {0,0,0,0,0,0,
	               0,0,0,0,0,0,
	               0,0,0,0,0,0,
	               0,0,0,0,0,0,
	               0,0,0,0,0,0,
	               0,0,0,0,0,0,
	               0,0,0,0,0,0,
	               0,0,0,0,0,0,
	               1,1,1,1,1,1,
	               1,1,1,1,1,1,};

void paint_let(int x_pos, int s, RGB fon){

	PIXEL p1, p_back;
	RGB color;

	color = pon_color(0, 191, 255);

	p_back = pon_color_pixel(fon);


	p1 = pon_color_pixel(color);

	int x, y, i, e;
	//posicion 4,1 --> x + y*(x_max +1) = 13
	for(y=0; y<10; y++){
		for(e = 0; e < s; e++){
			for(x=0; x<6; x++){
				for(i = 0; i < s; i++){
					if(number_pointer[6*y+x] == 1){
						p1.x = x*s + i + x_pos;
						p1.y = y*s + e + Y_SCOREBOARD;
						plot_pixel(p1);
					}else{
						p_back.x = x*s + i + x_pos;
						p_back.y = y*s + e + Y_SCOREBOARD;
						plot_pixel(p_back);
					}
				}
			}
		}
	}
}

void print_title(RGB fon) {

	int size;

	size = 10;

	number_pointer = p;
	paint_let(100, size,fon);

	number_pointer = o;
	paint_let(180, size,fon);

	number_pointer = n;
	paint_let(260, size,fon);

	number_pointer = g;
	paint_let(340, size,fon);

	return;
}


