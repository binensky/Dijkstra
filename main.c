
//#define DEBUG
//#define DRIVE_DEBUG
//#define MID_LINE_DEBUG
#define DRIVE
//#define TRACE

#include <stdio.h>
#include <pthread.h>

#include "include/miso_values.h"
#include "include/key_handler.h"
#include "include/miso_car_lib.h"
#include "include/miso_camera.h"
#include "include/miso_aimode.h"
#include "include/sensor_handler.h"
#include "include/parking_modules.h"
#include "include/drive_modules.h"
#include "include/file_lib.h"

pthread_t thread[3];	
// 0:key handling , 1:sensor handling, 2:distance_check

void init_drive(void);
void drive_ai();
void drive_cm();
void drive_turn(struct image_data* idata, double gradient, int intercept, int height);
inline void drive(struct image_data* idata);

int main(void)
{
	cm_handle = init_camera();
	car_connect();

	init_drive();
	pthread_create(&thread[0],NULL,key_handler,NULL);
	pthread_create(&thread[2],NULL,parking_check,NULL); 

	while(TRUE)
	{
		if(g_drive_flag == DF_DRIVE && g_drive_mode == AI_MODE)
			drive_ai();
		else if(g_drive_flag == DF_DRIVE && g_drive_mode == CM_MODE)
			drive_cm();
	}

	pthread_join(thread[0],NULL);
	pthread_join(thread[1],NULL);
	pthread_join(thread[2],NULL);
	return 0;
}

// DRIVE WITH ONLY STORED DATA. 
void drive_ai()
{
	struct image_data* idata;

	g_index = 0;
	distance_reset();

	while(g_drive_flag != DF_READY){
		// busy waiting for next image data.
		while( g_index>0 && d_data[g_index-1].dist < mDistance()){}

		// if need to ai_img_processing.. 
		if(idata->flag == IF_RED_STOP || idata->flag == IF_RED_SPEED_DOWN 
				|| idata->flag == IF_SG_STOP){
			while(TRUE){
				int flag = ai_img_process(MID_RED_STOP);
				if(flag == IF_RED_SPEED_DOWN){
					speed_set(500);
					turn_straight();
					pthread_create(&thread[1],NULL,sensor_handler,NULL);
					// sensoring??
					g_index++;
				}
				else if(flag == IF_RED_STOP){
					stop();
				}else if( flag==IF_SG_STOP || flag==IF_SG_LEFT || flag==IF_SG_RIGHT){
					traffic_drive(flag); //how to end drive mode??
				}else // exit of the states.. 
				{
					g_index++;
					break;
				}
			}	
		}else	// have not need to additional image processing. 
		{
			turn_set(d_data[g_index].angle);
			distance_set(d_data[g_index].dist+100);
			forward_dis();
			g_index++;
		}
	}
}

// DRIVE WITH ONLY CAMERA AND STORE THE DATA. 
void drive_cm()
{
	int prev_dist=0;
	struct image_data* idata;

	g_index = 0;
	distance_reset();

	while(g_drive_flag != DF_READY){
		// store image data into d_data

		printf("g_index : %d g_wait_thread : %d\n", g_index, g_wait_thread);
		if(g_wait_thread == WAIT_THREAD && g_index >= RESUME_INDEX)
			g_wait_thread = RESUME_THREAD;

		if(g_wait_thread == END_THREAD)
			pthread_create(&thread[1],NULL,sensor_handler,NULL);
	
		// check dist prev data and store dist. 
		if(g_index>0){
			if(d_data[g_index-1].flag == IF_WHITE_SPEED_DOWN)
				continue;
			d_data[g_index-1].dist = mDistance()-prev_dist;
			prev_dist = mDistance();
		}else if(g_index == 0){
			prev_dist = mDistance();
		}else{
			printf("g_index error\n");
			exit(0);
		}

		idata = cm_img_process();
#ifdef DRIVE_DEBUG
		printf("idata flag %d \n",idata->flag );  
#endif

		// drive flag check and drive. - inline function
		drive(idata);
		free(idata);

		if(g_index == 0){
			d_data[g_index].flag = IF_STRAIGHT;
			d_data[g_index].mid_flag = MID_STRAIGHT;
			d_data[g_index].angle = 0;
			d_data[g_index].dist = 0;
		}
		g_index+=1;
	}
}

