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
#include "include/key_handler.h"
#include "include/sensor_handler.h"
#include "include/distance_check.h"

pthread_t thread[3];	// 0:key handling , 1:sensor handling, 2:distance_check

struct image_data* img_head;
struct image_data* img_it;
struct image_data* img_tail;

void init_drive(void);
void drive(void);
void go_ahead(void);
void direct_test(void);

int main(void)
{
	cm_handle = init_camera();

	car_connect();
	pthread_create(&thread[0],NULL,key_handler,NULL);
	//pthread_create(&thread[1],NULL,sensor_handler,NULL);
	//pthread_create(&thread[2],NULL,distance_check,NULL);

	drive();
	//direct_test();

	pthread_join(thread[0],NULL);
	//pthread_join(thread[1],NULL);
	//pthread_join(thread[2],NULL);
	return 0;
}

void drive(void)
{
	struct image_data* idata;
	struct p_point mid_bot, dest;
	double gradient_left, gradient_right;
	int angle,input,intercept_left,intercept_right,dest_angle, height, temp_flag;
	
	init_drive();

	while(TRUE)
	{
		if(g_drive_flag == DF_VPARK || g_drive_flag == DF_PPARK)
		{
			stop();
			while(g_drive_flag != DF_DRIVE);
		}
			

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
				g_drive_flag = DF_STOP;
				break;

			case IF_LEFT:
#ifdef DRIVE_DEBUG
				printf("img angle %d\n", idata->angle[LEFT]);
#endif
				if(idata->angle[LEFT] < 90)
				{
					turn_set(DM_ANGLE_MAX);
#ifdef DRIVE
					if(g_drive_flag == DF_DRIVE)
						go_ahead();
#endif
					//speed_set(1000);
					break;
				}

				gradient_left = tan( (double)idata->angle[LEFT] * PI /180);
				intercept_left = idata->bot[LEFT].y - idata->bot[LEFT].x * gradient_left;

#ifdef DRIVE_DEBUG
				printf("left line : y =  %lfx + %d\n", gradient_left,intercept_left);
#endif
				
				height = gradient_left * MIDWIDTH + intercept_left;

#ifdef DRIVE_DEBUG
				printf("height in main : %d\n", height);
#endif

				// set temp flag
				if(idata->mid_flag == MID_STRAIGHT )
				{
					if(height > CUTLINE + 20)
						temp_flag = MID_STRAIGHT;
					else
					{
						if(idata->prev->mid_flag == MID_STRAIGHT)
							temp_flag = MID_STRAIGHT;
						else
							temp_flag = MID_CURVE_STRAIGHT;
					}
				}
				else if(idata->mid_flag == MID_CURVE_STRAIGHT)
				{
					if(idata->prev->mid_flag == MID_STRAIGHT)
						temp_flag = MID_STRAIGHT;
					else
						temp_flag = MID_CURVE_STRAIGHT;
				}
				else if(idata->mid_flag == MID_CURVE)
				{
						temp_flag = MID_CURVE;
				}

				printf("temp flag : %d \n",temp_flag);
				// set angle
				if(temp_flag == MID_STRAIGHT)
				{
					//speed_set(2000);
					idata->mid_flag = MID_STRAIGHT;
					turn_straight();
				}
				else
				{
					//speed_set(1000);
					mid_bot.y = 0;
					mid_bot.x = MIDWIDTH;
					dest.y = temp_flag == MID_CURVE ? DEST_HEIGHT : DEST_HEIGHT+60;
					dest.x = (int)((dest.y - intercept_left) / gradient_left);
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
#ifdef DRIVE
				if(g_drive_flag == DF_DRIVE)
					go_ahead();
#endif
				break;

			case IF_RIGHT:
#ifdef DRIVE_DEBUG
				printf("img angle %d\n", idata->angle[RIGHT]);
#endif
				if(idata->angle[RIGHT] > 90)
				{
					//speed_set(1000);
					turn_set(DM_ANGLE_MIN);
#ifdef DRIVE
				if(g_drive_flag == DF_DRIVE)
					go_ahead();
#endif
					break;
				}

				gradient_right = tan( (double)idata->angle[RIGHT] *PI /180);
				intercept_right = idata->bot[RIGHT].y - idata->bot[RIGHT].x * gradient_right ;
#ifdef DRIVE_DEBUG
				printf("right line : y =  %lfx + %d\n", gradient_right,intercept_right);
#endif
				height = gradient_right * MIDWIDTH + intercept_right;

#ifdef DRIVE_DEBUG
				printf("height in main : %d\n", height);
#endif
				// set temp flag
				if(idata->mid_flag == MID_STRAIGHT )
				{
					if(height > CUTLINE + 20)
						temp_flag = MID_STRAIGHT;
					else
					{
						if(idata->prev->mid_flag == MID_STRAIGHT)
							temp_flag = MID_STRAIGHT;
						else
							temp_flag = MID_CURVE_STRAIGHT;
					}
				}
				else if(idata->mid_flag == MID_CURVE_STRAIGHT)
				{
					if(idata->prev->mid_flag == MID_STRAIGHT)
						temp_flag = MID_STRAIGHT;
					else
						temp_flag = MID_CURVE_STRAIGHT;
				}
				else if(idata->mid_flag == MID_CURVE)
				{
						temp_flag = MID_CURVE;
				}

				printf("temp flag : %d \n",temp_flag);
				// set angle
				if(temp_flag == MID_STRAIGHT)
				{
					//speed_set(2000);
					idata->mid_flag = MID_STRAIGHT;
					turn_straight();
				}
				else
				{
					//speed_set(1000);
					mid_bot.y = 0;
					mid_bot.x = MIDWIDTH;
					dest.y = temp_flag == MID_CURVE ? DEST_HEIGHT : DEST_HEIGHT+60;
					dest.x = (int)((dest.y - intercept_right) / gradient_right);
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

#ifdef DRIVE
				if(g_drive_flag == DF_DRIVE)
					go_ahead();
#endif
				break;

			case IF_BOTH:
#ifdef DRIVE_DEBUG
				printf("img angle left : %d right : %d\n", idata->angle[LEFT], idata->angle[RIGHT]);
#endif
				// 100 까지 찾아서 양쪽선이 나오면 사실상 직진
				turn_straight();				
				//speed_set(2000);
/*
				gradient_left = tan( (double)idata->angle[LEFT] *PI /180);
				intercept_left = idata->bot[LEFT].y - idata->bot[LEFT].x * gradient_left;
				gradient_right = tan( (double)idata->angle[RIGHT] *PI /180);
				intercept_right = idata->bot[RIGHT].y - idata->bot[RIGHT].x * gradient_right ;

#ifdef DRIVE_DEBUG
				printf("left = %.2lfa + %d", gradient_left, intercept_left);
				printf("right = %.2lfa + %d", gradient_right, intercept_right);
#endif

				dest.x = (intercept_left - intercept_right) / (gradient_right - gradient_left);
				dest.y = gradient_left * dest.x + intercept_left;

#ifdef DRIVE_DEBUG
				printf("dest (%d, %d)\n", dest.x, dest.y);
#endif

				dest_angle = get_angle(mid_bot, dest);

#ifdef DRIVE_DEBUG
				printf("dest angle : %d", dest_angle);
#endif
				set_angle(dest_angle);
*/
#ifdef DRIVE
				if(g_drive_flag == DF_DRIVE)
					go_ahead();
#endif

				break;

			case IF_STRAIGHT:
				//speed_set(2000);
				turn_straight();
#ifdef DRIVE
				if(g_drive_flag == DF_DRIVE)
					go_ahead();
#endif
				break;

			case IF_OUTLINE:
				//speed_set(1000);
				if(g_angle < DM_STRAIGHT)
					turn_set(DM_ANGLE_MIN);
				else
					turn_set(DM_ANGLE_MAX);

#ifdef DRIVE
				if(g_drive_flag == DF_DRIVE)
					go_ahead();
#endif
				break;

			case IF_CL_LEFT:
				break;
			case IF_CL_RIGHT:
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
	}
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
			while(mDistance() - n < 1720){//printf("%d\n", mDistance() - n);
			}
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
	turn_straight();
	usleep(2000);
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

void go_ahead()
{
	distance_set(500);	
	forward_dis();
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
				distance_set(150);
				forward_dis();
				break;
			case '5':
				distance_set(150);
				backward_dis();
				break;
			case '6':
				n = mDistance();
				distance_set(1200);
				speed_set(1000);	
				forward_dis();
				while(mDistance() - n < 1720){printf("%d\n", mDistance() - n);}
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
				while(mDistance() - n < 1720){printf("%d\n", mDistance() - n);}
				turn_set(DM_ANGLE_MIN);
				while(mDistance() - n < 3800){}
				turn_straight();
				while(mDistance() - n < 4600){}
				break;
			default:
				break;
		}
	}
}
