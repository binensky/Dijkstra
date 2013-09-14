
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

struct image_data
{
	int angle;	
	int dist;
};

// flags
static int g_drive_flag = DF_STR;
static int g_image_flag = IF_DRIVE;


// data arrays
static int a2dm_right[18]={
1400, 	// 90 ~ 86
1400, 	// 85 ~ 81
1400, 	// 80 ~ 76
1400, 	// 75 ~ 71
1300,	// 70 ~ 66
1300,	// 65 ~ 61
1250,	// 60 ~ 56
1250,	// 55 ~ 51
1250,	// 50 ~ 46
1200,	// 45 ~ 41
1150,	// 40 ~ 36
1100,	// 35 ~ 31
1000,	// 30 ~ 26
1000,	// 25 ~ 21
950,	// 20 ~ 16
900,	// 15 ~ 11
850,	// 10 ~ 6
800	// 5 ~ 1
};	// img angle에 대한 drive moter 값 설정 

static int a2dm_left[18]={
1600, 	// 91 ~ 95
1600, 	// 96 ~ 100
1650, 	// 101 ~ 105
1650, 	// 106 ~ 110
1700,	// 111 ~ 115
1700,	// 116 ~ 120
1700,	// 121 ~ 125
1730,	// 126 ~ 130
1730,	// 131 ~ 135
1750,	// 136 ~ 140
1750,	// 141 ~ 145
1750,	// 146 ~ 150
1800,	// 151 ~ 155
1900,	// 156 ~ 160
2050,	// 161 ~ 165
2100,	// 166 ~ 170
2150,	// 171 ~ 175
2200	// 176 ~ 180
};	// img angle에 대한 drive moter 값 설정 


#endif
