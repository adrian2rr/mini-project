#include "xparameters.h"
#include "xgpio.h"


#include <stdlib.h>

#include <stdio.h>
#include "xparameters.h"
#include "lwipopts.h"

#include "platform_config.h"

#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/init.h"
#include "netif/xadapter.h"
#include "lwip/dhcp.h"
#include "config_apps.h"
#include "task.h"
#include "portmacro.h"
#include "xil_printf.h"

#include "utils.h"
// Only needed for SDK versions before 2015
//#include "xutil.h"


//====================================================

void print_ip(char *msg, struct ip_addr *ip)
{
    print(msg);
    xil_printf("%d.%d.%d.%d\r\n", ip4_addr1(ip), ip4_addr2(ip),
            ip4_addr3(ip), ip4_addr4(ip));
}

void print_ip_settings(struct ip_addr *ip, struct ip_addr *mask, struct ip_addr *gw)
{

    print_ip("Board IP: ", ip);
    print_ip("Netmask : ", mask);
    print_ip("Gateway : ", gw);
}

struct netif server_netif;

void network_thread()
{
    struct netif *netif;
    struct ip_addr ipaddr, netmask, gw;
    /* the mac address of the board. this should be unique per board */
    unsigned char mac_ethernet_address[] = { 0x00, 0x0a, 0x35, 0x00, 0x01, 0x02 };

    netif = &server_netif;


    /* initliaze IP addresses to be used */
    IP4_ADDR(&ipaddr,  192, 168,   1, 10);
    IP4_ADDR(&netmask, 255, 255, 255,  0);
    IP4_ADDR(&gw,      192, 168,   1,  1);


    /* print out IP settings of the board */
    print("\r\n\r\n");
    print("-----lwIP Socket Mode Demo Application ------\r\n");


    print_ip_settings(&ipaddr, &netmask, &gw);
    /* print all application headers */


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


void tx_data(void *);
void rx_data(void *);

//====================================================


int main_thread()
{

	print("HOLA MUNDO\n");
	lwip_init();
	init_butled();
	network_thread();

	sys_thread_new("tx_thread", (void(*)(void*))tx_data, 0,
	                THREAD_STACKSIZE,
	                DEFAULT_THREAD_PRIO);

	sys_thread_new("rx_thread", (void(*)(void*))rx_data, 0,
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

