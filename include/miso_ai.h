#ifndef _AI_PROCESS_H__
#define _AI_PROCESS_H__

#include <stdlib.h>
#include <math.h>
#include <pxa_lib.h>
#include <pxa_camera_zl.h>

#include "miso_car_lib.h"
#include "miso_values.h"
#include "miso_camera_print.h"
#include "cam_values.h"

int red_pixel_check(const int BOT_Y, const int TOP_Y);
int check_traffic_light();

int ai_img_process(struct image_data* img_data){ 

        init_ai_values(cm_handle);

	if( img_data->flag == MID_STOP){
		int red_flag = red_pixel_check(90,180);
		if(red_flag != MID_NONE)
			return red_flag;// IF_STOP(0)
	}
	
	if( img_data->flag == IF_TRAFFIC){
		return check_traffic_light();// IF_SG_STOP(40), IF_SG_LEFT(44), IF_SG_RIGHT(45) 
	}
 
       return 0;            
}                                 


void init_ai_values(int handle)
{
	// 버퍼 초기화 
	struct pxa_camera* camera = (struct pxa_camera*)cm_handle;

	for( i = 0; i < 5; i++){
		vidbuf = camera_get_frame(cm_handle);
		camera_release_frame(cm_handle,vidbuf);
	}

	if(camera->ref_count>0){
		camera_release_frame(cm_handle,vidbuf);
	}
	vidbuf = camera_get_frame(cm_handle);

}    

int red_pixel_check(const int BOT_Y, const int TOP_Y){
	int i,j,red_bot=-1,red_cnt=0;

	for(i = BOT_Y ; i < TOP_Y ; i++){
		for( j = 0 ; j < MAXWIDTH ; j+=3){
			if(IS_RED(j,i)){
				if( red_bot == -1)
					red_bot = i;
				red_cnt+=1;
			}
		}
	}
	if( red_cnt > 100)
	{
		if(red_bot <= 120){
			return MID_STOP;
		}else if(120 < red_bot && red_bot <= 170){
			return MID_SPEED_DOWN;
		}
	}
	return MID_NONE;
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
	else if(yellow_count >= 100){	
		printf("COLOR : YELLOW!\n");
		return IF_SG_STOP;
	}
	else if(green_count >= 250){
		printf("RIGHT TURN!\n");
		return IF_CL_RIGHT;
	}
	else if(green_count >= 100){
		printf("LEFT TURN!\n");
		return IF_CL_LEFT;
	}
	else
		return IF_SG_STOP;
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
