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

void init_values(int handle,struct image_data* idata);
void init_point();

void print_screen_org();
void print_screen_y();
void print_screen_cb();
void print_screen_cr();
void print_screen_color();

int red_count();
int get_road_angle();
int get_angle(struct p_point a, struct p_point b);
int left_line_check(int h);
int right_line_check(int h);
int find_inline(int rl_info, int i, int offset);
int find_outline(int rl_info, int y, int w);
int check_mid_line();
int check_speed_bump(int w,int y);
int find_in_point(int rl_info, int i, int offset);
int find_out_end_point( int i, int offset);
int set_end_point(int rl_info, struct p_point* pt_tmp, int flag);
int find_speed_bump_point(int w, int y);
int check_change_line(int rl_info, int x, int y);
int find_broken_line(int rl_info, int x, int y);

int check_traffic_light();
int is_left_turn(int x, int y);

int direct = NONE;
int dir_count = 0;
int is_speed_bump;
int pt_cnt;	// left pt, right pt,
int speed_bump_count = 0;
int angles[PT_SIZE-1];

struct image_data* line_check(int handle)
{
	int i,j,rl_info,k;
	int ret, temp, weight, midangle, topangle;
	int angle_bot, angle_end;
	struct image_data* img_data = (struct image_data*)malloc(sizeof(struct image_data));

	// init values
	init_values(cm_handle,img_data);
#ifdef DEBUG
	//print_screen_y();
	//print_screen_org();
	//print_screen_cb();
	//print_screen_cr();
	//print_screen_color();
	//check_traffic_light();
	//exit(0);
#endif

