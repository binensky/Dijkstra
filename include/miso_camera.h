#ifndef __CAMERA_H__
#define __CAMERA_H__

#include <stdlib.h>
#include <signal.h>
#include <math.h>
#include <pxa_lib.h>
#include <pxa_camera_zl.h>

#include "miso_car_lib.h"
#include "miso_values.h"
#include "cam_values.h"

struct sigaction act;
struct pxa_video_buf* vidbuf;
struct pxacam_setting camset;

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

void sighandler(int signo);
void init_values(int handle);
void init_point();

void print_screen_org();
void print_screen_y();
void print_screen_cb();
void print_screen_cr();
void print_screen_color();

void set_point_with_line_equation();
int get_road_angle();
int get_angle(struct p_point a, struct p_point b);
int left_line_check(int h);
int right_line_check(int h);
int find_inline(int rl_info, int i, int offset);
int find_outline(int rl_info, int y, int w);
int check_mid_line(int mid_line, int under_line);
int check_speed_bump(int rl_info, int w,int y);
int find_in_point(int rl_info, int i, int offset);
int find_out_end_point( int i, int offset);
int set_end_point(int rl_info, struct p_point* pt_tmp, int flag);
int find_speed_bump_point(int w, int y);
int find_speed_bump_in_point(int i, int j);

int check_traffic_light();
int is_left_turn(int x, int y);

int direct = NONE;
int dir_count = 0;
int is_speed_bump;
int distan = -1; // default
int pt_cnt = 0;
int first = 1;
int angles[10];


struct image_data* line_check(int handle)
{
	int i,j,rl_info;
	int ret, temp, weight, midangle, topangle;
	int angle_bot, angle_end;
	struct image_data* img_data = (struct image_data*)malloc(sizeof(struct image_data));
	struct p_point mid_bot_pt;
	// init values
	init_values(cm_handle);

#ifdef DEBUG
//	print_screen_y();
	//print_screen_org();
	//print_screen_cb();
	//print_screen_cr();
//	print_screen_color();
	//check_traffic_light();
	//exit(0);
#endif

	switch(g_drive_flag)
	{
		int tmp;

		case DF_CUR:
#ifdef TRACE
		printf(" DF_CUR");
#endif
		//speed_set(1000);	

		for(i = 1; i< CUTLINE ; i++)
		{
			if( (find_left == FL_NONE) && left_line_check(i))
			{
				rl_info = LEFT;
			}
			else if( (find_right == FL_NONE) && right_line_check(i))
			{
				rl_info = RIGHT;
			}

			if( find_right == FL_FIND || find_left == FL_FIND)
			{
				g_image_flag = IF_DRIVE;
				break;
			}
		}
		break;

		case DF_STR:
#ifdef TRACE
		printf("DF_STR\n");
#endif
		// 도로 중간 값을 검사한다. 
		tmp = check_mid_line(140,100);
		printf("===========================================================tmp : %d\n", tmp);
		//speed_set(1000);
		//tmp = MID_DRIVE;	// 차선변경이 자꾸 떠서 일단 주석처리
		if( tmp == MID_DRIVE)
		{
			for(i = 1; i< CUTLINE ; i++)
			{
				if((find_left == FL_NONE) && left_line_check(i))
				{
					rl_info = LEFT;
				}
				else if((find_right == FL_NONE) && right_line_check(i))
				{
					rl_info = RIGHT;
				}

				// 두 선중에 하나라도 제대로 된 선을 찾았으면 멈춘다. 
				if( find_right == FL_FIND || find_left == FL_FIND)
				{
					g_image_flag = IF_DRIVE;
					break;
				}
			}
		}
		else if( tmp == MID_STOP)
		{
			g_image_flag = IF_STOP;
			return img_data;
		}
		else if( tmp == MID_CL_LEFT){
			g_image_flag = IF_CL_LEFT;
			printf("----------------------change line left\n");
			return img_data;
		}

		else if( tmp == MID_CL_RIGHT)
		{
			g_image_flag = IF_CL_RIGHT;
			printf("----------------------change line right\n");
			return img_data;
		}
		break;

		case DF_STOP:
		tmp = check_traffic_light();
		// mid red??

		if(tmp == NONE){
			g_image_flag = IF_SG_STOP;
			return img_data;
		}
		else if(tmp == LEFT)
		{
			g_image_flag = IF_SG_LEFT;
			return img_data;
		}
		else if(tmp == RIGHT)
		{
			g_image_flag = IF_SG_RIGHT;
			return img_data;
		}
		// 신호등, 정지선  처리
		break;

		default:
		g_image_flag = IF_NO_PROCESS;
		return img_data;
	}