inline void drive(struct image_data* idata){
	double gradient;
	int angle, intercept, height;

	d_data[g_index].flag = idata->flag;
	d_data[g_index].mid_flag = idata->mid_flag;

	
	if(g_drive_flag == DF_VPARK || g_drive_flag == DF_PPARK){
		d_data[g_index].flag = g_drive_flag;
		d_data[g_index].mid_flag = MID_STRAIGHT;
	}
	else{
		d_data[g_index].flag = idata->flag;
		d_data[g_index].mid_flag = idata->mid_flag;
	}

	switch(d_data[g_index].flag){			
		case IF_RED_STOP:
			stop();
			d_data[g_index].mid_flag = MID_STRAIGHT;
			break;

		case IF_LEFT:

			if(idata->angle[LEFT] == 1000){
				turn_straight();
				d_data[g_index-1].mid_flag = MID_STRAIGHT;
				break;
			}
			if(g_index>0 && d_data[g_index-1].flag == IF_RIGHT){
				d_data[g_index-1].mid_flag = MID_STRAIGHT;
			}

			gradient = tan( (double)idata->angle[LEFT] * PI /180);
			intercept = idata->bot[LEFT].y - idata->bot[LEFT].x * gradient;
			height = gradient * MIDWIDTH + intercept;

			if(idata->angle[LEFT] < 90){
				turn_set(DM_ANGLE_MAX);
				d_data[g_index].angle = DM_ANGLE_MAX;
				break;
			}

			drive_turn(idata, gradient, intercept, height);
			break;

		case IF_RIGHT:
			
			if(idata->angle[RIGHT] == 1000){
				turn_straight();
				d_data[g_index-1].mid_flag = MID_STRAIGHT;
				break;
			}
			if(g_index>0 && d_data[g_index-1].flag == IF_LEFT )
			{
				d_data[g_index-1].mid_flag = MID_STRAIGHT;
			}

			gradient = tan( (double)idata->angle[RIGHT] *PI /180);
			intercept = idata->bot[RIGHT].y - idata->bot[RIGHT].x * gradient;
			height = gradient * MIDWIDTH + intercept;

			if(idata->angle[RIGHT] > 90)
			{
				turn_set(DM_ANGLE_MIN);
				d_data[g_index].angle = DM_ANGLE_MIN;
				break;
			}

			drive_turn(idata, gradient, intercept, height);
			break;

		case IF_RED_SPEED_DOWN:	
			printf("---RED SPEED DOWN---\n");
			speed_set(1000);
			d_data[g_index].angle = d_data[g_index-1].angle;
			break;
		case IF_WHITE_SPEED_DOWN:
			printf("---WHITE SPEED DOWN---\n");
			speed_set(500);
			d_data[g_index].angle = d_data[g_index-1].angle;
			break;

		case IF_SPEED_BUMP_CUR:
			d_data[g_index].angle = d_data[g_index-1].angle;
			break;
		case IF_SPEED_BUMP_ST:
		case IF_BOTH:
		case IF_STRAIGHT:
			turn_straight();
			d_data[g_index].angle = DM_STRAIGHT;
			break;

		case IF_OUTLINE:
			if(g_angle < DM_STRAIGHT)
			{
				turn_set(DM_ANGLE_MIN);
				d_data[g_index].angle = DM_ANGLE_MIN;
			}
			else if(g_angle > DM_STRAIGHT)
			{
				turn_set(DM_ANGLE_MAX);
				d_data[g_index].angle = DM_ANGLE_MAX;
			}
			else
			{
				turn_straight();
				d_data[g_index].angle = DM_STRAIGHT;
			}
			break;

		case IF_CL_LEFT:
		case IF_CL_RIGHT:
			change_course();
			break;
		case IF_SG_STOP:
		case IF_SG_LEFT:
		case IF_SG_RIGHT:
			traffic_drive(d_data[g_index].flag);
			break;
		case IF_PARK_V:
		case IF_PARK_H:
			parking(d_data[g_index].flag);
			break;
	}

#ifdef DRIVE_DEBUG
	printf(" idata->flag %d / g_drive_flag %d \n",idata->flag, g_drive_flag);
	if( idata->flag < IF_NO_DRIVE && g_drive_flag == DF_DRIVE){		
		distance_set(500);	
		forward_dis();
	}
#endif
}

