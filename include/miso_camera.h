#ifndef __CAMERA_H__
#define __CAMERA_H__

#include <stdlib.h>
#include <math.h>
#include <pxa_lib.h>
#include <pxa_camera_zl.h>

#include "miso_car_lib.h"
#include "miso_values.h"
#include "miso_camera_print.h"
#include "cam_values.h"

void init_values(int handle,struct image_data* idata);
void init_point();

int red_pixel_check(const int BOT_Y, const int TOP_Y);
int get_road_angle();
int get_angle(struct p_point a, struct p_point b);

void find_left_line();
void find_right_line();

int left_line_trace(int i, int offset);
int right_line_trace(int i, int offset);

int left_line_check(int h);
int right_line_check(int h);

int find_inline(int rl_info, int i, int offset);
int find_outline(int rl_info, int y, int w);
int find_in_point(int rl_info, int i, int offset);
int find_end_point( int i, int offset);
int set_end_point(int rl_info, struct p_point* pt_tmp, int flag, int* direct);

int check_mid_line();
int check_speed_bump(int w,int y);
int check_change_line(int rl_info, int x, int y);
int find_broken_line(int rl_info, int x, int y);

struct image_data* make_image_data(struct image_data* img_data);
struct image_data* left_set_image_data(struct image_data* img_data, char is_straight);
struct image_data* right_set_image_data(struct image_data* img_data, char is_straight);

int angles[PT_SIZE-1];
int pt_cnt;			// left pt, right pt,
int g_first = TRUE;

struct image_data* line_check()
{
	struct image_data* img_data = (struct image_data*)malloc(sizeof(struct image_data));

	init_values(cm_handle,img_data);
#ifdef DEBUG
	//print_screen_y();//print_screen_org();//print_screen_cb();//print_screen_cr();//print_screen_color();//print_traffic_light();//exit(0);
#endif
	printf("> drive flag : %d\n",g_drive_flag);

	switch(g_drive_flag)
	{
		case DF_DRIVE:
			img_data->mid_flag = check_mid_line();	// mid flag set. 
			printf(">> mid flag : %d\n", img_data->mid_flag);
			return make_image_data(img_data);	// img flag set. 

		case DF_STOP:
			img_data->flag = check_traffic_light();
			printf(">> traffic falg : %d\n", img_data->flag);
			return img_data;

		default:
			img_data->flag= IF_NO_PROCESS;
			return img_data;
	}
	return NULL;
}

int check_mid_line()
{
	int i,j;
	int red_cnt = 0, change_line_cnt = 0;
	int red_flag;

#ifdef TRACE
	printf(" >>> check_mid_line\n");
#endif
	red_flag = red_pixel_check(70,150);
	if(red_flag == MID_SPEED_DOWN)
		return MID_SPEED_DOWN;	// 23
	else if(red_flag == MID_STOP)
		return MID_STOP;	// 0

	// 미드라인 수직 검사 
	for(i=1; i < CUTLINE; i++){
		if(!IS_BLACK(MIDWIDTH, i)){
			int speed_bump_ret;

			printf("mid height %d \n", i);

			// speed bump check 
			if( 0 < (speed_bump_ret = check_speed_bump(MIDWIDTH,i)))
				return speed_bump_ret;

			// det mid flag
			if( i <= CUTLINE_OUTLINE )
				return MID_OUTLINE;	// 31
			else if( i <= CUTLINE_CURVE)
				return  MID_CURVE;	// 3
			else if( CUTLINE_CURVE < i && i <= CUTLINE)
				return MID_CURVE_STRAIGHT;	// 2
			else if(IS_WHITE(MIDWIDTH,i)) 
			{
				int k, w_cnt=0, y_cnt=0;

				for( k = i ; k < CUTLINE-1; k++){
					if( IS_BLACK(MIDWIDTH,k+1) && !IS_BLACK(MIDWIDTH,k) )
						break;
					if(IS_WHITE(MIDWIDTH,k))	w_cnt++;
					else if(IS_YELLOW(MIDWIDTH,k))	y_cnt++;
					else 				continue;
				}
				if(w_cnt > y_cnt && w_cnt > 10)
					return MID_WHITE_SPEED_DOWN;	//24
			}
			else 
				break;	// 1
		}else{// is black 
			continue;
		}
	} // end for
	return MID_STRAIGHT;	// 1
}