	if(find_left == FL_NONE && find_right == FL_NONE)
	{
		printf("-------- no line -------\n");
		img_data->angle = 1000;
		img_data->dist = 0;
		return img_data;
	}

	set_point_with_line_equation();

	mid_bot_pt.x = MIDWIDTH;
	mid_bot_pt.y = 0;

	midangle = get_angle(mid_bot_pt, pt[CONTACT]);
#ifdef DRIVE_DEBUG
	for(i=0; i<PT_SIZE; i++)
	{
		printf("pt[%d] : (%d,%d)  ", i, pt[i].x, pt[i].y);
	}
	printf("\n");
#endif

/*	
	if(midangle > 90){
		angle_bot = 180 - angles[BOT];
		angle_end = 180 - angles[pt_cnt - 2];
	} else{
		angle_bot = angles[BOT];
		angle_end = angles[pt_cnt - 2];
	}

	if( (angle_bot > angle_end * 2) || angle_bot < 10)
		img_data->flag = AF_CURVE;
	else if( pt[BOT].y < 15) 
		img_data->flag = AF_STRAIGHT;
	else
		img_data->flag = AF_STRAIGHT_END;
*/

	img_data->angle = midangle;
	//img_data->dist = (distan == -1) ? pt[pt_cnt-1].y : distan;

#ifdef DRIVE_DEBUG
	printf("angle : %d, dist %d\n",img_data->angle, img_data->dist);
#endif
	return img_data;
}

// 직선의 방정식으로 목적 지점을 결정
void set_point_with_line_equation()
{
#ifdef TRACE
	pritnf("set_point_with_line_equation\n");
#endif
	// y = ax + b
	float a = 0.0f;
	int b = 0;
	
	a = (float)((float)(pt[END].y - pt[BOT].y) / (float)(pt[BOT].x - pt[END].x));
	b = pt[BOT].y;

	pt[CONTACT].y = pt[END].y + 50;
	pt[CONTACT].x = (int)((float)(pt[CONTACT].y - b) / a);

}

// 라인을 찾은 것인지를 TRUE FALSE로 리턴 
int left_line_check(int i)
{
#ifdef TRACE
	printf("left line check\n");
#endif
	int w;

	if(IS_YELLOW(width_scan_point,i))	// 중간 값이 1일때, scan point가 선에 겹친경우. 아웃라인을 찾아야 한다.
	{
		return find_outline(LEFT,i,width_scan_point);
	}

	for( w = width_scan_point+1 ; w < MAXWIDTH -1; w++)		// 중간값이 1이 아닌 경우 인라인을 찾아야 한다. 
	{
		if(IS_YELLOW(w,i)){
			return find_inline(LEFT,i,w);
		}
	}
	return FALSE;
}

int right_line_check(int i)
{

#ifdef TRACE
	printf("right line check\n");
#endif

	int w;

	if( IS_YELLOW(width_scan_point,i))	// 중간 값이 1일때, scan point가 선에 겹친경우. 아웃라인을 찾아야 한다.
	{
		return find_outline(RIGHT,i,width_scan_point);
	}
	for( w = width_scan_point-1; w >= 0; w--)
	{
		if(IS_YELLOW(w,i)){
			return find_inline(RIGHT,i,w);
		}
	}
	return FALSE;
}

int find_inline(int rl_info, int y, int w)
{
#ifdef TRACE
	printf("find inline\n");
#endif
	int x, a;
	if(rl_info == LEFT)					// 왼쪽 선이라면 
	{
		// 위로 올라가면서 방지턱을 확인한다
		if(check_speed_bump(rl_info,w,y))
		{
#ifdef DRIVE_DEBUG
			printf("------------------ speed_bump\n");
#endif
			find_left = FL_PASS;
			return TRUE;
		}

		pt[BOT].y = y;
		pt[BOT].x = w;

#ifdef DRIVE_DEBUG
		printf("set bot point <%d><%d>\n",w,y);
#endif

		if(find_in_point(LEFT, y, w)) // sub_point를 찾는다. 
		{
			find_left = FL_FIND;
			return TRUE;
		}else{

			return FALSE;
		}
	}
	else if(rl_info == RIGHT)
	{
		// 위로 올라가면서 방지턱을 확인한다. 
		if(check_speed_bump(rl_info,w,y))
		{
			find_left = FL_PASS;
			return TRUE;
		}
		pt[BOT].y = y;		
		pt[BOT].x = w;

#ifdef DRIVE_DEBUG
		printf("set bot point <%d><%d>\n",w,y);
#endif

		if(find_in_point(RIGHT, y, pt[BOT].x))
		{
			find_right =FL_FIND;
			return TRUE;
		}else{ 

			return FALSE;
		}
	}
}