void drive_turn(struct image_data* idata, double gradient, int intercept, int height)
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
			if(g_index == 0 || (g_index>0 && (d_data[g_index-1].mid_flag == MID_STRAIGHT || d_data[g_index-1].mid_flag == MID_RED_STOP || d_data[g_index-1].mid_flag == MID_SPEED_BUMP_ST)))
				temp_flag = MID_STRAIGHT;
			else
			{
				//printf("prev mid flag : %d\n",d_data[g_index-1].mid_flag);
				temp_flag = MID_CURVE_STRAIGHT;
			}
		}
	}
	else if(idata->mid_flag == MID_CURVE_STRAIGHT){
		if(g_index == 0 || (g_index>0 && (d_data[g_index-1].mid_flag == MID_STRAIGHT || d_data[g_index-1].mid_flag == MID_RED_STOP || d_data[g_index-1].mid_flag == MID_SPEED_BUMP_ST)))
		{
			temp_flag = MID_STRAIGHT;
		}
		else
		{
			//printf("prev mid flag : %d\n",d_data[g_index-1].mid_flag);
			temp_flag = MID_CURVE_STRAIGHT;
		}
	}
	else if(idata->mid_flag == MID_CURVE)
	{
		temp_flag = MID_CURVE;
	}

	if(temp_flag == MID_STRAIGHT)
	{
		d_data[g_index].mid_flag = MID_STRAIGHT;

		if(idata->flag == IF_RIGHT)
		{
			if(idata->bot[RIGHT].y < 70)
			{	
				int angle = DM_STRAIGHT + (210 - 3*idata->bot[RIGHT].y);
				turn_set(angle);
				d_data[g_index].angle = angle;
			}
			else{
				turn_straight();
				d_data[g_index].angle = DM_STRAIGHT;
			}
		}
		else if(idata->flag == IF_LEFT)
		{
			if(idata->bot[LEFT].y < 70)
			{
				int angle = DM_STRAIGHT - (210 - 3*idata->bot[LEFT].y);
				turn_set(angle);
				d_data[g_index].angle = angle;
			}
			else{
				turn_straight();
				d_data[g_index].angle = DM_STRAIGHT;
			}
		}
		else{
			turn_straight();
			d_data[g_index].angle = DM_STRAIGHT;
		}
	}
	else
	{
		mid_bot.y = 0;
		mid_bot.x = MIDWIDTH;
		dest.y = temp_flag == MID_CURVE ? DEST_HEIGHT : DEST_HEIGHT+60;
		dest.x = (int)((dest.y - intercept)/gradient);
		dest_angle = get_angle(mid_bot,dest);
	
		if(temp_flag == MID_CURVE_STRAIGHT && dest_angle > 85  && dest_angle < 95){
			d_data[g_index].mid_flag = MID_STRAIGHT;
		}else{
			d_data[g_index].mid_flag = MID_CURVE;
		}

		if(dest_angle == 1000){
			turn_straight();
			d_data[g_index].angle = DM_STRAIGHT;
		}else if(dest_angle == 0 || dest_angle == 90 || dest_angle == 180){
			d_data[g_index].angle = d_data[g_index-1].angle;
		}else{
			int angle =  (int)(2200 - dest_angle * 70 / 9) ;
			turn_set( angle );
			d_data[g_index].angle = angle;
		}
	}
}

void init_drive()
{
#ifdef DRIVE
	sleep(3);
#endif
	turn_straight();
	usleep(2000);	
	camera_turn_right();
	usleep(500000);
	camera_straight();
	usleep(2000);
	speed_set(1000);
	usleep(2000);
	dm_speed_set(5);
	usleep(2000);
	accel(0x02f);
	usleep(2000);
	reduction(0x2f);
	usleep(2000);
	distance_set(2000);
	usleep(2000);
	line_stop();
}