	printf("   drive flag : %d\n",g_drive_flag);

#ifdef MID_LINE_DEBUG
	for(j = CUTLINE; j>=0 ; j--)
	{
		printf("%3d:",j);

		for(i = MAXWIDTH-1; i>=0; i--)
		{	
			if(IS_RED(i,j))
				printf("R");
			else if(IS_YELLOW(i,j))
				printf("1");
			else if(IS_WHITE(i,j))
				printf("2");
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
#endif

	//g_change_line = TRUE;	// 차선변경 제외

	switch(g_drive_flag)
	{
		int tmp;

		case DF_DRIVE:

		printf("DF_DRIVE\n");
		// 도로 중간 값을 검사한다. 
		
		tmp = check_mid_line();
		img_data->mid_flag = tmp;

		printf("check mid line : %d\n", tmp);

		if( tmp == MID_STRAIGHT)
		{
			for(i = 1; i< CUTLINE_POINT ; i++)
			{
				if((find_left == FL_NONE) && left_line_check(i))
				{
					if(!g_change_line && g_broken_line)
					{
						img_data->flag = IF_CL_LEFT;
						g_change_line = TRUE;
						return img_data;
					}

					img_data->flag += IF_LEFT;
					img_data->angle[LEFT] = get_road_angle();
					img_data->bot[LEFT].x = pt[BOT].x;
					img_data->bot[LEFT].y = pt[BOT].y;
#ifdef DRIVE_DEBUG
					for(k=0; k<pt_cnt; k++)
						printf("pt[%d] : (%d,%d)  ", k, pt[k].x, pt[k].y);
					printf("\n");
#endif
					init_point();
				}
				if((find_right == FL_NONE) && right_line_check(i))
				{
					if(!g_change_line && g_broken_line)
					{
						img_data->flag = IF_CL_RIGHT;
						g_change_line = TRUE;
						return img_data;
					}
					img_data->flag += IF_RIGHT;
					img_data->angle[RIGHT] = get_road_angle();
					img_data->bot[RIGHT].x = pt[BOT].x;
					img_data->bot[RIGHT].y = pt[BOT].y;
#ifdef DRIVE_DEBUG
					for(k=0; k<pt_cnt; k++)
					{
						printf("pt[%d] : (%d,%d)  ", k, pt[k].x, pt[k].y);
					}
					printf("\n");
#endif
					init_point();
				}
			}
			if(find_left == FL_NONE && find_right == FL_NONE)
				img_data->flag = IF_STRAIGHT;

			return img_data;
		}
		else if( tmp == MID_CURVE || tmp == MID_CURVE_STRAIGHT)
		{
			for(i = 1; i< CUTLINE_POINT ; i++)
			{
				if((find_left == FL_NONE) && left_line_check(i))
				{
					// 왼쪽 각도 설정하여 idata에 넣는다. 
					img_data->flag = IF_LEFT;
					img_data->angle[LEFT] = get_road_angle();
					img_data->bot[LEFT].x = pt[BOT].x;
					img_data->bot[LEFT].y = pt[BOT].y;
#ifdef DRIVE_DEBUG
					for(k=0; k<pt_cnt; k++)
					{
						printf("pt[%d] : (%d,%d)  ", k, pt[k].x, pt[k].y);
					}
					printf("\n");
#endif
					return img_data;
				}
				else if((find_right == FL_NONE) && right_line_check(i))
				{
					// 오른쪽 각도 설정하여 idata에 넣는다. 
					img_data->flag = IF_RIGHT;
					img_data->angle[RIGHT] = get_road_angle();
					img_data->bot[RIGHT].x = pt[BOT].x;
					img_data->bot[RIGHT].y = pt[BOT].y;
#ifdef DRIVE_DEBUG
					for(k=0; k<pt_cnt; k++)
					{
						printf("pt[%d] : (%d,%d)  ", k, pt[k].x, pt[k].y);
					}
					printf("\n");
#endif
					return img_data;
				}
			}
		}
		else if( tmp == MID_STOP){
			img_data->flag = IF_STOP;
			return img_data;
		}
		else if( tmp == MID_SPEED_BUMP_CUR)
		{
			img_data->flag = IF_SPEED_BUMP_CUR;
			return img_data;
		}
		else if( tmp == MID_SPEED_BUMP_ST)
		{
			img_data->flag = IF_SPEED_BUMP_ST;
			return img_data;
		}
		else if( tmp == MID_OUTLINE )
		{
			img_data->flag = IF_OUTLINE;
			return img_data;
		}	
		else if( tmp == MID_SPEED_DOWN)
		{
			img_data->flag = IF_SPEED_DOWN;
			return img_data;
		}

		break;

		/////////////////////////////////////////////////////// traffic 
		case DF_STOP:
		tmp = check_traffic_light();

		if(tmp == NONE){
			img_data->flag = IF_SG_STOP;
			return img_data;
		}
		else if(tmp == LEFT)
		{
			img_data->flag = IF_SG_LEFT;
			return img_data;
		}
		else if(tmp == RIGHT)
		{
			img_data->flag = IF_SG_RIGHT;
			return img_data;
		}
		break;

		default:
		img_data->flag= IF_NO_PROCESS;
		return img_data;
	}

	if(find_left == FL_NONE && find_right == FL_NONE)
	{
		printf("-------- no line -------\n");
		img_data->flag = IF_STRAIGHT;
		return img_data;
	}

	return img_data;
}

// 라인을 찾은 것인지를 TRUE FALSE로 리턴 
int left_line_check(int i)
{
	int w;
#ifdef TRACE
	printf("left line check\n");
#endif
	for( w = width_scan_point+1 ; w < MAXWIDTH -1; w++)		// 중간값이 1이 아닌 경우 인라인을 찾아야 한다. 
	{
		if(!IS_BLACK(w,i)){
			return find_inline(LEFT,i,w);
		}
	}
	return FALSE;
}

int right_line_check(int i)
{
	int w;
#ifdef TRACE
	printf("right line check\n");
#endif

	for( w = width_scan_point-1; w >= 0; w--)
	{
		if(!IS_BLACK(w,i)){
			return find_inline(RIGHT,i,w);
		}
	}
	return FALSE;
}

int find_inline(int rl_info, int y, int w)
{
	int x, a;
#ifdef TRACE
	printf("find inline\n");
#endif
	if(rl_info == LEFT)					// 왼쪽 선이라면 
	{
		pt[BOT].y = y;
		pt[BOT].x = w;
		pt_cnt += 1;

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
		pt[BOT].y = y;		
		pt[BOT].x = w;
		pt_cnt += 1;

#ifdef DRIVE_DEBUG
		printf("set bot point <%d><%d>\n",w,y);
#endif

		if(find_in_point(RIGHT, y, pt[BOT].x))
		{
			find_right = FL_FIND;
			return TRUE;
		}else{ 

			return FALSE;
		}
	}
}


int find_outline(int rl_info, int y, int w)
{
	int x;

	if(rl_info == LEFT)					// 왼쪽 선이라면 
	{
		for(x = w; x < MAXWIDTH-1; x++)
		{									// (0,1) 이 잡히는 경우. 
			if( IS_BLACK(x,y) && !IS_BLACK(x-1,y))
			{
				pt[BOT].y = y;
				pt[BOT].x = x-1;
				pt_cnt += 1;

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
			if( !IS_BLACK(x,y) && IS_BLACK(x-1,y)) 
			{
				pt[BOT].y = y;
				pt[BOT].x = x-1;
				pt_cnt += 1;

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
int red_count(){
	int i = 0, j = 0, red_count = 0;
	for(i = 70 ; i < 150; i ++){
		for( j = 0 ; j < MAXWIDTH ; j+=3)
		{
			if(IS_RED(j,i))
			{
				red_count += 1;
			}
		}
	}

	if(red_count >= 3){
		if(i < 120)
		{
#ifdef DEBUG
			printf("MID_STOP in %d\n", i);
#endif
			return MID_STOP;
		}
		else if(i > 140)
		{
#ifdef DEBUG
			printf("SLow Down in %d\n", i);
#endif
			return MID_SPEED_DOWN;
		}
	}
}

int white_count(int y){

	int i = 0, j = 0;
	int cnt = 0;

	for(i = y ; i < CUTLINE ; i ++){
		if(!IS_WHITE(MIDWIDTH, i)){
			return FALSE;	
		}
		else if(IS_WHITE(MIDWIDTH, i)){
#ifdef DEBUG
			printf("----------------------------------------------------------------cnt : %d\n", cnt);
#endif
			cnt += 1;		
		}
		if(cnt >= 4){
			return TRUE;
		}
	}

	return FALSE;


}
int check_mid_line()
{
	int i,j;
	int height = -1;
	int red_cnt = 0, change_line_cnt = 0;
	int red_flag;

#ifdef TRACE
	printf("check_mid_line\n");
#endif

	red_flag = red_count();

	if(red_flag == MID_SPEED_DOWN)
		return MID_SPEED_DOWN;
	else if(red_flag == MID_STOP)
		return MID_STOP;

	// 미드라인 수직 검사 
	for(i=1; i<CUTLINE; i++)
	{
		// check cross stop 
		if(!IS_BLACK(MIDWIDTH, i))
		{
			height = i;
			printf("yellow height %d \n",height);

			if(check_speed_bump(MIDWIDTH,i))
			{
				if(g_index>0 && d_data[g_index-1].mid_flag == MID_SPEED_BUMP_ST)
					return MID_SPEED_BUMP_ST;	// speed bump check.
				else if(g_index>0 && d_data[g_index-1].mid_flag == MID_SPEED_BUMP_CUR)
				{
					if(speed_bump_count == 6)
						return MID_SPEED_BUMP_ST;
					else
						return MID_SPEED_BUMP_CUR;
				}
				else
					return MID_SPEED_BUMP_CUR;
			}

			if(height <= 10)
				return MID_OUTLINE;
			else if( height <= CUTLINE_CURVE)
				return  MID_CURVE;
			else if( CUTLINE_CURVE < height && height <= CUTLINE)
				return MID_CURVE_STRAIGHT;

			if( IS_WHITE(MIDWIDTH,i)) 
			{
				if(i == CUTLINE -1 || IS_BLACK(MIDWIDTH, i+1))
					return MID_SPEED_DOWN;
			}
			break;
		}
		
		else{
		}// end else
	} // end for 
	if(height == -1) 
		return MID_STRAIGHT;
}

int check_speed_bump(int w, int y)
{
	int current_color = ( !IS_BLACK(w,y)? COL_YELLOW : COL_WHITE );
	int i, j;

#ifdef TRACE

	printf("in check_speed_bump %d, %d\n", w, y);

#endif

	for(j = y; j < CUTLINE; j++)
	{
		if(j > CUTLINE - 20)
			break;

		speed_bump_count = 0;
		for(i = MAXWIDTH-2; i>0; i--)
		{
			if(IS_BLACK_SPEED_BUMP(i,j))
			{
				break;
			}
			if(IS_YELLOW_SPEED_BUMP(i,j) && (current_color == COL_WHITE))
			{
				current_color = COL_YELLOW;
				speed_bump_count++;
			}
			else if(IS_WHITE_SPEED_BUMP(i,j) && (current_color == COL_YELLOW))
			{
				current_color = COL_WHITE;
				speed_bump_count++;
			}
		}
#ifdef DRIVE_DEBUG
		if(speed_bump_count != 0)
			printf("----------speed bump count in line %d : %d\n",j, speed_bump_count);
#endif

		if( g_index>0 && d_data[g_index-1].mid_flag != MID_SPEED_BUMP_CUR 
			&& d_data[g_index-1].mid_flag != MID_SPEED_BUMP_ST 
			&& speed_bump_count >= 4)
			return TRUE;	// MID_SPEED_BUMP_CUR
		else if(g_index>0 && d_data[g_index-1].mid_flag == MID_SPEED_BUMP_CUR 
				&& speed_bump_count == 6)
			return TRUE;	// MID_SPEED_BUMP_ST
		else if(g_index>0 && d_data[g_index-1].mid_flag == MID_SPEED_BUMP_CUR 
				&& speed_bump_count >= 4)
			return TRUE;	// MID_SPEED_BUMP_CUR
		else if(g_index>0 && d_data[g_index-1].mid_flag == MID_SPEED_BUMP_ST 
				&& speed_bump_count >= 4)
			return TRUE;	// MID_SPEED_BUMP_ST

	}
	return FALSE;
}

int find_in_point(int rl_info, int i, int offset)
{
	int y, x,flag;
	int direct = NONE;
	struct p_point pt_tmp, broken_pt;
#ifdef TRACE
	printf("find in point\n");
#endif

	if(rl_info == LEFT)
	{
		for( y = i+1; y < CUTLINE ; y++)
		{
			// 위 점이 1인 경우 오른쪽으로 진행중. 
			// 오른쪽으로 돌면서 ->(1,0)을 찾는다. 
			if(!IS_BLACK(offset,y))
			{
				flag = 1;

				for( x = offset; x>0; x--)
				{
					// (1,0)을 찾은 경우 점을 저장하고 offset을 갱신한다.  
					if( !IS_BLACK(x,y) && IS_BLACK(x-1,y) )
					{
						if(pt_cnt == 1)
						{
							pt[pt_cnt].y = y;
							pt[pt_cnt].x = x;
						}

						if(pt[pt_cnt-1].y + 10 == y)
						{
							pt[pt_cnt].y = y;
							pt[pt_cnt].x = x;
							pt_cnt += 1;
						}

						pt_tmp.y = y;
						pt_tmp.x = x;
						offset = x;

						if(set_end_point(rl_info, &pt_tmp,flag))
						{
							if(pt_cnt == 1 && pt[pt_cnt].x != -1)
								pt_cnt += 1;
							return TRUE;
						}
						break;
					}
				}
				// 오른쪽에서 1,0 을 못 찾은 경우
				if(x == 0)
				{
					if(check_change_line(LEFT, pt_tmp.x, pt_tmp.y))
					{
						g_broken_line = TRUE;
						return TRUE;
					}
					else if(pt[BOT+1].x == -1)
					{
						init_point();
						return FALSE;
					}
					else
					{
						if(pt_cnt == 1)
							pt_cnt += 1;
						return TRUE;
					}
				}
			}
			// 위 점이 0인 경우 왼쪽으로 순회하면서 (1,0)<- 찾는다. 
			else
			{
				flag = 0;

				for( x = offset; x<MAXWIDTH-1; x++)
				{
					if( IS_BLACK(x,y)  && !IS_BLACK(x+1,y))
					{

						if(pt_cnt == 1)
						{
							pt[pt_cnt].y = y;
							pt[pt_cnt].x = x;
						}

						if(pt[pt_cnt-1].y + 10 == y)
						{
							pt[pt_cnt].y = y;
							pt[pt_cnt].x = x+1;
							pt_cnt += 1;
						}

						pt_tmp.y = y;
						pt_tmp.x = x+1;
						offset = x+1;

						if(set_end_point(rl_info,&pt_tmp,flag))
						{
							if(pt_cnt == 1 && pt[pt_cnt].x != -1)
								pt_cnt += 1;
							return TRUE;
						}

						break;
					}
				}
				// 왼쪽에서 (10)을 못 찾은 경우
				if(x == MAXWIDTH-1){
					if(check_change_line(LEFT, pt_tmp.x, pt_tmp.y))
					{
						g_broken_line = TRUE;
						return TRUE;
					}
					else if(pt[BOT+1].x == -1)
					{
						init_point();
						return FALSE;
					}
					else
					{
						if(pt_cnt == 1)
							pt_cnt += 1;
						return TRUE;
					}
				}
			}
		}// end scan for top
	} // end rl_info == LEFT
	else
	{
		for( y = i+1; y < CUTLINE ; y++)
		{
			if( IS_BLACK(offset,y))
			{
				flag = 0;

				for( x = offset ; x >0 ; x--)
				{
					if(!IS_BLACK(x,y) && IS_BLACK(x-1,y))
					{

						if(pt_cnt == 1)
						{
							pt[pt_cnt].y = y;
							pt[pt_cnt].x = x;
						}

						if(pt[pt_cnt-1].y + 10 == y)
						{
							pt[pt_cnt].y = y;
							pt[pt_cnt].x = x-1;
							pt_cnt += 1;
						}

						pt_tmp.y = y;
						pt_tmp.x = x-1;
						offset = x-1;

						if(set_end_point(rl_info,&pt_tmp,flag))
						{
							if(pt_cnt == 1 && pt[pt_cnt].x != -1)
								pt_cnt += 1;
							return TRUE;
						}
						break;
					}
				}

				// 오른쪽에서 (0,1) 을 못 찾은 경우
				if(x == 0){
					if(check_change_line(RIGHT, pt_tmp.x, pt_tmp.y))
					{
						g_broken_line = TRUE;
						return TRUE;
					}
					else if(pt[BOT+1].x == -1)
					{
						init_point();
						return FALSE;
					}
					else
					{
						if(pt_cnt == 1)
							pt_cnt += 1;					
						return TRUE;
					}
				}

			} // 위 점이 1인 경우 왼쪽으로 순회하면서 0,1 찾기
			else
			{
				flag = 1;
				for( x = offset; x<MAXWIDTH-1; x++)
				{
					if( !IS_BLACK(x,y) && IS_BLACK(x+1,y))
					{

						if(pt_cnt == 1)
						{
							pt[pt_cnt].y = y;
							pt[pt_cnt].x = x;
						}

						if(pt[pt_cnt-1].y + 10 == y)
						{
							pt[pt_cnt].y = y;
							pt[pt_cnt].x = x;
							pt_cnt += 1;
						}

						pt_tmp.y = y;
						pt_tmp.x = x;
						offset = x;

						if(set_end_point(rl_info,&pt_tmp,flag))
						{
							if(pt_cnt == 1 && pt[pt_cnt].x != -1)
								pt_cnt += 1;
							return TRUE;
						}

						break;
					}
				}
				// 왼쪽에서 (0,1)을 못 찾은 경우
				if(x == MAXWIDTH-1)
				{
					if(check_change_line(RIGHT, pt_tmp.x, pt_tmp.y))
					{
						g_broken_line = TRUE;
						return TRUE;
					}
					else if(pt[BOT+1].x == -1)
					{
						init_point();
						return FALSE;
					}
					else
					{
						if(pt_cnt == 1)
							pt_cnt += 1;
						return TRUE;
					}
				}
			}
		} // end scan for top 
	} // end rl_info == RIGHT
	if(pt_cnt == 1 && pt[pt_cnt].x != -1)
		pt_cnt += 1;
	return TRUE;
}


int find_out_end_point(int i, int offset)
{
	int y, x;
#ifdef TRACE
	printf("find out point\n");
#endif

	for( y = i+1; y <= CUTLINE ; y++)
	{
		// 위 점이 1인 경우 왼쪽으로 순회하면서 0,1 찾기
		if( !IS_BLACK(offset,y) )
		{
			for( x = offset; x < MAXWIDTH-1; x++)
			{
				if( IS_BLACK(x+1, y) && !IS_BLACK(x,y))
				{
					if(pt[pt_cnt-1].y + 10 == y)
					{
						pt[pt_cnt].y = y;
						pt[pt_cnt].x = x;
						pt_cnt += 1;
					}

					offset = x;
					break;
				}
			}
			if(pt[BOT+1].x == -1 && x == MAXWIDTH-1)
			{
				init_point();
				return FALSE;
			}
		}else	// 위 점이 0인 경우 오른으로 순회하면서 0,1 찾기
		{
			for( x = offset; x > 0; x--)
			{
				if( !IS_BLACK(x,y) && IS_BLACK(x-1,y) )
				{
					if(pt[pt_cnt-1].y + 10 == y)
					{
						pt[pt_cnt].y = y;
						pt[pt_cnt].x = x;
						pt_cnt += 1;
					}

					offset = x;
					break;
				}
			}
			if(pt[BOT+1].x == -1 && x == 0 )
			{
				init_point();
				return FALSE;
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
		{
			if(pt_cnt != 1)
			{
				pt_cnt -= 1;
				pt[pt_cnt].x = -1;
				pt[pt_cnt].y = -1;
			}
			return TRUE;
		}
		else if(direct == RIGHT && pt_tmp->x > pt[pt_cnt-1].x)
		{
			if(pt_cnt != 1)
			{
				pt_cnt -= 1;
				pt[pt_cnt].x = -1;
				pt[pt_cnt].y = -1;
			}
			return TRUE;
		}
	}
	return FALSE;
}

int check_change_line(int rl_info, int x, int y)
{
	int i, j, k;

#ifdef TRACE
	printf("in check change line\n");
#endif

	for(j=y; j<y+50; j++)
	{
		if(j >= CUTLINE_CL)
			break;

		if(rl_info == LEFT)
		{
			for(i=x; i>x-20; i--)
			{
				if(IS_BLACK(i,j) && !IS_BLACK(i-1,j))
				{
					for(k=i-1; k>0; k--)
					{
						if(!IS_BLACK(k,j) && IS_BLACK(k-1,j))
						{
							if(find_broken_line(LEFT, k, j))
							{
								return TRUE;
							}
						}
					}
				}
			}
		}
		else
		{
			for(i=x; i<x+20; i++)
			{
				if(IS_BLACK(i,j) && !IS_BLACK(i+1,j))
				{
					for(k=i+1; k<MAXWIDTH; k++)
					{
						if(!IS_BLACK(k,j) && IS_BLACK(k+1,j))
						{
							if(find_broken_line(RIGHT, k, j))
							{
								return TRUE;
							}
						}
					}
				}
			}
		}
	}
	return FALSE;
}

int find_broken_line(int rl_info, int x, int y)
{
	int i,j,offset = x;

#ifdef TRACE
	printf("in find broken line\n");
#endif

	if(rl_info == LEFT)
	{
		for(j=y+1; j<CUTLINE; j++)
		{
			if(IS_BLACK(offset, j))
			{
				for(i=offset; i<MAXWIDTH-1; i++)
				{
					if(IS_BLACK(i,j) && !IS_BLACK(i+1,j))
					{
						offset = x+1;
						j++;
						break;
					}
				}
				if(i > MIDWIDTH)
				{
					if(j < y+5 && j > CUTLINE_CL)
						return FALSE;
					else
						return TRUE;
						
				}
			}
			else
			{
				for(i=offset; i>0; i--)
				{
					if(!IS_BLACK(i,j) && IS_BLACK(i-1,j))
					{
						offset = x;
						j++;
						break;
					}
				}
				if(i < MIDWIDTH)
				{
					if(j < y+5 && j > CUTLINE_CL)
						return FALSE;
					else
						return TRUE;
				}
			}
		}
	}
	else
	{
		for(j=y+1; j<CUTLINE; j++)
		{
			if(IS_BLACK(offset, j))
			{
				for(i=offset; i>=0; i--)
				{
					if(IS_BLACK(i,j) && !IS_BLACK(i-1,j))
					{
						offset = x-1;
						j++;
						break;
					}
				}
				if(i < MIDWIDTH)
				{
					if(j < y+5 && j > CUTLINE_CL)
						return FALSE;
					else
						return TRUE;
				}
			}
			else
			{
				for(i=offset; i<MAXWIDTH; i++)
				{
					if(!IS_BLACK(i,j) && IS_BLACK(i+1,j))
					{
						offset = x;
						j++;
						break;
					}
				}
				if(i > MIDWIDTH)
				{
					if(j < y+5 && j > CUTLINE_CL)
						return FALSE;
					else
						return TRUE;
				}
			}
		}
	}
	return FALSE;
}

int get_road_angle()
{
	int i;
	int sum = 0;
#ifdef TRACE
	printf("get road angle\n");
#endif

#ifdef DRIVE_DEBUG
	printf("pt_cnt : %d\n", pt_cnt);
#endif

	for(i = 0; i<pt_cnt-1; i++)
	{
		int angle;
		angle = get_angle( pt[i], pt[i+1] );
		angles[i] = angle;
#ifdef DRIVE_DEBUG
		printf("angle[%d]:%d ", i, angle);
#endif
		sum += angle;
	}
#ifdef DRIVE_DEBUG
	printf("\n");
#endif

	if(pt_cnt  == 1)
		return 1000;

	return sum/(pt_cnt-1);
}

int get_angle( struct p_point a, struct p_point b)
{
	int ret = (int)(atan2((double)(b.y-a.y), (double)(b.x-a.x)) * 180 / PI);
	if(ret < 0)
		return ret + 180;
	else if(ret > 180)
		return ret - 180;
	return ret;
}

int check_traffic_light()
{
	// 정지선에 의한 차량 정지 시 신호등 확인 부분 수행
	// return으로 정지(NONE), 좌회전(LEFT), 우회전(

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
	int i;
#ifdef TRACE
	printf("init point\n");
#endif
	for( i = 0; i< PT_SIZE; i++){
		pt[i].y	= -1;
		pt[i].x = -1;
	}
	direct = NONE;
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

void init_values(int handle,struct image_data* idata)
{
	int i=0;
#ifdef TRACE
	printf("init values\n");
#endif
	if(idata == NULL)
		idata = (struct image_data*)malloc(sizeof(struct image_data));

	init_point();
	//width_scan_point = get_width_scan_point();
	width_scan_point = MIDWIDTH;
	find_left = FL_NONE;
	find_right = FL_NONE;

	// 버퍼 초기화 
	struct pxa_camera* camera = (struct pxa_camera*)cm_handle;
	if(camera->ref_count > 0){
		camera_release_frame(cm_handle,vidbuf);
	}
	
	vidbuf = camera_get_frame(cm_handle);
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
	for(j = CUTLINE ; j>=0 ; j--)
	{
		printf("%3d:",j);

		for(i = MAXWIDTH-1; i>=0; i--)
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
		}
		printf("\n");
	}
	printf("\n");
}

int init_camera(){
	int len;

	cm_handle = camera_open(NULL, 0);
	if(cm_handle < 0)
		return -1;

	//	system("echo b > /proc/invert/tb"); //LCD DriverIC top-bottom invert ctrl

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