int find_outline(int rl_info, int y, int w)
{
	int x;

	if(check_speed_bump(rl_info, w, y))
	{
		printf("speed_bump outline : %d\n", y);
		is_speed_bump = TRUE;
		return find_speed_bump_point(w, y);
	}

	if(rl_info == LEFT)					// 왼쪽 선이라면 
	{
		for(x = w; x < MAXWIDTH-1; x++)
		{									// (0,1) 이 잡히는 경우. 
			if( IS_BLACK(x,y) && IS_YELLOW(x-1,y))
			{
				pt[BOT].y = y;
				pt[BOT].x = x-1;

				if(find_out_end_point(y, pt[BOT].x)) // sub_point를 찾는다. 
				{
					find_left =  FL_FIND;
					return TRUE;
				}else 
					return FALSE;
			}
		}
	}
	else if(rl_info == RIGHT)
	{
		for(x = w; x > 0; x--)
		{
			if( IS_YELLOW(x,y) && IS_BLACK(x-1,y)) 
			{
				pt[BOT].y = y;
				pt[BOT].x = x-1;

				if(find_out_end_point(y, pt[BOT].x))
				{
					find_right = FL_FIND;
					return TRUE;
				}else 
					return FALSE;
			}
		}
	}
}

int find_speed_bump_point(int w, int y)
{
	int i, j, start = -1;
	
	for(j = y; j <= CUTLINE; j++)
	{
		if( IS_WHITE_SPEED_BUMP(w,j) )
		{
			printf("in white_speed_bump true %d, %d\n",w,j);
			for(i = w; i < MAXWIDTH-1; i++)
			{
				if(!IS_YELLOW(i,j) && IS_YELLOW(i+1,j))
				{
					pt[BOT].x = i+1;
					pt[BOT].y = j;
					//printf("bot point %d, %d\n", pt[BOT].x, pt[BOT].y);

					return find_speed_bump_in_point(i+1,j);
				}
			}

			for(i = w; i > 0 ; i--)
			{
				if(!IS_YELLOW(i,j) && IS_YELLOW(i-1,j))
				{
					pt[BOT].x = i-1;
					pt[BOT].y = j;
					//printf("bot point %d, %d\n", pt[BOT].x, pt[BOT].y);

					return find_speed_bump_in_point(i-1,j);
				}
			}
		}
	}
	return FALSE;
}

int find_speed_bump_in_point(int i, int j)
{
	int k, l, offset;
	offset = i;
	printf("in find_speed_bump_in_point\n");

	for(l = j+1; l<CUTLINE; l++ )
	{
		if(IS_YELLOW(offset,l))
		{
			for(k = offset; k > 0; k--)
			{
				if( IS_YELLOW(k,l) && !IS_YELLOW(k-1,l))
				{
					pt[END].x = k;
					pt[END].y = l;
					printf("end point %d, %d\n",pt[END].x ,pt[END].y);
					offset = k;
					break;
				}
			}
			if(k == 0)
			{
				if(pt[END].x == -1 || pt[END].y == -1)
				{
					return FALSE;
				}
				else
				{
					is_speed_bump = TRUE;
					find_left = TRUE;
					find_right = TRUE;
					return TRUE;
				}
			}
		}
		else
		{
			for(k = offset; k < MAXWIDTH-1; k++)
			{
				if( IS_YELLOW(k+1,l) && !IS_YELLOW(k,l))
				{
					pt[END].x = k+1;
					pt[END].y = l;
					printf("end point %d, %d\n",pt[END].x ,pt[END].y);
					offset = k;
					break;
				}
			}
			if(k == MAXWIDTH-1){
				if(pt[END].x == -1 || pt[END].y == -1)
				{
					return FALSE;
				}
				else
				{
					is_speed_bump = TRUE;
					find_left = TRUE;
					find_right = TRUE;
					return TRUE;
				}
			}
		}
	}

	if(pt[END].x == -1 || pt[END].y == -1)
		return FALSE;

	is_speed_bump = TRUE;
	find_left = TRUE;
	find_right = TRUE;
	return TRUE;
}

