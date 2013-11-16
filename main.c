//#define DEBUG
//#define DRIVE_DEBUG
//#define MID_LINE_DEBUG
#define DRIVE
//#define TRACE
#define SAVE_FILE

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
//	drive_test();

	pthread_create(&thread[0],NULL,key_handler,NULL);
	pthread_create(&thread[2],NULL,parking_check,NULL);

	printf("thread create\n");
	
	while(g_drive_flag != DF_END)
	{
		if( g_drive_mode == AI_MODE)
			drive_ai();
		else if( g_drive_mode == CM_MODE){
			drive_cm();
			fwrite_data(d_data);
		}
	}
	return 0;
}

// DRIVE WITH ONLY STORED DATA. 
void drive_ai()
{
	struct image_data* idata;
	int prev_dist=0;
	
	int it_index = 181;// for test.. 
	g_index = 183;	//for test
	distance_set(200);
	forward_dis();
	// park_init();	

	while(g_drive_flag != DF_READY && it_index <= g_index ){
		// busy waiting for next image data.

		while(d_data[it_index-1].dist > mDistance()-prev_dist){}
		prev_dist = mDistance();

		printf("---------------- g_index : %d / %2d / %2d / %4d / %4d -------------------\n",it_index,d_data[it_index].flag, d_data[it_index].mid_flag,d_data[it_index].angle, d_data[it_index].dist);
		// if need to ai_img_processing.. 
		if(d_data[it_index].flag > IF_NO_DRIVE || d_data[it_index].flag == IF_WHITE_SPEED_DOWN){
			ai_event_proc(d_data[it_index].flag);

		}else{
			turn_set(d_data[it_index].angle);
			distance_set(d_data[it_index].dist+100);
			forward_dis();
		}

		it_index++;
	}
	g_drive_flag = DF_END;
}

