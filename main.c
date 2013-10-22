//#define DEBUG
//#define DRIVE_DEBUG
//#define MID_LINE_DEBUG
#define DRIVE
#define TRACE

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
	double gradient;
	int angle,input,intercept;
	init_drive();

	while(TRUE)

	{			
		idata = line_check(cm_handle); // get image data 
	
		idata->prev = img_it;
		idata->next = img_it->next;
		img_it->next =idata;
		img_it->next->prev =idata;
		img_it=idata;

		switch(idata->flag)
		{			
			case IF_STOP:
				printf("===================================IF_STOP\n");
				stop();
				g_drive_flag = DF_STOP;
				break;

			case IF_LEFT:
#ifdef DRIVE_DEBUG
				printf("img angle %d\n", idata->angle);
#endif
				if(idata->angle[LEFT] < 90)
				{
					turn_set(2200);
					break;
				}
				gradient = tan( (double)idata->angle[LEFT] *PI /180);
				intercept = -320 * gradient + idata->dist[LEFT];
				mid_bot.y = 0;
				mid_bot.x = MIDWIDTH;
				dest.y = 100;
				dest.x = (int)((dest.y - intercept) / gradient);

				set_angle(get_angle(mid_bot, dest));
				break;

			case IF_RIGHT:
#ifdef DRIVE_DEBUG
				printf("img angle %d\n", idata->angle);
#endif
				if(idata->angle[RIGHT] > 90)
				{
					turn_set(800);
					break;
				}

				gradient = tan( (double)idata->angle[RIGHT] *PI /180);
				intercept = idata->dist[RIGHT];
				mid_bot.y = 0;
				mid_bot.x = MIDWIDTH;
				dest.y = 100;
				dest.x = (int)((dest.y - intercept) / gradient);

				set_angle(get_angle(mid_bot, dest));
				break;

			case IF_BOTH:
#ifdef DRIVE_DEBUG
				printf("img angle %d\n", idata->angle);
#endif

				// 선 두개가 만나는 지점을 dest로
				// 일단 직진, 수정하기
				turn_straight();

			case IF_STRAIGHT:
				turn_straight();
				break;

			case IF_OUTLINE:
				if(g_angle < 1533)
					turn_set(800);
				else
					turn_set(2200);

				break;

			case IF_CL_LEFT:
			case IF_CL_RIGHT:

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
		distance_set(500);		
		forward_dis();
#endif
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
			turn_set(2200);
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
			turn_set(800);
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

	img_head = (struct img_data*)malloc(sizeof(struct image_data));
	img_tail = (struct img_data*)malloc(sizeof(struct image_data));

	img_head->next = img_tail;
	img_head->prev = NULL;
	img_tail->prev = img_head;
	img_tail->next = NULL;
	img_it = img_head;
}

void direct_test()
{
	struct image_data* idata;
	init_drive();
	while(TRUE)
	{
		char input;
		int n;

		printf("0.get image, 1. turn left, 2. turn right, 3. set straight, 4. go, 5. back, 6. traffic left, 7. traffic right \n");
		scanf("%c",&input);

		switch(input)
		{
			case '0':
				idata = line_check(cm_handle); // get image data 
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
				backward_dis(0);
				break;
			case '6':
				n = mDistance();
				distance_set(1200);
				speed_set(1000);	
				forward_dis();
				while(mDistance() - n < 1720){printf("%d\n", mDistance() - n);}
				turn_set(2200);
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
				turn_set(800);
				while(mDistance() - n < 3800){}
				turn_straight();
				while(mDistance() - n < 4600){}
				break;
			default:
				break;
		}
	}
}