struct image_data* make_image_data(struct image_data* img_data){

	int i  = 0;

	if( img_data->mid_flag == MID_STRAIGHT){	// mid flag가 직선일때 
		for(i = 1; i< CUTLINE_POINT ; i++){	// 컷라인 포인트 까지 올라가면서 양쪽선 찾는다. 
			if((find_left == FL_NONE) && left_line_check(i)){
				// img_data setting with left line check result.
				left_set_image_data(img_data,TRUE);	 
			}

			if((find_right == FL_NONE) && right_line_check(i)){
				// img_data setting with left line check result.
				right_set_image_data(img_data,TRUE);
			}
		}
		if(find_left == FL_NONE && find_right == FL_NONE)	// find all
			img_data->flag = IF_STRAIGHT;
		else if( find_left != FL_NONE && find_right != FL_NONE) // not find. (= else)
			img_data->flag = IF_STRAIGHT;

		return img_data;

	}else if( img_data->mid_flag == MID_CURVE || img_data->mid_flag == MID_CURVE_STRAIGHT)
	{
		for(i = 1; i< CUTLINE_POINT ; i++){
			if((find_left == FL_NONE) && left_line_check(i)){
				return left_set_image_data(img_data,FALSE);
			}else if((find_right == FL_NONE) && right_line_check(i)){
				return right_set_image_data(img_data,FALSE);
			}else	continue;

		}
		if( find_left != FL_NONE && find_right != FL_NONE) // not find. else of branchs.
			img_data->flag = IF_STRAIGHT;

		return img_data;

	}else	// other mid_flags are equal img_data->mid_flag to img_data->flag. 
	{
		img_data->flag = img_data->mid_flag;
		return img_data;
	}
}


struct image_data* left_set_image_data(struct image_data* img_data, char is_straight)
{
	int i;

	if(is_straight && !had_change_line && is_broken_linee)
	{
		img_data->flag = IF_CL_LEFT;
		had_change_line = TRUE;
		return img_data;
	}

	img_data->flag = IF_LEFT;
	img_data->angle[LEFT] = get_road_angle();
	img_data->bot[LEFT].x = pt[BOT].x;
	img_data->bot[LEFT].y = pt[BOT].y;

	init_point();
	return img_data;
}

struct image_data* right_set_image_data(struct image_data* img_data, char is_straight)
{				
	int i;

	if( is_straight && !had_change_line && is_broken_linee)
	{
		img_data->flag = IF_CL_RIGHT;
		had_change_line = TRUE;
		return img_data;
	}

	img_data->flag = IF_RIGHT;
	img_data->angle[RIGHT] = get_road_angle();
	img_data->bot[RIGHT].x = pt[BOT].x;
	img_data->bot[RIGHT].y = pt[BOT].y;

	init_point();
	return img_data;
}




// 양 옆으로 가면서 선을 찾는다. 
int left_line_check(int i)
{
	int w;
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
	printf("find inline info :%d \n", rl_info);
#endif
	if(rl_info == LEFT)	// left line
	{
		pt[BOT].y = y;
		pt[BOT].x = w;
		pt_cnt += 1;

#ifdef DRIVE_DEBUG
		printf("set bot point <%d><%d>\n",w,y);
#endif

		if(find_in_point(LEFT, y, pt[BOT].x)) //  set pt array.
		{
			find_left = FL_FIND;
			return TRUE;
		}else{
			return FALSE;
		}
	}else 			// right line
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

				if(find_end_point(y, pt[BOT].x)) // sub_point를 찾는다. 
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

				if(find_end_point(y, pt[BOT].x))
				{
					find_right = FL_FIND;
					return TRUE;
				}else 
					return FALSE;
			}
		}
	}

}

