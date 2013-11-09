#include "miso_car_lib.h"

void  change_course(){

	int n = 0;
	n = mDistance();
	distance_set(1300);
	speed_set(2000);
	winker_light(EMERGENCY);
	forward_dis();
	while(mDistance() - n < 20){}
	turn_set(DM_STRAIGHT);
	while(mDistance() - n < 320){}
	turn_set(DM_ANGLE_MIN);
	while(mDistance() - n < 820){}
	turn_set(DM_STRAIGHT);
	while(mDistance() - n < 3220){}
	turn_set(DM_ANGLE_MAX);
	while(mDistance() - n < 4020){}
	turn_set(DM_STRAIGHT);
	speed_set(1000);
	winker_light(OFF);
}


void traffic_drive(int flag){
	int n = 0;

	switch(flag){
		case IF_SG_STOP:
			stop();
			buzzer_on();
			g_drive_flag = DF_STOP;
			break;

		case IF_SG_LEFT:
			n = mDistance();
			distance_set(1200);
			speed_set(1000);	
			forward_dis();
			while(mDistance() - n < 1720){}
			turn_set(DM_ANGLE_MAX);
			while(mDistance() - n < 3800){}
			turn_straight();
			while(mDistance() - n < 4600){}
			stop();
			break;

		case IF_SG_RIGHT:
			n = mDistance();
			distance_set(1200);
			speed_set(1000);
			forward_dis();
			while(mDistance() - n < 1720){}
			turn_set(DM_ANGLE_MIN);
			while(mDistance() - n < 3800){}
			turn_straight();
			while(mDistance() - n < 4600){}
			stop();
			break;
	}
}

void direct_test()
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
				idata = line_check(cm_handle,idata); // get image data 
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
			default:
				break;
		}
	}
}
