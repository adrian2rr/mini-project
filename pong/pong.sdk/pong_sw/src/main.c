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

#include "config.h"
#include "utils.h"
#include "plot.h"
// Only needed for SDK versions before 2015
//#include "xutil.h"
  
//====================================================

struct netif server_netif;


//====================================================


void network_thread()
{
    struct netif *netif;
    struct ip_addr ipaddr, netmask, gw;
    /* the mac address of the board. this should be unique per board */
    unsigned char mac_ethernet_address[] = { 0x00, 0x0a, 0x35, 0x00, 0x01, 0x02 };

    netif = &server_netif;


    /* initliaze IP addresses to be used */
    IP4_ADDR(&ipaddr,  192, 168,   1, 11);
    IP4_ADDR(&netmask, 255, 255, 255,  0);
    IP4_ADDR(&gw,      192, 168,   1,  1);

    /* Add network interface to the netif_list, and set it as default */
    if (!xemac_add(netif, &ipaddr, &netmask, &gw, mac_ethernet_address, PLATFORM_EMAC_BASEADDR)) {
        printf("Error adding N/W interface\r\n");
        return;
    }
    netif_set_default(netif);

    /* specify that the network if is up */
    netif_set_up(netif);

    /* start packet receive thread - required for lwIP operation */
    sys_thread_new("xemacif_input_thread", (void(*)(void*))xemacif_input_thread, netif,
            THREAD_STACKSIZE,
            DEFAULT_THREAD_PRIO);

    return;
}

void game_thread();


//====================================================


int main_thread()
{

	print("Inicio\n");
	RGB color;
	BALL ball;

	lwip_init();
	init_gpios();
	//network_thread();

	print("Plot\n");

	color = pon_color(0,0,0);
	plot_back(color);

	print("Plot frame\n");

	color = pon_color(144,195,0);
	plot_frame(color);

	plot_red(color, 7, 20);

	sys_thread_new("game_thread", (void(*)(void*))game_thread, 0,
						THREAD_STACKSIZE,
					    DEFAULT_THREAD_PRIO);

	vTaskDelete(NULL);

	return 0;
}

int main()
{

	sys_thread_new("main_thread", (void(*)(void*))main_thread, 0,
					THREAD_STACKSIZE,
				    DEFAULT_THREAD_PRIO);

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