int check_mid_line(int mid_line,int under_line)
{
	// return 되는 가지의 수 
	// 빨간 표지판, 차선 변경, 직진, 곧 곡선구간 

	int i,j,k;
	int left_color = COL_UNKNOWN, right_color = COL_UNKNOWN;

#ifdef TRACE
	printf("in check_mid_line\n");
#endif

#ifdef MID_LINE_DEBUG

	//printf(" left color %d, right color  %d \n", left_color,right_color);
	printf("Y value\n");
	for(j = mid_line; j>under_line ; j--)
	{
		printf("%3d:",j);

		for(i = MAXWIDTH-1; i>=0; i--)
		{	
			if(Y(i,j) >= THRESHOLD)
			{
				if(IS_RED(i,j))
					printf("R");
				else if(IS_YELLOW(i,j))
					printf("1");
				else if(IS_WHITE(i,j))
					printf("2");
				else
					printf(" ");
			}
			else 
				printf(" ");
		}
		printf("\n");
	}
	for( j = 0 ; j < MAXWIDTH; j++)
	{
		if( j == width_scan_point)
			printf("^");
		else 
			printf(" " );
	}
	printf("\n");

#endif

	// mid line 까지 올라가면서 검사한다. 
	for(i=0;i<mid_line;i++)
	{
		if(IS_YELLOW(MIDWIDTH,i))
			return MID_DRIVE;
	}

	for( i = under_line ; i < mid_line; i++)
	{
		if(!IS_BLACK(MIDWIDTH,i))
			break;
		// 왼쪽 탐색 
		for( j = MIDWIDTH ; j < MAXWIDTH-3; j++)
		{
			if( !IS_BLACK(j,i) )
			{
				if(IS_RED(j,i))
				{
					if( i == under_line )
						return MID_STOP;
					return MID_DRIVE;
				}
				// 아웃 라인이 노랑일 때 
				else if(IS_YELLOW(j-1,i) && IS_YELLOW(j,i) && IS_BLACK(j+1,i) && IS_BLACK(j+2,i))
				{
#ifdef	DEBUG
					printf("left mid line yellow (%d,%d)\n",j,i);
#endif
					left_color = COL_YELLOW;
					break;
				}// 아웃 라인이 흰색일 때 
				else if(IS_WHITE(j-1,i) && IS_WHITE(j,i) && IS_BLACK(j+1,i) && IS_BLACK(j+2,i))
				{
#ifdef	DEBUG
					printf("left mid line white (%d,%d)\n",j,i);
#endif
					left_color = COL_WHITE;
					break;
				}
			}
		} 
		// 오른 쪽 탐색 
		for( j = MIDWIDTH -1; j > 2 ; j--)
		{
			if( !IS_BLACK(j,i) )
			{
				if(IS_RED(j,i))
				{
					if( i == under_line)
						return MID_STOP;
					return MID_DRIVE;
				}
				else if(IS_YELLOW(j+1,i) && IS_YELLOW(j,i) && IS_BLACK(j-1,i) && IS_BLACK(j-2,i))
				{
					right_color = COL_YELLOW;
#ifdef	MID_LINE_DEBUG
					printf("right mid line yellow (%d,%d)\n",j,i);
#endif
					break;
				}
				else if(IS_WHITE(j+1,i) && IS_WHITE(j,i) && IS_BLACK(j-1,i) && IS_BLACK(j-2,i))
				{
					right_color = COL_WHITE;
#ifdef	MID_LINE_DEBUG
					printf("left mid line white (%d,%d)\n",j,i);
#endif
					break;
				}
			}
		}

/*		if(left_color == COL_YELLOW && right_color == COL_WHITE)
			return MID_CL_RIGHT;

		else if(left_color == COL_WHITE && right_color == COL_YELLOW)
			return MID_CL_LEFT;

		else{
			right_color = COL_UNKNOWN;
			left_color = COL_UNKNOWN;
		}*/

	} // end vertical scan 
	return MID_DRIVE;			
}