int red_pixel_check(const int BOT_Y, const int TOP_Y){
	int i,j,red_bot=-1,cnt=0;
	for(i = BOT_Y ; i < TOP_Y ; i++){
		for( j = 0 ; j < MAXWIDTH ; j+=3){
			if(IS_RED(j,i)){
				if( red_bot == -1)
					red_bot = i;
				cnt+=1;
			}
		}
	}
	if( cnt > 100)
	{
		if(i < 120){
#ifdef DRIVE_DEBUG
			printf("MID_STOP in %d\n", i);
#endif
			return MID_STOP;
			}
		else if(i > 140){
#ifdef DRIVE_DEBUG
			printf("SLow Down in %d\n", i);
#endif
			return MID_SPEED_DOWN;

		}
	}
	return MID_NONE;
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
			printf("------------------------cnt : %d\n", cnt);
#endif
			cnt += 1;		
		}
		if(cnt >= 4){
			return TRUE;
		}
	}

	return FALSE;
}



int left_line_trace(int i, int offset)
{
	int x,y;
	int direct = NONE;
	const int above_pix_0 = 0;
	const int above_pix_1 = 1;
	struct p_point pt_tmp, broken_pt;

	for( y = i+1; y < CUTLINE ; y++){
		if(!IS_BLACK(offset,y)){		// 위 점이 1인 경우 오른쪽으로 진행중. 오른쪽으로 돌면서 ->(1,0)을 찾는다. 
			for( x = offset; x>0; x--){	// (1,0)을 찾은 경우 점을 저장하고 offset을 갱신한다.  
				if( !IS_BLACK(x,y) && IS_BLACK(x-1,y) ){
					if(pt[pt_cnt-1].y + 10 == y){
						pt[pt_cnt].y = y;
						pt[pt_cnt].x = x;
						pt_cnt += 1;
					}else if(pt_cnt == 1){
						pt[pt_cnt].y = y;
						pt[pt_cnt].x = x;
					}
					pt_tmp.y = y;
					pt_tmp.x = x;
					offset = x;

					if(set_end_point(LEFT, &pt_tmp, above_pix_1, &direct)){
						if(pt_cnt == 1 && pt[pt_cnt].x != -1)
							pt_cnt += 1;
						return TRUE;
					}
					break;
				}
			}if(x == 0){			// 오른쪽에서 1,0 을 못 찾은 경우
				if(check_change_line(LEFT, pt_tmp.x, pt_tmp.y)){
					is_broken_linee = TRUE;
					return TRUE;
				}
				else if(pt[BOT+1].x == -1){
					init_point();
					return FALSE;
				}
				else{
					if(pt_cnt == 1)
						pt_cnt += 1;
					return TRUE;
				}
			}
		}else{					// 위 점이 0인 경우 왼쪽으로 순회하면서 (1,0)<- 찾는다. 
			for( x = offset; x<MAXWIDTH-1; x++){
				if( IS_BLACK(x,y)  && !IS_BLACK(x+1,y)){
					if(pt_cnt == 1){
						pt[pt_cnt].y = y;
						pt[pt_cnt].x = x;
					}
					if(pt[pt_cnt-1].y + 10 == y){
						pt[pt_cnt].y = y;
						pt[pt_cnt].x = x+1;
						pt_cnt += 1;
					}

					pt_tmp.y = y;
					pt_tmp.x = x+1;
					offset = x+1;

					if(set_end_point(LEFT, &pt_tmp, above_pix_0, &direct)){
						if(pt_cnt == 1 && pt[pt_cnt].x != -1)
							pt_cnt += 1;
						return TRUE;
					}

					break;
				}
			}
			if(x == MAXWIDTH-1){		// 왼쪽에서 (10)을 못 찾은 경우
				if(check_change_line(LEFT, pt_tmp.x, pt_tmp.y)){
					is_broken_linee = TRUE;
					return TRUE;
				}else if(pt[BOT+1].x == -1){
					init_point();
					return FALSE;
				}else{
					if(pt_cnt == 1)
						pt_cnt += 1;
					return TRUE;
				}
			}
		}
	}// end scan for top

}

