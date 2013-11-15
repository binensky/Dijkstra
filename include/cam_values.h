#ifndef __CAM_VALUES_H__
#define __CAM_VALUES_H__

// VIDEOBUF_COUNT must be larger than STILLBUF_COUNT
#define PI 3.14159265358979323846
#define VIDEOBUF_COUNT  1
#define STILLBUF_COUNT  0
#define CAM_STATUS_INIT     0
#define CAM_STATUS_READY    1
#define CAM_STATUS_BUSY     2

// -------- MISO Defined Value ----------

#define CUTLINE 140
#define CUTLINE_CL 140
#define CUTLINE_POINT 100
#define CUTLINE_CURVE 70
#define CUTLINE_OUTLINE 10
#define GAP 10

#define DEST_HEIGHT 140

// threshold
#define THRESHOLD 130
#define THRESHOLD_CB 120

#define THRESHOLD_RED_MIN_CB 100
#define THRESHOLD_RED_CB 130
#define THRESHOLD_RED_CR 165
#define THRESHOLD_YELLOW_CB 120
#define THRESHOLD_YELLOW_CR 140
#define THRESHOLD_GREEN_CB 120
#define THRESHOLD_GREEN_CR 110

#define THRESHOLD_STOP_CB 120
#define THRESHOLD_STOP_CR 170

#define THRESHOLD_RED_STOP 60
#define THRESHOLD_RED_STOP_CB 120
#define THRESHOLD_RED_STOP_CR 180 

#define BOT 0
#define MID 1
#define END 2
#define PT_SIZE 14

// find  line flag 
#define FL_NONE 0
#define FL_FIND 1
#define FL_PASS 2

// mid line flag

#define MID_NONE -1
#define MID_STRAIGHT 1
#define MID_CURVE_STRAIGHT 2
#define MID_CURVE 3


#define MID_SPEED_BUMP_ST 11
#define MID_SPEED_BUMP_CUR 12

#define MID_RED_SPEED_DOWN 23
#define MID_WHITE_SPEED_DOWN 24

#define MID_OUTLINE 31

#define MID_CL_LEFT 54
#define MID_CL_RIGHT 55

#define MID_RED_STOP 99

// screen size 
#define MAXHEIGHT 240
#define MAXWIDTH 320
#define MIDWIDTH 160

// color flag
#define COL_UNKNOWN 0
#define COL_PASS 1
#define COL_YELLOW 2
#define COL_WHITE 3
#define COL_RED 4

// whell flag
#define WHEEL_STRAIGHT 0
#define WHEEL_TURN 1

// macro function 
#define Y(A,B) vidbuf->ycbcr.y[(B)*MAXWIDTH+(A)]
//#define Y(C,D) img_buf_y[(D)][(C)]
#define CB(E,F) vidbuf->ycbcr.cb[((F)/2)*MAXWIDTH+((E)/2)]
#define CR(G,H) vidbuf->ycbcr.cr[((H)/2)*MAXWIDTH+((G)/2)]

#define IS_UNKNOWN(M,N) ((Y(M,N) >= THRESHOLD) && (Y(M,N) < THRESHOLD+70) && (CB(M,N) >= THRESHOLD_YELLOW_CB))
#define IS_YELLOW(I,J) (( Y(I,J) >= THRESHOLD) && ( CB(I,J) < THRESHOLD_YELLOW_CB))
#define IS_WHITE(K,L) ((Y(K,L) >= THRESHOLD+70) && ( CB(K,L) >= THRESHOLD_CB))

#define IS_RED(X,Z) (Y(X,Z) >= THRESHOLD_RED_STOP  && CB(X,Z) < THRESHOLD_RED_STOP_CB && CR(X,Z) >= THRESHOLD_RED_STOP_CR)

#define IS_BLACK(X,Z) !(IS_YELLOW(X,Z) || IS_WHITE(X,Z) || IS_RED(X,Z))

#define IS_YELLOW_SPEED_BUMP(O,P) (IS_YELLOW(O,P) && IS_YELLOW(O-1,P) && IS_YELLOW(O+1, P))
#define IS_WHITE_SPEED_BUMP(O,P) (IS_WHITE(O,P) && IS_WHITE(O-1,P) && IS_WHITE(O+1, P))
#define IS_BLACK_SPEED_BUMP(O,P) (IS_BLACK(O,P)&&!IS_UNKNOWN(O,P) && IS_BLACK(O-1,P)&&!IS_UNKNOWN(O-1,P) && IS_BLACK(O+1,P)&&!IS_UNKNOWN(O+1,P)) 

#define IS_TRAFFIC_RED(X,Z) (Y(X,Z) >= THRESHOLD && CB(X,Z) < THRESHOLD_CB && CR(X,Z) >= THRESHOLD_RED_CR)
#define IS_TRAFFIC_YELLOW(X,Z) (Y(X,Z) >= THRESHOLD  && CB(X,Z) < THRESHOLD_CB && CR(X,Z) >= THRESHOLD_YELLOW_CR && CR(X,Z) < THRESHOLD_RED_CR)
#define IS_TRAFFIC_GREEN(X,Z) (Y(X,Z) >= THRESHOLD  && CB(X,Z) < THRESHOLD_CB && CR(X,Z) < THRESHOLD_GREEN_CR)

struct pxa_camera
{
	int handle;
	int status;
	int mode;
	int sensor;
	int ref_count;

	// Video Buffer
	int width;
	int height;
	enum    pxavid_format format;
};


struct p_point pt[PT_SIZE];

struct sigaction act;
struct pxa_video_buf* vidbuf;
struct pxacam_setting camset;

int find_left = FL_NONE, find_right = FL_NONE;
int img_buf_y[MAXHEIGHT][MAXWIDTH];
int width_scan_point = MIDWIDTH;
int is_broken_line = FALSE;
int had_change_line = TRUE;
int cnt_change_line = 0;

#endif