int check_speed_bump(int rl_info, int w, int y)
{
	int count = 0;
	int current_color = ( IS_YELLOW(w,y)? COL_YELLOW : COL_WHITE );
	int is_break = FALSE;
	int i, j;

	//printf("in check_speed_bump %d, %d, %d\n", rl_info, w, y);
	if(rl_info == LEFT)
	{
		for(j = y; j < CUTLINE; j++)
		{
			count = 0;
			for(i = MAXWIDTH-2; i>0; i--)
			{
				if(IS_BLACK_SPEED_BUMP(i,j))
				{
					is_break = TRUE;
					break;
				}
				
				if(IS_YELLOW_SPEED_BUMP(i,j) && (current_color == COL_WHITE))
				{
					current_color = COL_YELLOW;
					count++;
				}
				else if(IS_WHITE_SPEED_BUMP(i,j) && (current_color == COL_YELLOW))
				{
					current_color = COL_WHITE;
					count++;
				}
			}
			if(count >= 4)
				return TRUE;
		}
	}
	else if(rl_info == RIGHT)
	{
		for(j = y; j < CUTLINE; j++)
		{
			count = 0;
			for(i = 1; i<MAXWIDTH -1; i++)
			{
				if(IS_BLACK_SPEED_BUMP(i,j))
				{
					break;
				}

				if(IS_YELLOW_SPEED_BUMP(i,j) && (current_color == COL_WHITE))
				{
					current_color = COL_YELLOW;
					count++;
				}
				else if(IS_WHITE_SPEED_BUMP(i,j) && (current_color == COL_YELLOW))
				{
					current_color = COL_WHITE;
					count++;
				}
			}
			if(count >= 4)
				return TRUE;
		}
	}
	return FALSE;
}

int find_in_point(int rl_info, int i, int offset)
{
#ifdef TRACE
	printf("find in point\n");
#endif

	int y, x,flag;
	struct p_point pt_tmp;

	if(rl_info == LEFT)
	{
		for( y = i+1; y <= CUTLINE ; y++)
		{
			// 위 점이 1인 경우 오른쪽으로 진행중. 
			// 오른쪽으로 돌면서 ->(1,0)을 찾는다. 
			if(!IS_BLACK(offset,y))
			{
				flag = 1;

				for( x = offset; x>0; x--)
				{
					if(x == MIDWIDTH)
						distan = y;

					// (1,0)을 찾은 경우 점을 저장하고 offset을 갱신한다.  
					if( IS_YELLOW(x,y) && IS_BLACK(x-1,y) )
					{
						pt_tmp.y = y;
						pt_tmp.x = x;
						offset = x;
						
						if(set_end_point(rl_info, &pt_tmp,flag))
						{
							return TRUE;
						}
						/*
						else if(x < offset - 100){ // 한 행에서 너무 많이 x이동을 수행하게 되면 
							if(set_end_point(rl_info,&pt_tmp,flag))
								return TRUE;
						}
						*/
						break;
					}
				}
				// 오른쪽에서 1,0 을 못 찾은 경우
				if(x == 0)
				{
					if(pt[END].x == -1)
					{
						init_point();
						return FALSE;
					}
					else
						return TRUE;
				}
			}
			// 위 점이 0인 경우 왼쪽으로 순회하면서 (1,0)<- 찾는다. 
			else
			{
				flag = 0;

				for( x = offset; x<MAXWIDTH-1; x++)
				{
					if(x == MIDWIDTH)
						distan = y;

					if( IS_BLACK(x,y)  && IS_YELLOW(x+1,y))
					{
						pt_tmp.y = y;
						pt_tmp.x = x+1;
						offset = x+1;

						if(set_end_point(rl_info,&pt_tmp,flag))
						{
							return TRUE;
						}

						/*
						else if(x > offset + 100){ // 한 행에서 너무 많이 x이동을 수행하게 되면 
							if(set_end_point(rl_info,&pt_tmp,flag))
								return TRUE;
						}
						*/
						break;
					}
				}
				// 왼쪽에서 (10)을 못 찾은 경우
				if(x == MAXWIDTH-1){
					if(pt[END].x == -1)
					{
						init_point();
						return FALSE;
					}
					else
						return TRUE;
				}
			}
		}// end scan for top
	} // end rl_info == LEFT
	else
	{
		for( y = i+1; y <= CUTLINE ; y++)
		{
			if( IS_BLACK(offset,y))
			{
				flag = 0;

				for( x = offset ; x >0 ; x--)
				{
					if(x == MIDWIDTH)
						distan = y;

					if(IS_YELLOW(x,y) && IS_BLACK(x-1,y))
					{
						pt_tmp.y = y;
						pt_tmp.x = x-1;
						offset = x-1;

						if(set_end_point(rl_info,&pt_tmp,flag))
						{
							return TRUE;
						}
						break;
					}
				}

				// 오른쪽에서 (0,1) 을 못 찾은 경우
				if(x == 0){
					if(pt[END].x == -1)
					{
						init_point();
						return FALSE;
					}
					else
						return TRUE;
				}

			} // 위 점이 1인 경우 왼쪽으로 순회하면서 0,1 찾기
			else
			{
				flag = 1;
				for( x = offset; x<MAXWIDTH-1; x++)
				{
					if(x == MIDWIDTH)
						distan = y;

					if( IS_YELLOW(x,y) && IS_BLACK(x+1,y))
					{
						pt_tmp.y = y;
						pt_tmp.x = x;
						offset = x;

						if(set_end_point(rl_info,&pt_tmp,flag))
						{
							return TRUE;
						}

						break;
					}
				}
				// 왼쪽에서 (0,1)을 못 찾은 경우
				if(x == MAXWIDTH-1)
				{
					if(pt[END].x == -1)
					{
						init_point();
						return FALSE;
					}
					else
						return TRUE;
				}
			}
		} // end scan for top 
	} // end rl_info == RIGHT
	return TRUE;
}