int right_line_trace(int i, int offset)
{
	int x,y;
	int direct = NONE;
	const int above_pix_0 = 0;
	const int above_pix_1 = 1;
	struct p_point pt_tmp, broken_pt;

	for( y = i+1; y < CUTLINE ; y++){
		if( IS_BLACK(offset,y)){
			for( x = offset ; x >0 ; x--){
				if(!IS_BLACK(x,y) && IS_BLACK(x-1,y)){
					if(pt[pt_cnt-1].y + 10 == y){
						pt[pt_cnt].y = y;
						pt[pt_cnt].x = x-1;
						pt_cnt += 1;
					}
					else if(pt_cnt == 1){
						pt[pt_cnt].y = y;
						pt[pt_cnt].x = x;
					}

					pt_tmp.y = y;
					pt_tmp.x = x-1;
					offset = x-1;		

					if(set_end_point(RIGHT,&pt_tmp,above_pix_0, &direct)){
						if(pt_cnt == 1 && pt[pt_cnt].x != -1)
							pt_cnt += 1;
						return TRUE;
					}
					break;
				}
			}if(x == 0){		// 오른쪽에서 (0,1) 을 못 찾은 경우
				if(check_change_line(RIGHT, pt_tmp.x, pt_tmp.y)){
					is_broken_linee = TRUE;
					return TRUE;
				}
				else if(pt[BOT+1].x == -1){
					init_point();
					return FALSE;
				}else{
					if(pt_cnt == 1)
						pt_cnt += 1;					
					return TRUE;
				}
			}
		}else{ 			// 위 점이 1인 경우 왼쪽으로 순회하면서 0,1 찾기
			for( x = offset; x<MAXWIDTH-1; x++){
				if( !IS_BLACK(x,y) && IS_BLACK(x+1,y)){
					if(pt[pt_cnt-1].y + 10 == y){
						pt[pt_cnt].y = y;
						pt[pt_cnt].x = x;
						pt_cnt += 1;
					}else if(pt_cnt == 1){
						pt[pt_cnt].y = y;
						pt[pt_cnt].x = x;
					}else{
						pt_tmp.y = y;
						pt_tmp.x = x;
						offset = x;
					}
					if(set_end_point(RIGHT,&pt_tmp,above_pix_1, &direct)){
						if(pt_cnt == 1 && pt[pt_cnt].x != -1)
							pt_cnt += 1;
						return TRUE;
					}
					break;
				}
			}if(x == MAXWIDTH-1){// 왼쪽에서 (0,1)을 못 찾은 경우
				if(check_change_line(RIGHT, pt_tmp.x, pt_tmp.y)){
					is_broken_linee = TRUE;
					return TRUE;
				}else if(pt[BOT+1].x == -1){
					init_point();
					return FALSE;
				}else{
					if(pt_cnt == 1)
						pt_cnt += 1;
					return TRUE;
				}
			}
		}
	} // end scan for top 
} 

int check_speed_bump(int w, int y)
{
	int current_color = ( !IS_BLACK(w,y)? COL_YELLOW : COL_WHITE );
	int i, j;
	int speed_bump_count = 0;

#ifdef TRACE
	printf(">>>> check_speed_bump %d, %d\n", w, y);
#endif

	for(j = y; j <= CUTLINE-40; j++)
	{
		speed_bump_count = 0;

		// count 
		for(i = MAXWIDTH-2; i>0; i--)
		{
			if(IS_BLACK_SPEED_BUMP(i,j))
				break;
			if(IS_YELLOW_SPEED_BUMP(i,j) && (current_color == COL_WHITE)){
				current_color = COL_YELLOW;
				speed_bump_count++;
			}
			else if(IS_WHITE_SPEED_BUMP(i,j) && (current_color == COL_YELLOW)){
				current_color = COL_WHITE;
				speed_bump_count++;
			}
		}

		if(g_index>0 && d_data[g_index-1].mid_flag == MID_SPEED_BUMP_ST)
			return MID_SPEED_BUMP_ST;	// speed bump check.

		else if(g_index>0 && d_data[g_index-1].mid_flag == MID_SPEED_BUMP_CUR)
		{
			if(speed_bump_count == 6)
				return MID_SPEED_BUMP_ST;
			else
				return MID_SPEED_BUMP_CUR;
		}
		else if( speed_bump_count >= 4)
			return MID_SPEED_BUMP_CUR;
		else 
			continue;
	}

	return -1;
}


