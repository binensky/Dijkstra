//#define DEBUG
//#define DRIVE_DEBUG
//#define MID_LINE_DEBUG
#define DRIVE
//#define TRACE

#include <stdio.h>
#include <pthread.h>

#include "include/miso_values.h"
#include "include/miso_car_lib.h"
#include "include/miso_camera.h"
#include "include/sensor_handler.h"
#include "include/distance_check.h"

pthread_t thread[3];	// 0:key handling , 1:sensor handling, 2:distance_check

struct image_data* img_head;
struct image_data* img_it;
struct image_data* img_tail;

void init_drive(void);
void drive(void);
void key_handling();
void drive_turn(struct image_data* idata, double gradient, int intercept, int height);
void direct_test(void);
void change_course();


int main(void)
{
	cm_handle = init_camera();

	car_connect();
	pthread_create(&thread[1],NULL,sensor_handler,NULL);
	//pthread_create(&thread[2],NULL,distance_check,NULL);

	drive();
//	direct_test();

	pthread_join(thread[1],NULL);
	//pthread_join(thread[2],NULL);
	return 0;
}

void drive(void)
{
	struct image_data* idata;
	double gradient;
	int angle,input,intercept, height;
	
	init_drive();
	
	while(TRUE)
	{
		// check key down 
//		key_handling();
		idata = line_check(cm_handle); // get image data 
	
		idata->prev = img_it;
		idata->next = img_it->next;
		img_it->next->prev =idata;
		img_it->next =idata;
		img_it=idata;

		printf("idata flag %d \n",idata->flag );  
		
		

		switch(idata->flag)
		{			
			case IF_STOP:
				stop();
				idata->mid_flag = MID_STRAIGHT;
				//g_drive_flag = DF_STOP;
				break;

			case IF_LEFT:
#ifdef DRIVE_DEBUG
				printf("img angle %d\n", idata->angle[LEFT]);
#endif
				if(idata->prev->flag == IF_RIGHT )
				{
					idata->prev->mid_flag = MID_STRAIGHT;
				}

				if(idata->angle[LEFT] < 90)
				{
					turn_set(DM_ANGLE_MAX);
					speed_set(1000);
					break;
				}

				gradient = tan( (double)idata->angle[LEFT] * PI /180);
				intercept = idata->bot[LEFT].y - idata->bot[LEFT].x * gradient;
#ifdef DRIVE_DEBUG
				printf("left line : y =  %lfx + %d\n", gradient,intercept);
#endif
				height = gradient * MIDWIDTH + intercept;

#ifdef DRIVE_DEBUG
				printf("height in main : %d\n", height);
#endif
				drive_turn(idata, gradient, intercept, height);
				break;

			case IF_RIGHT:
#ifdef DRIVE_DEBUG
				printf("img angle %d\n", idata->angle[RIGHT]);
#endif
				if(idata->prev->flag == IF_LEFT ){
					printf("==========right to left\n");
					idata->prev->mid_flag = MID_STRAIGHT;
				}

				if(idata->angle[RIGHT] > 90)
				{
					turn_set(DM_ANGLE_MIN);
					break;
				}

				gradient = tan( (double)idata->angle[RIGHT] *PI /180);
				intercept = idata->bot[RIGHT].y - idata->bot[RIGHT].x * gradient ;
#ifdef DRIVE_DEBUG
				printf("right line : y =  %lfx + %d\n", gradient,intercept);
#endif
				height = gradient * MIDWIDTH + intercept;

#ifdef DRIVE_DEBUG
				printf("height in main : %d\n", height);
#endif
				drive_turn( idata, gradient, intercept, height);
				break;

			case IF_BOTH:
#ifdef DRIVE_DEBUG
				printf("img angle left : %d right : %d\n", idata->angle[LEFT], idata->angle[RIGHT]);
#endif
				turn_straight();				
				break;

			case IF_STRAIGHT:
				turn_straight();
				break;

			case IF_OUTLINE:
				if(g_angle < DM_STRAIGHT)
					turn_set(DM_ANGLE_MIN);
				else if(g_angle > DM_STRAIGHT)
					turn_set(DM_ANGLE_MAX);
				else
					turn_straight();
				break;
				
			case IF_SPEED_DOWN:
				printf("------------------------SPEED DOWN------------------\n");
				speed_set(500);
				break;
			case IF_SPEED_BUMP_ST:
				turn_straight();
				break;
			case IF_CL_LEFT:
				change_course();
				break;
			case IF_CL_RIGHT:
				change_course();
				break;
			case IF_SG_STOP:
				traffic_drive(IF_SG_STOP);
				break;
			case IF_SG_LEFT:
				traffic_drive(IF_SG_LEFT);
				break;
			case IF_SG_RIGHT:
				traffic_drive(IF_SG_RIGHT);
				break;
		}
#ifdef DRIVE
		if(idata->flag != IF_STOP && idata->flag != IF_SG_STOP 
		&& idata->flag  != IF_SG_LEFT && idata->flag !=IF_SG_RIGHT 
		&& idata->flag != IF_CL_LEFT && idata->flag != IF_CL_RIGHT && g_drive_flag != DF_STOP)
		{		
			distance_set(500);	
			forward_dis();
		}
#endif
	}
}
void  change_course(){

	int n = 0;
	n = mDistance();
	distance_set(1400);
	speed_set(2000);
	winker_light(EMERGENCY);
	forward_dis();
	winker_light(EMERGENCY);
	while(mDistance() - n < 20){}
	turn_set(DM_STRAIGHT);
	while(mDistance() - n < 320){}
	turn_set(DM_ANGLE_MIN);
	while(mDistance() - n < 820){}
	turn_set(DM_STRAIGHT);
	while(mDistance() - n < 3420){}
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



void init_drive()
{
#ifdef DRIVE
	sleep(3);
#endif

	if((keyFD = open(keyDev,O_RDONLY))<0)
	{
		perror("Cannot open /dev/key!");
	}
	
	turn_straight();
	usleep(2000);	
	camera_turn_left();
	usleep(1000000);
	camera_turn_right();
	usleep(1000000);
	camera_straight();
	usleep(2000);
	speed_set(1000);
	usleep(2000);
	accel(0x02f);
	usleep(2000);
	reduction(0x2f);
	usleep(2000);
	distance_set(2000);
	usleep(2000);
	dm_speed_set(1);
	line_stop();

	img_head = (struct image_data*)malloc(sizeof(struct image_data));
	img_tail = (struct image_data*)malloc(sizeof(struct image_data));

	img_head->next = img_tail;
	img_head->prev = img_head;
	img_tail->prev = img_head;
	img_tail->next = img_tail;
	img_it = img_head;
	img_head->mid_flag = MID_STRAIGHT;
}

void direct_test()
{
	struct image_data* idata;
	init_drive();
	while(TRUE)
	{
		char input;
		int n;

		printf("0.get image, 1. turn left, 2. turn right, 3. set straight, 4. go, 5. mack, 6. traffic left, 7. traffic right \n");
		scanf("%c",&input);

		switch(input)
		{
			case '0':
				idata = line_check(cm_handle); // get image data 
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

void drive_turn(struct image_data* idata, double gradient, int intercept, int height )
{

	struct p_point mid_bot, dest;
	int temp_flag,dest_angle;
	// set temp flag
	if(idata->mid_flag == MID_STRAIGHT )
	{
		if(height > CUTLINE + 20)
			temp_flag = MID_STRAIGHT;
		else
		{
			if(idata->prev->mid_flag == MID_STRAIGHT || idata->prev->mid_flag == MID_STOP || idata->prev->mid_flag == MID_SPEED_BUMP_ST)
				temp_flag = MID_STRAIGHT;
			else
			{
				printf("prev mid flag : %d\n",idata->prev->mid_flag);
				temp_flag = MID_CURVE_STRAIGHT;
			}
		}
	}
	else if(idata->mid_flag == MID_CURVE_STRAIGHT)
	{
		if(idata->prev->mid_flag == MID_STRAIGHT || idata->prev->mid_flag == MID_STOP || idata->prev->mid_flag == MID_SPEED_BUMP_ST)
			temp_flag = MID_STRAIGHT;
		else
		{
			printf("prev mid flag : %d\n",idata->prev->mid_flag);
			temp_flag = MID_CURVE_STRAIGHT;
		}
	}
	else if(idata->mid_flag == MID_CURVE)
	{
		temp_flag = MID_CURVE;
	}

	printf("temp flag : %d \n",temp_flag);

	if(temp_flag == MID_STRAIGHT)
	{
		idata->mid_flag = MID_STRAIGHT;
		if(idata->flag == IF_RIGHT)
		{
			if(idata->bot[RIGHT].y < 50)
			{
				turn_set(DM_STRAIGHT + (150 - 2*idata->bot[RIGHT].y));
			}
			else
			{
				turn_straight();
			}
		}
		else if(idata->flag == IF_LEFT)
		{
			if(idata->bot[LEFT].y < 50)
			{
				turn_set(DM_STRAIGHT - (150 - 2*idata->bot[LEFT].y));
			}
			else
			{
				turn_straight();
			}
		}
		else
		{
			turn_straight();
		}
	}
	else
	{
		//speed_set(1000);
		mid_bot.y = 0;
		mid_bot.x = MIDWIDTH;
		dest.y = temp_flag == MID_CURVE ? DEST_HEIGHT : DEST_HEIGHT+60;
		dest.x = (int)((dest.y - intercept) / gradient);
#ifdef DRIVE_DEBUG
		printf("dest (%d, %d)\n", dest.x, dest.y);
#endif

		dest_angle = get_angle(mid_bot,dest);
#ifdef DRIVE_DEBUG
		printf("dest angle : %d\n", dest_angle);
#endif
		if(temp_flag == MID_CURVE_STRAIGHT && dest_angle > 85  && dest_angle < 95)
		{
			idata->mid_flag = MID_STRAIGHT;
		}
		else
		{
			idata->mid_flag = MID_CURVE;
		}
		set_angle(dest_angle);
	}

}

void key_handling()
{

	unsigned char buf;
	unsigned char read_key = read(keyFD, &buf,sizeof(buf));
	int key = read_key;

	switch(key)
	{
		case KEY1:
			keyState[0]=~keyState[0];
			break;
		case KEY2:
			keyState[1]=~keyState[1];
			speed_down(1000);
			buzzer_on();
			usleep(500000);
			buzzer_on();
			break;
		case KEY3:
			buzzer_on();
			usleep(500000);
			buzzer_on();
			usleep(500000);
			buzzer_on();
			usleep(500000);
			stop();
			exit_camera(cm_handle);
			exit(0);
			break;
		default:
			break;
	}
}
