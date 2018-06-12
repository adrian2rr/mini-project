/*
 * utils.c
 *
 *  Created on: Apr 26, 2018
 *      Author: adrianrodriguezrodri
 */

#include "xgpio.h"

XGpio addgre, redblu, leds, bots, sws;
int i, psb_check, dip_check;


void init_butled()
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
		int psb_check = 0;
	  //psb_check = XGpio_DiscreteRead(&push, 1);

	  //xil_printf("Push Buttons Status %x\r\n", psb_check);

	  return psb_check;
}

int read_sw()
{

	//dip_check = XGpio_DiscreteRead(&dip, 1);

	//xil_printf("DIP Switch Status %x\r\n", dip_check);

	return dip_check;
}