// DRIVE WITH ONLY CAMERA AND STORE THE DATA. 
void drive_cm()
{
	int prev_dist=0;
	struct image_data* idata;

	//g_index = 0;
	distance_reset();

	while(g_drive_flag == DF_DRIVE || g_drive_flag == DF_STOP || g_drive_flag == DF_SPEED_DOWN){

		// init thread.. 
	//	printf("g_index : %d g_wait_thread : %d\n", g_index, g_wait_thread);
		if(g_wait_thread == WAIT_THREAD && g_index >= RESUME_INDEX)
			g_wait_thread = RESUME_THREAD;

		idata = cm_img_process();

//#ifdef DRInVE_DEBUG
		printf("> df_ %d>>>>>if%d \n",g_drive_flag,idata->flag );  
//#endif
	// check dist prev data and store dist. 
		if(g_index>0){
			if(d_data[g_index-1].flag == IF_PARK_H)
				d_data[g_index-1].dist = g_park_dis;
			else
				d_data[g_index-1].dist = mDistance()-prev_dist;
			prev_dist = mDistance();
		}else if(g_index == 0){
			prev_dist = mDistance();
		}else{
			printf("g_index error\n");
			exit(0);
		}

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
			speed_set(1500);
			usleep(1000);
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

			//if(d_data[g_index-1].flag != IF_WHITE_SPEED_DOWN)

			if(g_drive_flag != DF_SPEED_DOWN)
			{
				pthread_create(&thread[1],NULL,sensor_handler,NULL);
				printf("set df speed down\n");
				usleep(10000);
				g_drive_flag = DF_SPEED_DOWN;
				turn_straight();
				d_data[g_index].angle = DM_STRAIGHT;
			}
			else
			{
				if(idata->bot[LEFT].y < 0 && idata->bot[RIGHT].y < 0)
				{
					printf("no point...\n");
					turn_straight();
					d_data[g_index].angle = DM_STRAIGHT;
				}
				else if(idata->bot[RIGHT].y < 0)
				{
					printf("no right point L %d\n", idata->bot[LEFT].y);
					int angle = DM_STRAIGHT - (280 - 2*idata->bot[LEFT].y);
					turn_set(angle);
					d_data[g_index].angle = angle;
				}
				else if(idata->bot[LEFT].y < 0)
				{
					printf("no left point R %d\n", idata->bot[RIGHT].y);
					int angle = DM_STRAIGHT + (280 - 2*idata->bot[RIGHT].y);
					turn_set(angle);
					d_data[g_index].angle = angle;
				}
				else
				{
					printf("both point L %d R %d \n",idata->bot[LEFT].y,idata->bot[RIGHT].y);
					int angle = DM_STRAIGHT + (idata->bot[LEFT].y - idata->bot[RIGHT].y)*3;
					turn_set(angle);
					d_data[g_index].angle = angle;
				}
			}
	
			break;

		case IF_SPEED_BUMP_CUR:
			if(d_data[g_index-1].flag == IF_SPEED_BUMP_CUR)
				turn_set(d_data[g_index-1].angle - 50);
				//turn_set((d_data[g_index-1].angle + DM_STRAIGHT)/2);
			d_data[g_index].angle = d_data[g_index-1].angle -50;
			//d_data[g_index].angle = (d_data[g_index-1].angle + DM_STRAIGHT) / 2;
			break;

		case IF_BOTH:
			printf("left  %d, right %d\n",idata->bot[LEFT].y, idata->bot[RIGHT].y);

			if(d_data[g_index-1].flag == IF_SPEED_BUMP_ST)
			{
				d_data[g_index].flag = IF_SPEED_BUMP_ST;
				turn_straight();
				d_data[g_index].angle = DM_STRAIGHT;
			}
			else if(idata->bot[RIGHT].y < 70 && idata->bot[LEFT].y < 70)
			{
				speed_set(1000);
				usleep(1000);
				int angle = DM_STRAIGHT + (idata->bot[LEFT].y - idata->bot[RIGHT].y)*5;
				turn_set(angle);
				d_data[g_index].angle = angle;
			}
			else if(idata->bot[RIGHT].y < 70)
			{
				int angle = DM_STRAIGHT + (480 - 6*idata->bot[RIGHT].y);
				turn_set(angle);
				d_data[g_index].angle = angle;
			}
			else if(idata->bot[LEFT].y < 70)
			{
				int angle = DM_STRAIGHT - (480 - 6*idata->bot[LEFT].y);
				turn_set(angle);
				d_data[g_index].angle = angle;
			}
			else{
				int angle = DM_STRAIGHT + (idata->bot[LEFT].y - idata->bot[RIGHT].y)*5;
				turn_set(angle);
				d_data[g_index].angle = angle;
			}

			break;
		case IF_SPEED_BUMP_ST:
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

//#ifdef DRIVE_DEBUG
	printf(" idata->flag %d / g_drive_flag %d \n",idata->flag, g_drive_flag);
	if( idata->flag < IF_NO_DRIVE && g_drive_flag == DF_DRIVE){		
		distance_set(500);	
		forward_dis();
	}
//#endif
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
			temp_flag = MID_CURVE_STRAIGHT;
		}
	}
	else if(idata->mid_flag == MID_CURVE)
	{
		temp_flag = MID_CURVE;
	}
	printf("temp flag : %d\n",temp_flag);

	if(temp_flag == MID_STRAIGHT)
	{
		d_data[g_index].mid_flag = MID_STRAIGHT;
		if(d_data[g_index-1].flag == IF_SPEED_BUMP_ST)
		{
			d_data[g_index].flag = IF_SPEED_BUMP_ST;
			turn_straight();
			d_data[g_index].angle = DM_STRAIGHT;
			return;
		}
		if(idata->flag == IF_RIGHT)
		{
			printf("bot right %d\n", idata->bot[RIGHT].y);
			if(idata->bot[RIGHT].y < 70)
			{
				int angle = DM_STRAIGHT + (480 - 6*idata->bot[RIGHT].y);
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
			printf("bot left %d\n", idata->bot[LEFT].y);
			if(idata->bot[LEFT].y < 70)
			{
				int angle = DM_STRAIGHT - (480 - 6*idata->bot[LEFT].y);
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
		dest.y = temp_flag == MID_CURVE ? DEST_HEIGHT : DEST_HEIGHT+100;
		dest.x = (int)((dest.y - intercept)/gradient);
		dest_angle = get_angle(mid_bot,dest);

		if(idata->flag == IF_LEFT)
			printf("gra %.2lf, bot (%d,%d) dest (%d,%d) dest angle : %d\n",gradient, idata->bot[LEFT].x, idata->bot[LEFT].y,dest.x,dest.y,dest_angle);
		else
			printf("gra %.2lf, bot (%d,%d) dest (%d,%d) dest angle : %d\n",gradient, idata->bot[RIGHT].x, idata->bot[RIGHT].y,dest.x,dest.y,dest_angle);
		
		
	
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
	winker_light(OFF);
	usleep(10000);	
	turn_straight();
	usleep(10000);	
	camera_turn_right();
	usleep(500000);
	camera_straight();
	usleep(10000);
	speed_set(1500);
	usleep(10000);
	dm_speed_set(5);
	usleep(10000);
	accel(0x02f);
	usleep(10000);
	reduction(0x2f);
	usleep(10000);
	distance_set(2000);
	usleep(10000);
	line_stop();

// set flags...
	g_drive_flag = DF_READY;
	g_drive_mode = NO_MODE;
	g_index = 0;
	g_park_dis = 0;
	g_wait_thread =INIT_THREAD;
	printf("init drive end\n");
}



