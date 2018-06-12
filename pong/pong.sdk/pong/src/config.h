/*
 * config.h
 *
 *  Created on: May 9, 2018
 *      Author: adrianrodriguezrodri
 */

#ifndef CONFIG_H_
#define CONFIG_H_

#define THREAD_STACKSIZE 2048

#define X_RES 320
#define Y_RES 240
#define MAP_RES 500

#define R_RES 31
#define B_RES 31
#define G_RES 63
#define COL_RES 255

#define X_SCOREBOARD_P1 200
#define X_SCOREBOARD_P2 270
#define Y_SCOREBOARD 70
#define SIZE_SCOREBOARD 5

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
	RGB fondo;
	int ancho;
	int largo;
	int pos;
	int lpos;
	int margen;
} PALA;

#define FRAME_SIZE 10
#define FRAME_OFFSET 10

#define PALA_DEF_SP 8

#define BALL_SIZE 5

typedef struct BALL {
	RGB color;
	RGB fondo;
	int size;
	int posx;
	int posy;
	int lposx;
	int lposy;
	int xsp;
	int ysp;
} BALL;

typedef struct MESS {
	int id;
	int subid;
	int xb;
	int yb;
	int p;
	int l;
	int v;
} MESS;

typedef struct SCOREBOARD{
	int score_p1;
	int score_p2;
	int posx_p1;
	int posx_p2;
	int posy;
	int size;
} SCOREBOARD;

#endif /* CONFIG_H_ */
