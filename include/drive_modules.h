#include "miso_car_lib.h"

#ifndef _DRIVE_MODULES_H_
#define _DRIVE_MODULES_H_

void  change_course(){

	int n;
	int a =300, b = 320, c = 2500, d = 150;



	a *= 1000;
	b *= 1000;
	c *= 1000;
	d *= 1000;

	usleep(10000);
	turn_set(DM_STRAIGHT);
	usleep(10000);
	speed_set(2500);
	winker_light(EMERGENCY);
	usleep(10000);
	distance_set(3000);
	forward_dis();
	usleep(10000);
	//speed_set(2000);
//	usleep(10000);
	//distance_set(1300);
//	usleep(10000);
	//forward_dis();
	
	usleep(10000);
	n = getCurrentTimeMillis();
	while(getCurrentTimeMillis() - n < a){}
	turn_set(DM_ANGLE_MIN+200);
	
	n = getCurrentTimeMillis();
	while(getCurrentTimeMillis() - n < b){}
	turn_set(DM_STRAIGHT);
	n = getCurrentTimeMillis();
	while(getCurrentTimeMillis() - n < c){}
	turn_set(DM_ANGLE_MAX-100);
	n = getCurrentTimeMillis();
	while(getCurrentTimeMillis() - n < d){}
	turn_set(DM_STRAIGHT);
	speed_set(START_SPEED);
	winker_light(OFF);
	usleep(10000);
}


void traffic_drive(int flag){
	int n = 0;

	int a = 1300, b = 1600 , c = 3000;
	a *= 1000;
	b *= 1000;
	c *= 1000;
	switch(flag){
		case IF_SG_STOP:
			stop();
			usleep(10000);
			g_drive_flag = DF_STOP;
			break;

		case IF_SG_LEFT:
			distance_set(1200);
			usleep(10000);
			speed_set(1500);	
			usleep(10000);
			forward_dis();
			n = getCurrentTimeMillis();
			while(getCurrentTimeMillis() - n < a){}
			turn_set(DM_ANGLE_MAX);
			n = getCurrentTimeMillis();
			while(getCurrentTimeMillis() - n < b){}
			turn_straight();
			n = getCurrentTimeMillis();
			while(getCurrentTimeMillis() - n < c){}
			stop();
			usleep(10000);
			g_drive_flag = DF_END;
			break;

		case IF_SG_RIGHT:
			n = mDistance();
			distance_set(1200);
			usleep(10000);
			speed_set(1500);
			usleep(10000);
			forward_dis();
			n = getCurrentTimeMillis();
			while(getCurrentTimeMillis() - n < a){}
			turn_set(DM_ANGLE_MIN);
			n = getCurrentTimeMillis();
			while(getCurrentTimeMillis() - n < b){}
			turn_straight();
			n = getCurrentTimeMillis();
			while(getCurrentTimeMillis() - n < c){}
			stop();
			usleep(10000);
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

		printf("0.get image, 1. turn left, 2. turn right, 3. set straight, 4. go, 5. mack, 6. traffic left, 7. traffic right, 8. change course\n");
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
				traffic_drive(IF_SG_LEFT);
				break;
			case '7':
				traffic_drive(IF_SG_RIGHT);
				break;
			case '8':
				change_course();
				break;
			case '9':
				parking(IF_PARK_V);
				//park_vertical();
				break;
			case 'b':
				parking(IF_PARK_H);
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

