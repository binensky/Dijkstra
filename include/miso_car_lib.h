#ifndef __MISO_CAR_LIB_H__
#define __MISO_CAR_LIB_H__

#include "car_lib.h"
#include "miso_values.h"

#define DM_INTERVAL 100

// MACRO FUNCTION. 
#define HIGH(X) X/256
#define LOW(X) X%256

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
	if( v > DM_ANGLE_MAX )
		g_angle = DM_ANGLE_MAX;
	else if( v < DM_ANGLE_MIN)
		g_angle = DM_ANGLE_MIN;
	else
		g_angle = v;

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
	cm_angle(HIGH(2200),LOW(2200));
}

void camera_turn_left()
{
	cm_angle(HIGH(800),LOW(800));
}

void distance_set(int dis)
{
	distance(HIGH(dis),LOW(dis));
}

int mDistance(){
	buf[0] = 0xce;
	write(uart_fd, &buf[0], 1);
	read(uart_fd, &read_buf[0], 4);
	write(uart_fd, &buf[0], 1);
	read(uart_fd, &read_buf[0], 4);
	return  read_buf[0]* 256*256*256 + read_buf[1]*65536 + read_buf[2]*256 + read_buf[3];
}
#endif

