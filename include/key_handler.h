
#ifndef __KEY_HANDLER_H__
#define __KEY_HANDLER_H__

#include "miso_car_lib.h" 
#include "miso_camera.h"

#define KEY1 1
#define KEY2 2
#define KEY3 3

char keyDev[] ="/dev/KEYPAD";
int keyFD = -1;
char keyState[3]={OFF,OFF,OFF};

void* key_handler(void* data)
{
	// connect car 
	unsigned char buf,read_key;
	int key;

	// open key interface. 
	if((keyFD = open(keyDev,O_RDONLY))<0)
	{
		perror("Cannot open /dev/key!");
		buzzer_on();
	}

	// buzy wait to pushing key. 
	while(1){
		read_key= read(keyFD, &buf,sizeof(buf));
		key = read_key;

		switch(key)
		{
			case KEY1:
				// STOP / RESET
				if( g_drive_flag == DF_DRIVE || g_drive_flag == DF_SPEED_DOWN )
				{
					g_drive_flag = DF_READY;
				
					buzzer_on();
					usleep(500000);

					stop();
				}else if( g_drive_flag == DF_READY){
		
					// write image data into file. 
					fwrite_data(d_data);

					g_drive_flag = DF_END;
			
					buzzer_on();
					usleep(500000);

					stop();
					
					struct pxa_camera* camera = (struct pxa_camera*)cm_handle;

					while(camera->ref_count > 0){
						camera_release_frame(cm_handle,vidbuf);
					}
					exit_camera(cm_handle);
					exit(0);
				}else{}
				
				break;
			case KEY2:
				// START CM_MODE
				if( g_drive_flag == DF_READY)
				{
					g_drive_flag = DF_DRIVE;
					g_drive_mode = CM_MODE;
					buzzer_on();
					usleep(500000);
					buzzer_on();
					usleep(500000);
				}
				break;

			case KEY3:
				if( g_drive_flag == DF_READY)
				{
					// get image data from file. 
					fread_data(d_data);

					g_drive_flag = DF_DRIVE;
					g_drive_mode = AI_MODE;
			
					buzzer_on();
					usleep(500000);
					buzzer_on();
					usleep(500000);
					buzzer_on();
					usleep(500000);
					// start
				}
				break;

			default:
				break;
		}
	}
}

#endif
