#ifndef __MISO_CAR_LIB_H__
#define __MISO_CAR_LIB_H__

#include "car_lib.h"
#include "miso_values.h"
#include "angle_table.h"


#define DM_INTERVAL 100

// MACRO FUNCTION. 
#define HIGH(X) X/256
#define LOW(X) X%256

// ANGLE RNAGES. 
#define RANGE_STRAIGHT(X)	((X) == 1000)
#define RANGE_NO_CHANGE(X) 	((X) == 0 || (X) == 90 || (X) == 180)
#define RANGE_RIGHT(X)		(0<(X) && (X)<90)
#define RANGE_LEFT(X)		(90<(X) && (X)<180)

static int g_sp = 0;
static int g_angle = DM_STRAIGHT;
static char g_sleep = FALSE;

// car speed 
void speed_set(int sp)
{
	g_sp = sp;
	lm_speed(HIGH(g_sp),LOW(g_sp));
}

void dm_speed_set(int sp)
{
	dm_speed((char)sp);
}

void speed_up(int v)
{
	if( LSPEED_MAX < g_sp+v)
		g_sp = LSPEED_MAX;
	else 
		g_sp+=v;

	lm_speed(HIGH(g_sp),LOW(g_sp));
}

void speed_down(int v)
{
	if( LSPEED_MIN > g_sp - v)
		g_sp = LSPEED_MIN;
	else 
		g_sp-=v;

	lm_speed(HIGH(g_sp),LOW(g_sp));
}

// direction
void turn_straight()
{
	g_angle = DM_STRAIGHT;
	dm_angle(HIGH(g_angle),LOW(g_angle));
}

void turn_set(int v)
{
	if( v > DM_ANGLE_MAX )
		g_angle = DM_ANGLE_MAX;
	else if( v < DM_ANGLE_MIN)
		g_angle = DM_ANGLE_MIN;
	else
		g_angle = v;

	printf("#set angle %d \n",g_angle);
	dm_angle(HIGH(g_angle),LOW(g_angle));
}

void turn_left(int v,int dist, int flag)
{
	int angle;
//	angle =  (DM_STRAIGHT+((v-135)*(14))) / DM_INTERVAL + 2;
	if(flag == AF_CURVE){
		turn_set(left_angle[(dist-1)/5][v/3]);
	}
	else if( flag == AF_STRAIGHT){
		if(dist >= 90)
			return ;
		turn_set(2000);
	}
	else if( flag == AF_STRAIGHT_END)
	{
		if( v <= 155 && v >= 134){
			turn_set(1533);
		}
		else if( (v < 134 && v > 90) || (v < 180 && v > 155) )
			return ;
		//	turn_set(2000);
	}
	else 
		printf("ERROR : invalid flag in turn_left\n");
}

void turn_right(int v,int dist, int flag)
{
	int angle;
//	angle =  (DM_STRAIGHT-((45-v)*(16))) / DM_INTERVAL - 2;
	if(flag == AF_CURVE){
		turn_set(right_angle[(dist-1)/5][v/3]);
	}
	else if( flag == AF_STRAIGHT){
		if(dist >= 90)
			return ;
		turn_set(1000);
	}
	else if( flag == AF_STRAIGHT_END)
	{
		if( v >= 25 && v <= 46){
			turn_set(1533);
		}
		else if( (v > 46 && v < 90) || (v > 0 && v < 25) )
			return ;
			//turn_set(1000);
	}
	else 
		printf("ERROR : invalid flag in turn_right\n");
}

// cm_angle 
void camera_straight()
{
	cm_angle(HIGH(CM_STRAIGHT),LOW(CM_STRAIGHT));
}

void camera_turn_right()
{
	cm_angle(HIGH(CM_STRAIGHT+100),LOW(CM_STRAIGHT+100));
}

void camera_turn_left()
{
	cm_angle(HIGH(CM_STRAIGHT-100),LOW(CM_STRAIGHT-100));
}

void distance_set(int dis)
{
	distance(HIGH(dis),LOW(dis));
}

void change_line(int v)
{
	if(g_image_flag == IF_CL_LEFT){

	}
	else if( g_image_flag == IF_CL_RIGHT){

	}
}

int mDistance(){

	buf[0] = 0xce;
	write(uart_fd, &buf[0], 1);
	read(uart_fd, &read_buf[0], 4);
	return read_buf[1]*65536+read_buf[2]*256+read_buf[3];
}

void set_angle(int angle,int dist, int flag)
{
	if(RANGE_STRAIGHT(angle)){
		g_drive_flag = DF_STR;
	//	speed_set(1500);
		turn_straight();
	} 
	else if( RANGE_RIGHT(angle)){
		g_drive_flag = DF_CUR;
	//	speed_down(10);		
		turn_right(angle,dist,flag);
	} 
	else if(RANGE_LEFT(angle)){
		g_drive_flag = DF_CUR;		
	//	speed_down(10);
		turn_left(angle,dist,flag);
	}else if(RANGE_NO_CHANGE(angle))
	{
		g_drive_flag = DF_CUR;
	}
}

#endif

