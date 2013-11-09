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
#include "include/drive_modules.h"
#include "include/file_lib.h"

pthread_t thread[3];	// 0:key handling , 1:sensor handling, 2:distance_check

void init_drive(void);
void key_handling();
void drive_ai();
void drive_cm();
void drive_md();
void drive_turn(struct image_data* idata, double gradient, int intercept, int height);
inline void drive(struct image_data* idata);

int main(void)
{
	cm_handle = init_camera();
	car_connect();
	pthread_create(&thread[1],NULL,sensor_handler,NULL);
	//pthread_create(&thread[2],NULL,distance_check,NULL);

	init_drive();

	g_drive_mode = CM_MODE;

	if(g_drive_mode == AI_MODE)
	{
		// get image data from file. 
		fread_data(stored_data);
		drive_ai();
	}else if(g_drive_mode == MD_MODE)
	{ 

		// get image data from file. 
		fread_data(stored_data);
		drive_md();
		// write image data into file.
		fwrite_data(stored_data,g_index);
	}else
	{
		drive_cm();
		// write image data into file. 
		fwrite_data(stored_data,g_index);
	}

	//direct_test();
	pthread_join(thread[1],NULL);
	//pthread_join(thread[2],NULL);
	return 0;
}

// DRIVE WITH ONLY STORED DATA. 
void drive_ai()
{
	struct image_data* idata;
	g_index = 0;

	while(TRUE)
	{
		key_handling();
		
		// busy waiting for next image data. 
		while( g_index>0 && stored_data[g_index-1].dist < mDistance()){}

		// get image data from the file.
		drive(&stored_data[g_index++]);

	}
}

// DRIVE WITH ONLY CAMERA AND STORE THE DATA. 
void drive_cm()
{
	int prev_dist=0;
	g_index = 0;

	while(TRUE)
	{
		// check switch key handling
		key_handling();

		// store image data into stored_data
		line_check(cm_handle,&stored_data[g_index]);
		printf("idata flag %d \n",stored_data[g_index].flag );  

		// check dist prev data and store dist. 
		if(g_index>0)
		{
			stored_data[g_index-1].dist = mDistance()-prev_dist;
			prev_dist = mDistance();
		}

		// drive flag check and drive. - inline function
		drive(&stored_data[g_index++]);
	}
}

// DRIVE WITH STORED DATA AND CAMERA AND MODIFY STORED DATA. 
void drive_md()
{
	struct image_data* idata;
	g_index = 0;

	while(TRUE)
	{
		key_handling();

		// get idata from img processing and store 'stored_data'
		idata = line_check(cm_handle,NULL); // get image data 

		// compare stored data
		// modify stored data

		// free idata memory. 
		free(idata);
		printf("idata flag %d \n",stored_data[g_index].flag ); 

		// drive flag check and drive. - inline function
		drive(&stored_data[g_index++]);
	}

}

inline void drive(struct image_data* idata)
{
	double gradient;
	int angle,input, intercept, height;

	switch(idata->flag)
	{			
		case IF_STOP:
			stop();
			idata->mid_flag = MID_STRAIGHT;
			g_drive_flag = DF_STOP;
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
			height = gradient * MIDWIDTH + intercept;
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


			gradient = tan( (double)idata->angle[RIGHT] *PI /180);
			intercept = idata->bot[RIGHT].y - idata->bot[RIGHT].x * gradient;
			height = gradient * MIDWIDTH + intercept;
			drive_turn( idata, gradient, intercept, height);
			break;

		case IF_SPEED_DOWN:
			printf("------------------------SPEED DOWN------------------\n");
			speed_set(500);
			break;
		case IF_SPEED_BUMP_ST:
		case IF_BOTH:
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

		case IF_CL_LEFT:
		case IF_CL_RIGHT:
			change_course();
			break;
		case IF_SG_STOP:
		case IF_SG_LEFT:
		case IF_SG_RIGHT:
			traffic_drive(idata->flag);
			break;
	}
#ifdef DRIVE
	if(idata->flag != IF_STOP && idata->flag != IF_SG_STOP 
			&& idata->flag  != IF_SG_LEFT && idata->flag !=IF_SG_RIGHT 
			&& idata->flag != IF_CL_LEFT && idata->flag != IF_CL_RIGHT)
	{		
		distance_set(500);	
		forward_dis();
	}
#endif

}

void key_handling()
{

	unsigned char buf;
	unsigned char read_key = read(keyFD, &buf,sizeof(buf));
	int key = read_key;

	switch(key)
	{
		case KEY1:
			break;
		case KEY2:
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
}


