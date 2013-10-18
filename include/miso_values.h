
#ifndef __MISO_VALUES_H__
#define __MISO_VALUES_H__

#define NONE -1

#define TRUE 0xff
#define FALSE 0x00

// required define-values. 
#define LSPEED_MAX 2000
#define LSPEED_MIN 500
#define DISTANCE_MAX 65535
#define DISTANCE_MIN 0	
#define DM_ANGLE_MAX 2200
#define DM_ANGLE_MIN 800 
#define CM_SPEED_MAX 2200
#define CM_SPEED_MIN 800

// drive flag
#define DF_STOP 0
#define DF_STR 1
#define DF_CUR 2
#define DF_VPARK 3
#define DF_PPARK 4
#define DF_END 9

// image flag 
#define IF_NO_PROCESS -1

#define IF_STOP 0
#define IF_DRIVE 1
#define IF_CL_LEFT 2
#define IF_CL_RIGHT 3

#define IF_SG_STOP 10
#define IF_SG_LEFT 12
#define IF_SG_RIGHT 13

#define DM_STRAIGHT 1533
#define CM_STRAIGHT 1533

#define LF_INIT -1
#define LF_STRAIGHT 0
#define LF_CURVE 1
#define LF_CURVE_STRAIGHT 2

struct image_data
{
	int angle;	
	int dist;
};

struct line_equation
{
	float gradient;
	int intercept;
};

// flags
static int g_drive_flag = DF_STR;
static int g_image_flag = IF_DRIVE;
static int line_flag = LF_INIT;

#endif
