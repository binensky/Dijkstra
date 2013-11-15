#ifndef _AI_PROCESS_H__
#define _AI_PROCESS_H__

#include <stdlib.h>
#include <math.h>
#include <pxa_lib.h>
#include <pxa_camera_zl.h>

#include "miso_car_lib.h"
#include "miso_values.h"
#include "miso_camera.h"

void init_ai_values(int handle);

int ai_img_process(int flag){ 

        init_ai_values(cm_handle);

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


void init_ai_values(int handle)
{
	int i;
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

void arrange_stored_data(struct drive_data* d_data, int index)
{
	int  i = 0;
}

#endif