int find_out_end_point(int i, int offset)
{

#ifdef TRACE
	printf("find out point\n");
#endif
	int y, x;

	distan = 0;
	for( y = i+1; y <= CUTLINE ; y++)
	{
		// 위 점이 1인 경우 왼쪽으로 순회하면서 0,1 찾기
		if( !IS_BLACK(offset,y) )
		{
			for( x = offset; x < MAXWIDTH-1; x++)
			{
				if( IS_BLACK(x+1, y) && IS_YELLOW(x,y))
				{
					pt[END].x = x;
					pt[END].y = y;
					offset = x;
					break;
				}
			}
			if(x == MAXWIDTH-1)
			{
				if(pt[END].x == -1)
				{
					init_point();
					return FALSE;
				}
				else
					return TRUE;
			}
		}else	// 위 점이 0인 경우 오른으로 순회하면서 0,1 찾기
		{
			for( x = offset; x > 0; x--)
			{
				if( IS_YELLOW(x,y) && IS_BLACK(x-1,y) )
				{
					pt[END].x = x;
					pt[END].y = y;
					offset = x;
					break;
				}
			}
			if(x == 0)
			{
				if(pt[END].x == -1)
				{
					init_point();
					return FALSE;
				}
				else
					return TRUE;
			}
		}
	}
	return TRUE;
}

int set_end_point(int rl_info, struct p_point* pt_tmp, int flag)
{
#ifdef TRACE
	printf("set_end_point\n");
#endif
/*
	if(direct == NONE)
	{
		if( pt_tmp->x > pt[pt_cnt-1].x)
			direct = LEFT;
		else if(pt_tmp->x < pt[pt_cnt-1].x)
			direct = RIGHT;
	}
	else
	{
		if(direct == LEFT && pt_tmp->x < pt[pt_cnt-1].x)
			return TRUE;
		else if(direct == RIGHT && pt_tmp->x > pt[pt_cnt-1].x)
			return TRUE;
	}
	return FALSE;
*/
	// 방향이 설정되어 있지 않을 때,
	if( direct == NONE ) 
	{
		if( pt_tmp->y > GAP)
		{
			if( pt[MID].y == -1){
				pt[MID].x = pt_tmp->x;
				pt[MID].y = pt_tmp->y;
			}
			else if( pt[MID].y + GAP < pt_tmp->y )
			{
				if( pt[MID].x > pt_tmp->x )
					direct = RIGHT;
				else if( pt[MID].x < pt_tmp->x)
					direct = LEFT;
			}
		}		
	}
	else 
	{
		if(	// 방향이 바뀌는 경우
				(direct == LEFT && rl_info == LEFT && flag) ||
				(direct == LEFT && rl_info == RIGHT && !flag) ||
				(direct == RIGHT && rl_info == LEFT && !flag) ||
				(direct == RIGHT && rl_info == RIGHT && flag)
		  )
		{
//			pt[END].x = (pt[BOT].x + pt[END].x) / 2;
			return TRUE;
		}
		else if( pt[END].x == pt_tmp->x)
		{
			dir_count+=1;
			if( dir_count >3)
			{
				dir_count = 0;
				return TRUE;
			}
		}
		else
			dir_count = 0;

	}

	pt[END].x = pt_tmp->x;
	pt[END].y = pt_tmp->y;

	return FALSE;
}

