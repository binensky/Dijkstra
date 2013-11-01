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
#define RANGE_LEFT(X)		(0<(X) && (X)<20)
#define RANGE_RIGHT(X)		(160<(X) && (X)<180)
#define RANGE_ELSE(X)		(20 <= (X) && (X) <= 160 && (X) != 90)

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
	printf(" ## turn set : %d ## \n", g_angle);
	dm_angle(HIGH(g_angle),LOW(g_angle));
}

void turn_set(int v)
{
	//int before = g_angle;

	if( v > DM_ANGLE_MAX )
		g_angle = DM_ANGLE_MAX;
	else if( v < DM_ANGLE_MIN)
		g_angle = DM_ANGLE_MIN;
	else
		g_angle = v;

	//g_angle = (g_angle + before) /2;

	printf(" ## turn set : %d ## \n", g_angle);
	dm_angle(HIGH(g_angle),LOW(g_angle));
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
}

int mDistance(){

	buf[0] = 0xce;
	write(uart_fd, &buf[0], 1);
	read(uart_fd, &read_buf[0], 4);
	return read_buf[1]*65536+read_buf[2]*256+read_buf[3];
}

void set_angle(int angle)
{
	if(RANGE_STRAIGHT(angle)){
		//g_drive_flag = DF_STR;
		turn_straight();
	} 
	/*
	else if( RANGE_RIGHT(angle)){
		turn_set(800);
	} 
	else if(RANGE_LEFT(angle)){
		turn_set(2200);
	}
	else if(RANGE_NO_CHANGE(angle))
	{
	}
	else if(RANGE_ELSE(angle))
	{
		//turn_set( (int)(2100 - angle * 20 / 3) );
		turn_set( (int)(2200 - angle * 70 / 9) );
	}
	*/
	else if(RANGE_NO_CHANGE(angle)){}
	else{
		turn_set( (int)(2287.5 - angle * 35 / 4) );
	}
}

#endif

