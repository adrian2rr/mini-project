#include "xparameters.h"
#include "xgpio.h"

#include <stdlib.h>
#include <stdio.h>
#include "lwipopts.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/init.h"
#include "netif/xadapter.h"
#include "lwip/dhcp.h"
#include "task.h"
#include "portmacro.h"
#include "platform_config.h"
//#include "time.h"

#include "config.h"
#include "utils.h"
#include "plot.h"
// Only needed for SDK versions before 2015
//#include "xutil.h"
  
//====================================================

int bucle;


//====================================================


void server_tx_data();

void game_thread();

void client_rx_data();

void print_clien();

void con_clien();

void demo(){

	int limit_ty, limit_by, limit_lx, limit_rx;

	limit_ty = FRAME_OFFSET+FRAME_SIZE+1;
	limit_by = MAP_RES-(FRAME_OFFSET+FRAME_SIZE+1);
	limit_lx = FRAME_OFFSET+FRAME_SIZE+1;
	limit_rx = MAP_RES-(FRAME_OFFSET+FRAME_SIZE+1);

	RGB color;
	BALL ball;

	color = pon_color(0,0,0);
	plot_back(color);

	color = pon_color(255,255,255);
	plot_frame(color);

	plot_red(color, 5, 10);

	ball = crear_ball(color, pon_color(0,0,0), 2, 100, 100);

	bucle = 1;
	//srand(time(NULL));

	ball.xsp = 5;
	ball.ysp = (rand() % 5) + 1;

	while(bucle){

		if (ball.xsp > 0){
			if ( ball.posx+ball.size*BALL_SIZE+ball.xsp < limit_rx){
				ball.lposx = ball.posx;
				ball.posx += ball.xsp;
			}else{
				color = pon_color((rand() % 255),(rand() % 255),(rand() % 255));
				plot_frame(color);
				color = pon_color((rand() % 255),(rand() % 255),(rand() % 255));
				ball.color = color;
				ball.lposx = ball.posx;
				ball.posx += ball.xsp;
				ball.xsp = -ball.xsp;
				if (ball.ysp < 0)
					ball.ysp = -1*((rand() % 5) + 1);
				else
					ball.ysp = (rand() % 5) + 1;
			}
		}else{
			if ( ball.posx+ball.xsp > limit_lx+1){
				ball.lposx = ball.posx;
				ball.posx += ball.xsp;
			}else{
				color = pon_color((rand() % 255),(rand() % 255),(rand() % 255));
				plot_back(color);
				color = pon_color((rand() % 255),(rand() % 255),(rand() % 255));
				plot_frame(color);
				color = pon_color((rand() % 255),(rand() % 255),(rand() % 255));
				ball.color = color;
				ball.lposx = ball.posx;
				ball.posx += ball.xsp;
				ball.xsp = -ball.xsp;
				if (ball.ysp < 0)
					ball.ysp = -1*((rand() % 5) + 1);
				else
					ball.ysp = (rand() % 5) + 1;
			}
		}

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

		plot_ball(ball);
		plot_red(color, 5, 10);

		vTaskDelay( 30 / portTICK_RATE_MS );

	}

	ball.color = pon_color(0,0,0);
	ball.posx = 100;
	ball.posy = 100;

	plot_ball(ball);

	vTaskDelete(NULL);

	return;
}

void print_menu(){

	print("  -- MENU --\n\n");
	print(" - 1) Local Game\n");
	print(" - 2) Search Player\n");
	print(" - 3) Game Requests\n\n");
	print("Select an option:  ");

	return;
}

void local_play(){

	int c;

	c = 1;

	sys_thread_new("local_game", (void(*)(void*))game_thread, &c, THREAD_STACKSIZE, DEFAULT_THREAD_PRIO);

	while(c != 0){
		vTaskDelay( 500 / portTICK_RATE_MS );
	}

	return;
}

void serv_play(){

	int c, m;

	c = 0;

	print("\n Connect to client? 1 Yes - 0 No \n Enter action (1-0): ");
	scanf("%d", &m);

	if(m == 0){
		con_clien(2);
	}else{

		con_clien(1);

		c = 3;

		sys_thread_new("local_game", (void(*)(void*))game_thread, &c, THREAD_STACKSIZE, DEFAULT_THREAD_PRIO);

		while(c != 0){
			vTaskDelay( 500 / portTICK_RATE_MS );
		}
	}

	return;
}

//====================================================


int main_thread()
{

	int menu;
	int sele;

	lwip_init();
	init_gpios();

	sys_thread_new("network_thread", (void(*)(void*))server_tx_data, 0, THREAD_STACKSIZE, DEFAULT_THREAD_PRIO);

	while(1){

		sys_thread_new("demo", (void(*)(void*))demo, 0, THREAD_STACKSIZE, DEFAULT_THREAD_PRIO);

		menu = 0;

		do{
			print_menu();

			scanf("%d", &sele);

			switch(sele){
			case 1:
				print("\n -- Local Play -- \n");
				bucle = 0;
				local_play();
				menu = 1;
				break;
			case 2:
				print("\n -- Search Player -- \n");
				bucle = 0;
				client_rx_data();
				menu = 1;
				break;
			case 3:
				print("\n -- Players -- \n");
				bucle = 0;
				serv_play();
				menu = 1;
				break;
			default:
				print("\n -- Incorrect Option -- \n\n");
				break;
			}

		}while(menu == 0);

	}

	vTaskDelete(NULL);

	return 0;
}

int main()
{

	print("\t--- INICIANDO ---\n\n");

	sys_thread_new("main_thread", (void(*)(void*))main_thread, 0, THREAD_STACKSIZE, 0);

	vTaskStartScheduler();

	while(1);
	return 0;
}

//====================================================

void vApplicationMallocFailedHook( void )
{
	/* vApplicationMallocFailedHook() will only be called if
	configUSE_MALLOC_FAILED_HOOK is set to 1 in FreeRTOSConfig.h.  It is a hook
	function that will get called if a call to pvPortMalloc() fails.
	pvPortMalloc() is called internally by the kernel whenever a task, queue or
	semaphore is created.  It is also called by various parts of the demo
	application.  If heap_1.c or heap_2.c are used, then the size of the heap
	available to pvPortMalloc() is defined by configTOTAL_HEAP_SIZE in
	FreeRTOSConfig.h, and the xPortGetFreeHeapSize() API function can be used
	to query the size of free heap space that remains (although it does not
	provide information on how the remaining heap might be fragmented). */
	xil_printf("Memory Allocation Error\r\n");
	taskDISABLE_INTERRUPTS();
	for( ;; );
}
/*-----------------------------------------------------------*/

void vApplicationStackOverflowHook( xTaskHandle *pxTask, signed char *pcTaskName )
{
	( void ) pcTaskName;
	( void ) pxTask;

	/* vApplicationStackOverflowHook() will only be called if
	configCHECK_FOR_STACK_OVERFLOW is set to either 1 or 2.  The handle and name
	of the offending task will be passed into the hook function via its
	parameters.  However, when a stack has overflowed, it is possible that the
	parameters will have been corrupted, in which case the pxCurrentTCB variable
	can be inspected directly. */
	xil_printf("Stack Overflow in %s\r\n", pcTaskName);
	taskDISABLE_INTERRUPTS();
	for( ;; );
}
void vApplicationSetupHardware( void )
{

}
