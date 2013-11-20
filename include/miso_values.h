
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
#define DM_ANGLE_MAX 2150
#define DM_ANGLE_MIN 850 
#define CM_SPEED_MAX 2200
#define CM_SPEED_MIN 800

// drive flag
#define DF_STOP 0
#define DF_READY 1
#define DF_DRIVE 2
#define DF_WHITE_SPEED_DOWN 3
#define DF_RED_SPEED_DOWN 4
#define DF_SPEED_BUMP 5
#define DF_END 9
#define DF_VPARK 61
#define DF_PPARK 62

// image flag 
#define IF_NO_PROCESS -1

#define IF_STRAIGHT 1
#define IF_LEFT 4
#define IF_RIGHT 5
#define IF_BOTH 6

#define IF_SPEED_BUMP_ST 11
#define IF_SPEED_BUMP_CUR 12

#define IF_RED_SPEED_DOWN 23
#define IF_WHITE_SPEED_DOWN 24

#define IF_OUTLINE 31

#define IF_NO_DRIVE 40

#define IF_SG_LEFT 44
#define IF_SG_RIGHT 45
#define IF_SG_STOP 49

#define IF_CL_LEFT 54
#define IF_CL_RIGHT 55

#define IF_PARK_V 61
#define IF_PARK_H 62

#define IF_RED_STOP 99

#define DM_STRAIGHT 1533
#define CM_STRAIGHT 1515

#define KEY1 1
#define KEY2 2
#define KEY3 3

#define INIT_THREAD 0
#define WAIT_THREAD 1
#define RESUME_THREAD 2
#define END_THREAD 3

#define NO_MODE -1
#define CM_MODE 0
#define AI_MODE 1

#define RESUME_INDEX 150

#define DATA_SIZE 10000
#define START_SPEED 1500

#define RED_TOP_Y 180
#define RED_BOT_Y 90
struct p_point{
	int x;		// x좌표 (0~319)
	int y;		// y좌표 (0~239)
};

struct image_data{
	int flag; 		// whole image process flag. 
	int mid_flag;		// mid line image process flag. 
	struct p_point bot[3];	// bottom point of lines(none, left, right). 
	int angle[3];		// angle of lines (none, left, right)
};

struct drive_data{
	int flag;	// whole image process flag. 
	int mid_flag;	// mid line image process flag 
	int angle;	// image process angle. 
	int dist;	// distance of drive moter. 
};

// car status flags
static int g_drive_flag;
static int g_drive_mode;
static int g_index ;
static int g_park_dis;
static int g_wait_thread ;
static int had_red_stop = FALSE;
// camera fd
static int cm_handle;	

// car drive datas of previous drive. 
struct drive_data d_data[DATA_SIZE];
pthread_t thread[3];	
#endif
