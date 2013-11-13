
#ifndef __KEY_HANDLER_H__
#define __KEY_HANDLER_H__

#include "miso_car_lib.h" 
#include "miso_values.h" 	// for cm_handle

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
				buzzer_on();
				usleep(500000);
				stop();
				exit_camera(cm_handle);
				exit(0);
				break;
			case KEY2:
				// START CM_MODE

				buzzer_on();
				usleep(500000);
				buzzer_on();
				usleep(500000);

				break;
			case KEY3:
				// START AI_MODE
				buzzer_on();
				usleep(500000);
				buzzer_on();
				usleep(500000);
				buzzer_on();
				usleep(500000);
				// start
				break;
			default:
				break;
		}
	}
}

#endif
