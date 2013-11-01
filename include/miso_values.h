
#ifndef __MISO_VALUES_H__
#define __MISO_VALUES_H__

#define NONE -1
#define STOP 0

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
#define DF_DRIVE 1
#define DF_VPARK 3
#define DF_PPARK 4
#define DF_END 9

// image flag 
#define IF_NO_PROCESS -1
#define IF_STOP 0
#define IF_LEFT 1
#define IF_RIGHT 2
#define IF_BOTH 3
#define IF_STRAIGHT 5
#define IF_OUTLINE 6

#define IF_SG_STOP 10
#define IF_SG_LEFT 12
#define IF_SG_RIGHT 13
#define IF_CL_LEFT 21
#define IF_CL_RIGHT 22

#define DM_STRAIGHT 1533
#define CM_STRAIGHT 1533

struct p_point
{
	int x;		// x좌표 (0~319)
	int y;		// y좌표 (0~239)
};

struct image_data
{
	struct image_data* prev;
	struct image_data* next;

	int flag; 	// NONE(-1), STOP(0), LEFT, RIGHT, LEFT+RIGHT
	int mid_flag;
	int angle[3];
	struct p_point bot[3];
};

// flags
static int g_drive_flag = DF_DRIVE;
static int g_image_flag = IF_NO_PROCESS;
#endif
