#include "miso_car_lib.h"

#ifndef _DRIVE_MODULES_H_
#define _DRIVE_MODULES_H_


void  change_course(){

	int n = mDistance();
	usleep(10000);
	turn_set(DM_STRAIGHT);
	usleep(10000);
	speed_set(1000);
	winker_light(EMERGENCY);
	usleep(10000);
	distance_set(1300);
	forward_dis();
	usleep(10000);
	speed_set(2000);
	usleep(10000);
	distance_set(1300);
	usleep(10000);
	forward_dis();
	
	usleep(10000);
	while(mDistance() - n < 320){}
	turn_set(DM_ANGLE_MIN);
	while(mDistance() - n < 820){}
	turn_set(DM_STRAIGHT);
	while(mDistance() - n < 3620){}
	turn_set(DM_ANGLE_MAX);
	while(mDistance() - n < 4120){}
	turn_set(DM_STRAIGHT);
	speed_set(1500);
	winker_light(OFF);
	usleep(10000);
}


void traffic_drive(int flag){
	int n = 0;

	switch(flag){
		case IF_SG_STOP:
			stop();
			g_drive_flag = DF_STOP;
			break;

		case IF_SG_LEFT:
			n = mDistance();
			distance_set(1200);
			usleep(10000);
			speed_set(1000);	
			usleep(10000);
			forward_dis();
			while(mDistance() - n < 1720){}
			turn_set(DM_ANGLE_MAX);
			while(mDistance() - n < 3800){}
			turn_straight();
			while(mDistance() - n < 4600){}
			stop();
			g_drive_flag = DF_END;
			break;

		case IF_SG_RIGHT:
			n = mDistance();
			distance_set(1200);
			usleep(10000);
			speed_set(1000);
			usleep(10000);
			forward_dis();
			while(mDistance() - n < 1720){}
			turn_set(DM_ANGLE_MIN);
			while(mDistance() - n < 3800){}
			turn_straight();
			while(mDistance() - n < 4600){}
			stop();
			g_drive_flag = DF_END;
			break;
	}
	distance_reset();
}

void drive_test()
{
	struct image_data* idata;
	while(TRUE)
	{
		char input;
		int n;

		printf("0.get image, 1. turn left, 2. turn right, 3. set straight, 4. go, 5. mack, 6. traffic left, 7. traffic right \n");
		scanf("%c",&input);

		switch(input)
		{
			case '0':

				idata = cm_img_process(); // get image data 
				printf("idata flag : %d\n", idata->flag);
				break;	
			case '1':	
				g_angle += 100;
				turn_set(g_angle);
				break;
			case '2':
				g_angle -= 100;
				turn_set(g_angle);
				break;
			case '3':
				turn_straight();
				break;
			case '4':
				distance_set(50);
				forward_dis();
				break;
			case '5':
				distance_set(50);
				backward_dis();
				break;
			case '6':
				n = mDistance();
				distance_set(1200);
				speed_set(1000);	
				forward_dis();
				while(mDistance() - n < 1720){}
				turn_set(DM_ANGLE_MAX);
				while(mDistance() - n < 3800){}
				turn_straight();
				while(mDistance() - n < 4600){}
				break;
			case '7':
				n = mDistance();
				distance_set(1200);
				speed_set(1000);	
				forward_dis();
				while(mDistance() - n < 1720){}
				turn_set(DM_ANGLE_MIN);
				while(mDistance() - n < 3800){}
				turn_straight();
				while(mDistance() - n < 4600){}
				break;
			case '8':
				change_course();
				break;
			case '9':
				parking(IF_PARK_V);
				break;
			case 'a':
				scanf("%d",&g_park_dis);
				parking(IF_PARK_H);
				break;
			default:
				break;
		}
	}
}

#endif

