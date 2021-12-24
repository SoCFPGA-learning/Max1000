/*=====================================
 * This example is a very simple memory test
 *======================================*/

#include "system.h" // decodes base-addresses
#include <stdio.h> // provides printf()-function
#include <io.h> // provides IORD()-function
#include "altera_avalon_pio_regs.h"
#include "alt_types.h"

//#define led_time 131072

int main()
{
//  int count, value;
	unsigned long i = 0;
	int led_on = 1;
	int led_direction = 0;
	int led_time[5];
	led_time[0] =  4096;
	led_time[1] =  8192;
	led_time[2] = 16384;
	led_time[3] = 32768;
	led_time[4] = 65536;
	int led_time_state = 0;
	int led_time_actual = led_time[led_time_state];
	int key_state_old = 1;
	int key_state_new;




	printf("Hello from Nios II!\n");

	while(1)
	{
		i++;
		if (i == led_time_actual)
		{
			IOWR_ALTERA_AVALON_PIO_DATA(LED_PIO_BASE, led_on);
			if (led_direction == 0)
			{
				led_on = led_on << 1;
				if (led_on >= 129)
				{
					led_on = 64;
					led_direction = 1;
				}
			}
			else
			{
				led_on = led_on >> 1;
				if (led_on <= 0)
				{
					led_on = 2;
					led_direction = 0;
				}
			}

			i = 0;
		}
		key_state_new = IORD_ALTERA_AVALON_PIO_DATA(KEY_PIO_BASE);
		if (key_state_new != key_state_old)
		{
			key_state_old = key_state_new;
			if (key_state_new == 0)
			{
				led_time_state++;
				if (led_time_state == 5)
				{
					led_time_state = 0;
				}
				led_time_actual = led_time[led_time_state];
				i = 0;
				printf("%02x\n",led_time_actual);
			}
		}
	}
	return 0;
}
