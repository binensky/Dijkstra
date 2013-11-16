#ifndef _AI_PROCESS_H__
#define _AI_PROCESS_H__

#include <pthread.h>
#include <stdio.h>

#include "miso_car_lib.h"
#include "miso_values.h"
#include "miso_camera.h"
#include "sensor_handler.h"
#include "parking_modules.h"
#include "drive_modules.h"

void init_ai_camera();

int ai_img_process(int flag){ 

	init_ai_camera(cm_handle);

	if( flag == IF_RED_STOP){
		int red_flag = red_pixel_check(90,180);
		if(red_flag != MID_NONE)
			return red_flag;// IF_STOP(0)
	}

	if( flag == IF_SG_STOP || flag == IF_SG_LEFT || flag == IF_SG_RIGHT){
		return check_traffic_light();// IF_SG_STOP(40), IF_SG_LEFT(44), IF_SG_RIGHT(45) 
	}

	return 0;            
}                                 


void init_ai_camera()
{
	int i;
	// 버퍼 초기화 
	struct pxa_camera* camera = (struct pxa_camera*)cm_handle;

	if(g_first){
		g_first = FALSE;
		for( i = 0; i < 5; i++){
			vidbuf = camera_get_frame(cm_handle);
			camera_release_frame(cm_handle,vidbuf);
		}
	}

	if(camera->ref_count>0){
		camera_release_frame(cm_handle,vidbuf);
	}
	vidbuf = camera_get_frame(cm_handle);

}    

void ai_event_proc(int flag)
{

	//check_modules... 
	if(flag ==  IF_CL_RIGHT || flag == IF_CL_LEFT){
		printf(">>>>>> ai  change line  %d\n", flag);
		distance_set(100);
		usleep(10000);
		change_course();
	}else if(flag == IF_PARK_V || flag == IF_PARK_H){
		stop();
		park_mode = PARK_ON;
		parking(flag);
	}else	// not module , but need to image processing.. 
	{
		while(TRUE){
			init_ai_camera();
			if(flag == IF_WHITE_SPEED_DOWN){
				speed_set(500);
				usleep(10000);
				distance_set(500);
				usleep(10000);
				forward_dis();
				usleep(10000);
				turn_straight();
				pthread_create(&thread[1],NULL,sensor_handler,NULL);
			
				while(g_drive_flag != DF_STOP)
				{	
					distance_set(200);
					usleep(10000);
					forward_dis();
				}
				stop();
				break;
			}else if(flag == IF_RED_STOP){
				stop();
				flag = red_pixel_check(90,180);
				printf("red pix check flag %d\n",flag);
				if( flag == MID_NONE)
					break;

			}else if( flag==IF_SG_STOP || flag==IF_SG_LEFT || flag==IF_SG_RIGHT){
				stop();
				flag = check_traffic_light();
				if( flag == IF_SG_STOP)
					continue;
				else{
					traffic_drive(flag);
					break;
				}
			}else 
				break;
		}

	}
}	

struct drive_data*  arrange_drive_data(struct drive_data* d_data, int index)
{
	int i,j;
	struct drive_data* arr_data = (struct drive_data*)malloc(sizeof(struct drive_data)*index);

	// flag, mid_flag, angle, dist, speed. 
	//  stage 1. pre processing input drive data SET FLAGS.
	for( i = 0; i < index ; i++){
		// set flags.. angle straight. 
		if (d_data[i].angle = DM_STRAIGHT){

			d_data[i].flag = IF_STRAIGHT;
			d_data[i].mid_flag = MID_STRAIGHT;
		}else{ // others.. no pre-processing.. 
		}
	}

	// stage 2. merge drive data into arr_data.
	for(i = 0, j = 0; j < index; i++, j++){

		// merge case 1: straight -> straight 
		if( i>0 && d_data[i].flag == IF_STRAIGHT && d_data[j].flag == IF_STRAIGHT){

			// when previous and current data have same speed , merge dist data. 
			if( arr_data[i].speed == d_data[j].speed){
				arr_data[i-1].dist += d_data[j].dist;
				i-=1;
			}
		}else{ // set other
			arr_data[i].flag = IF_STRAIGHT;
			arr_data[i].mid_flag = MID_STRAIGHT;
			arr_data[i].angle = DM_STRAIGHT;
			arr_data[i].dist = d_data[j].dist;
			arr_data[i].speed = d_data[j].speed;

			// no merge.. 
		}
	}
	return arr_data;
}

#endif
