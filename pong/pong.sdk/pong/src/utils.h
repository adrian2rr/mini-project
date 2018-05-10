/*
 * utils.h
 *
 *  Created on: May 10, 2018
 *      Author: adrianrodriguezrodri
 */

#ifndef UTILS_H_
#define UTILS_H_

extern void init_gpios();

extern void write_led(int psb_led);

extern int read_bt();

extern int read_sw();

extern void plot_pixel(PIXEL pixel);

extern RGB pon_color(int r, int b, int g);

extern PIXEL pon_color_pixel(RGB color);

extern PALA crear_pala(RGB color, int ancho, int largo, int margen);

#endif /* UTILS_H_ */
