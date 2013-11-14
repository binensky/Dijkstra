#ifndef __SENSOR_HANDLER_H__
#define __SENSOR_HANDLER_H__

#include "miso_car_lib.h"
#include "stdio.h"

#define ON_STOP_LINE 127

void* sensor_handler(void* data)
{
	char ret;
	//sudden_stop(250);
	int status = 0;

	while(TRUE)
	{
		line_start();
		buf[0] = 0xb2;
		write(uart_fd,&buf[0],1);
		read(uart_fd,&read_buf[0],2);
		ret = read_buf[0];
		//printf("ret : %d\n",ret);
		if( ret == 127 )
		{
			g_drive_flag = DF_STOP;
			stop();
			sleep(1);
			//sudden_stop(250);
			if(status == 0){
			//	printf("buzzor!!!\n");
				buzzer_on();
				status = 1;
			}
		}
		line_stop();
		usleep(2000);
	}
}

#endif