int get_road_angle()
{
	int i;
	int sum = 0;

	for(i = 0; i<pt_cnt-1; i++)
	{
		int angle;
		angle = get_angle( pt[i], pt[i+1] );
		angles[i] = angle;
#ifdef DRIVE_DEBUG
		printf("ange[%d]:%d ", i, angle);
#endif
		sum += angle;
	}
	printf("\n");
	
	if(pt_cnt  == 1)
		return 1000;

	return sum/(pt_cnt-1);
}

int get_angle( struct p_point a, struct p_point b)
{
	printf("point a : (%d,%d) , point b : (%d,%d)\n", a.x, a.y, b.x, b.y);
	return (int)(atan2((double)(b.y-a.y), (double)(a.x-b.x)) * 180 / PI);
}

int check_traffic_light()
{
	// 정지선에 의한 차량 정지 시 신호등 확인 부분 수행
	// return으로 정지(NONE), 좌회전(LEFT), 우회전(RIGHT)

	int i, j;
	int red_count = 0, yellow_count = 0, green_count = 0;

	// cutline 위 부분을 확인
	for(i = MAXWIDTH -1 ; i >= 0; i--)
	{
		for(j = MAXHEIGHT-1 ; j> 170; j--)
		{
			// Y, CB, CR값 테스트로 Red, Green, Yellow 구분 (+ 좌회전 화살표?)
			if(IS_TRAFFIC_RED(i,j))
			{
				red_count++;
			}
			else if(IS_TRAFFIC_GREEN(i,j))	// 초록불인 경우
			{
				green_count++;
			}
			else if(IS_TRAFFIC_YELLOW(i,j))
			{
				yellow_count++;
			}
			else							// 그 외
			{
			}
		}
	}
	
	if(red_count >= 100)
	{
		printf("COLOR : RED!\n");
		return NONE;
	}
	else if(yellow_count >= 100)
	{
		printf("COLOR : YELLOW!\n");
		return NONE;
	}
	else if(green_count >= 250)
	{
		printf("RIGHT TURN!\n");
		return RIGHT;
	}
	else if(green_count >= 100)
	{
		printf("LEFT TURN!\n");
		return LEFT;
	}
	else
	{
		printf("Nothing! red : %d, yellow : %d, green : %d\n",red_count, yellow_count, green_count);
	}
}

int is_left_turn(int x, int y)
{
	int k, l, count = 0;
	for(k = x; k > x-35 ;k--)
	{
		for(l = y; l > 200; l--)
		{
			if(IS_TRAFFIC_GREEN(k,l))	// 초록불인 경우
			{
				count++;
			}
		}
	}
	if(count < 250)
		return TRUE;
	else
		return FALSE;
}


void init_point()
{
#ifdef TRACE
	printf("init point\n");
#endif
	int i;
	for( i = 0; i< PT_SIZE; i++){
		pt[i].y	= -1;
		pt[i].x = -1;
	}

	direct = NONE;
	distan = -1;
	pt_cnt = 0;
}

int get_width_scan_point()
{
	if(g_angle > DM_STRAIGHT + 500) // 심한 좌회전인경우
		return MAXWIDTH-1;

	else if(g_angle < DM_STRAIGHT - 500)
		return 0;

	else
		return  MIDWIDTH-1;
}

void init_values(int handle)
{
#ifdef TRACE
	printf("init values\n");
#endif
	int i=0;

	init_point();
	width_scan_point = get_width_scan_point();
	//width_scan_point = MIDWIDTH;
	find_left = FL_NONE;
	find_right = FL_NONE;

	// 버퍼 초기화 
	if(!first)
		camera_release_frame(cm_handle,vidbuf);
	else
		first = 0;

	vidbuf = camera_get_frame(cm_handle);
//	camera_release_frame(cm_handle,vidbuf);
//	vidbuf = camera_get_frame(cm_handle);
	//camera_release_frame(cm_handle,vidbuf);
	//vidbuf = camera_get_frame(cm_handle);
}

void sighandler(int signo)
{
	close(fd);
	exit(0);
}

void print_screen_org()
{	
	int i,j;
	printf("Y value\n");
	for(j = CUTLINE; j>0 ; j--)
	{

		printf("%3d:",j);
		for(i = MAXWIDTH-1; i>=0; i--)
		{	
			printf("%3d ",Y(i,j));
		}
		printf("\n");

	}
	for( j = 0 ; j < MAXWIDTH; j++)
	{
		if( j == width_scan_point)
			printf("^");
		else 
			printf(" " );
	}

}