int find_in_point(int rl_info, int y, int offset)
{

#ifdef TRACE
	printf("find in point\n");
#endif

	if(rl_info == LEFT){
		return left_line_trace(y, offset);	
	}
	else{
		return right_line_trace(y, offset);
	}
}


int find_end_point(int i, int offset)
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

int set_end_point(int rl_info, struct p_point* pt_tmp, int flag, int* direct)
{
	if(*direct == NONE)
	{
		if( pt_tmp->x > pt[pt_cnt-1].x)
			*direct = LEFT;
		else if(pt_tmp->x < pt[pt_cnt-1].x)
			*direct = RIGHT;
	}
	else
	{
		if(*direct == LEFT && pt_tmp->x < pt[pt_cnt-1].x)
		{
			if(pt_cnt != 1)
			{
				pt_cnt -= 1;
				pt[pt_cnt].x = -1;
				pt[pt_cnt].y = -1;
			}
			return TRUE;
		}
		else if(*direct == RIGHT && pt_tmp->x > pt[pt_cnt-1].x)
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

	for(j=y; ( j<y+50 && j < CUTLINE_CL ); j++)
	{
#ifdef trace
		printf("in check change line\n");
#endif
		if( rl_info == LEFT){
			for(i=x; ( i>x-50 && i>=0 ); i--)
			{
				if(  !IS_BLACK(i+1,j) && IS_BLACK(i,j))
				{
					if(find_broken_line(LEFT, k, j))
					{
						return TRUE;	
					}
				}
			}
		}
		else
		{
			for(i=x; i<x+50; i++)
			{
				if(IS_BLACK(i,j) && !IS_BLACK(i+1,j))
				{
					if(find_broken_line(RIGHT, k, j))
					{
						return TRUE;
					}
				}
			}
		}
	}
#ifdef trace
	printf("out check change line\n");
#endif


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
		return left_line_trace(y, x);
	}
	else
	{
		return right_line_trace(y, x);
	}
	return FALSE;
}

int check_traffic_light()
{
	int i, j;
	int red_count = 0, yellow_count = 0, green_count = 0;

	// cutline 위 부분을 확인
	for(i = MAXWIDTH -1 ; i >= 0; i--)
	{
		for(j = MAXHEIGHT-1 ; j> 170; j--)
		{
			// Y, CB, CR값 테스트로 Red, Green, Yellow 구분 (+ 좌회전 화살표?)
			if(IS_TRAFFIC_RED(i,j))
				red_count++;
			else if(IS_TRAFFIC_GREEN(i,j))	// 초록불인 경우
				green_count++;
			else if(IS_TRAFFIC_YELLOW(i,j))
				yellow_count++;
		}
	}

	if(red_count >= 100)
		return IF_SG_STOP;
	else if(yellow_count >= 100)
		printf("COLOR : YELLOW!\n");
	else if(green_count >= 250)
		printf("RIGHT TURN!\n");
	else if(green_count >= 100)
		printf("LEFT TURN!\n");
	else
		return IF_SG_STOP;
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
	for(i=0; i<pt_cnt; i++)
		printf("pt[%d]:(%d,%d) ", i, pt[i].x, pt[i].y);
	printf("\n");

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
		return 1000; //...

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
	pt_cnt = 0;
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

	if(g_first)
	{
		g_first = FALSE;
		for( i = 0; i < 5; i++)
		{
			vidbuf = camera_get_frame(cm_handle);
			camera_release_frame(cm_handle,vidbuf);
		}
	}

	if(camera->ref_count > 0){
		camera_release_frame(cm_handle,vidbuf);
	}
	vidbuf = camera_get_frame(cm_handle);
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


