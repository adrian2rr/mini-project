/*
 * config.h
 *
 *  Created on: May 9, 2018
 *      Author: adrianrodriguezrodri
 */

#ifndef CONFIG_H_
#define CONFIG_H_

#define THREAD_STACKSIZE 4096

#define BIT_VERT 7

#define X_RES 320
#define Y_RES 240
#define MAP_RES 1000

#define R_RES 31
#define B_RES 31
#define G_RES 63
#define COL_RES 100

typedef struct PIXEL {
    int x; //[0,100]
    int y; //[0,100]
    int red; //[0, 31]
    int blue; //[0, 31]
    int green; // [0, 63]
} PIXEL;

typedef struct RGB {
    int red; //[0, 31]
    int blue; //[0, 31]
    int green; // [0, 63]
} RGB;

typedef struct PALA {
	RGB color;
	int ancho;
	int largo;
	int pos;
	int margen;
} PALA;

#define FRAME_SIZE 50
#define FRAME_OFFSET 20

#define PALA_DEF_SP 10

#endif /* CONFIG_H_ */
