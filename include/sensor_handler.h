#ifndef __SENSOR_HANDLER_H__
#define __SENSOR_HANDLER_H__

#include "miso_car_lib.h"
#include "stdio.h"

#define ON_STOP_LINE 127

void* sensor_handler(void* data)
{
	char ret = 0;
	while(TRUE)
	{
		line_start();
		usleep(10000);
		buf[0] = 0xb2;
		write(uart_fd,&buf[0],1);
		read(uart_fd,&read_buf[0],2);
		ret = read_buf[0];
	
		if( ret == ON_STOP_LINE )
		{	

			printf("sensor >>>>>>>>>>>>>>>>>>>>>>!!!!!!!!!!!!!!!!!!!!!! ret : %d\n",ret);
			g_drive_flag = DF_STOP;
			usleep(10000);
			stop();
			usleep(10000);
			buzzer_on();
			usleep(10000);
	
			break;
		}

	}
			line_stop();
}

#endif