void print_screen_y()
{
	int i,j;
	printf("Y value\n");
	for(j = CUTLINE+40; j>0 ; j--)
	{
		printf("%3d:",j);

		for(i = MAXWIDTH-1; i>=0; i--)
		{	
			if(Y(i,j) >= THRESHOLD)
			{
				if(IS_YELLOW(i,j))
					printf("1");
				else if(IS_WHITE(i,j))
					printf("2");
				else
					printf("0");
			}
			else 
				printf("0");
		}
		printf("\n");
	}
	for( j = 0 ; j < MAXWIDTH; j++)
	{
		if( j == width_scan_point)
			printf("^");
		else 
			printf(" " );
	}
	printf("\n");
}

void print_screen_cb()
{
	int i,j;
	printf("CB value\n");
	/*
	   for(i = CUTLINE; i>0; i--)
	   {
	   printf("%3d ", i);
	   }
	 */
	for(j = 100; j>0 ; j--)
	{
		printf("%3d:",j);
		for(i = MAXWIDTH-1; i>=0; i--)
		{
			printf("%3d ",CB(i,j));
		}
		printf("\n");
	}
	printf("\n");

}

void print_screen_cr()
{
	int i,j;
	printf("CR value\n");
	/*
	   for(i = 200; i>=0; i--)
	   {
	   printf("%3d ", i);
	   }
	 */
	for(j = 200; j>100 ; j--)
	{
		printf("%3d:",j);
		for(i = MAXWIDTH -1; i>=0; i--)
		{
			printf("%3d ",CR(i, j));
		}
		printf("\n");
	}
	printf("\n");

}

void print_screen_color()
{
	int i,j;
	printf("    ");
	for(i = MAXWIDTH -1; i>=0; i--)
	{
		printf("%d", i/100);
	}
	printf("\n");
	for(i = MAXHEIGHT-1; i>150 ; i--)
	{
		printf("%3d:",i);
		for(j = MAXWIDTH-1; j>=0; j--)
		{	
			if(IS_TRAFFIC_RED(j,i))
				printf("R");
			else if(IS_TRAFFIC_YELLOW(j,i))
				printf("Y");
			else if(IS_TRAFFIC_GREEN(j,i))
				printf("G");
			else
				printf(" ");
			/*
			   if(vidbuf->ycbcr.cb[i/2 * MAXWIDTH + j/2] >= THRESHOLD_RED_MIN_CB &&
			   vidbuf->ycbcr.cb[i/2 * MAXWIDTH + j/2] < THRESHOLD_RED_CB &&		// 빨간불인 경우
			   vidbuf->ycbcr.cr[i/2 * MAXWIDTH + j/2] >= THRESHOLD_RED_CR)
			   printf("R");
			   else if(vidbuf->ycbcr.cb[i/2 * MAXWIDTH + j/2] < THRESHOLD_GREEN_CB &&
			   vidbuf->ycbcr.cr[i/2 * MAXWIDTH + j/2] < THRESHOLD_GREEN_CR)	// 초록불인 경우
			   printf("G");
			   else if(vidbuf->ycbcr.cb[i/2 * MAXWIDTH + j/2] < THRESHOLD_YELLOW_CB &&		// 노란불인 경우
			   vidbuf->ycbcr.cr[i/2 * MAXWIDTH + j/2] >= THRESHOLD_YELLOW_CR &&
			   vidbuf->ycbcr.cr[i/2 * MAXWIDTH + j/2] < THRESHOLD_RED_CR)
			   printf("Y");
			   else
			   printf(" ");
			 */
		}
		printf("\n");
	}
	/*
	   for( j = 0 ; j < MAXWIDTH; j++)
	   {
	   if( j == width_scan_point)
	   printf("^");
	   else 
	   printf(" " );
	   }
	 */
	printf("\n");
}

int init_camera(){
	int len;

	cm_handle = camera_open(NULL, 0);
	if(cm_handle < 0)
		return -1;

	system("echo b > /proc/invert/tb"); //LCD DriverIC top-bottom invert ctrl

	memset(&camset,0,sizeof(camset));
	camset.mode = CAM_MODE_VIDEO;
	camset.format = pxavid_ycbcr422;
	camset.width = 320;
	camset.height = 240;

	camera_config(cm_handle,&camset);
	camera_start(cm_handle);

	return cm_handle;
}


int exit_camera(int handle){
	camera_stop(handle);
	camera_close(handle);
	return 0;
}

#endif


