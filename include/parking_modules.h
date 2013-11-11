#include "miso_car_lib.h"


/*
	thread 함수 
	0. distacance를 계속해서 확인한다.
	1. distance가 가까워 지는 부분에서 속도를 줄인다.
	2. 앞으로 가면서 distance를 확인하는데 다시 멀어지는 부분이 나오면 mDistance를 기록한다.
	2. distance가 다시 가까워지는 부분에서 mDistance를 구해서 그 거리가 얼마냐에 따라서 
		수평주차를 할 것인지, 수직주차를 할 것인지 확인한다. 
	3. distance가 다시 멀어지는 부분에서 차를 멈추고 해당 모듈을 수행한다. 
	
	기타 (distance가 멀어지냐 가까워지냐에 따라서 수평을 맞추는 작업을 한다.) 	 
*/


static int on_park = 0;		// is parking now??


void *distance_check(void* p_data)
{

	int prev_dist = -1;

	while(1)
	{
		if(check_distance(3) <= 200)  // distance가 가까워 지는것 확인
		{
			if(on_park){ // 주차장을 잡은 상태라면

				if( mDistance - prev_dist > 300)  // 일정값 보다 크다면 수평주차
					d_data[g_index].flag = IF_PARK_H;
				else
					d_data[g_index].flag = IF_PARK_V;

				while(1)
				{

					int tmp_dis = check_distance(3);
					if(tmp_dis>=500) // 다시 멀어지는 지점을 확인
					{
						stop();
						// 주차 모드 해제(but main run parking modules)
						on_park = 0;		
					}else // 주차장과 수평을 맞추기 위한 작업 수행
					{
						if(tmp_dis > check_distance(3)) // 멀어지면
						{
							// 가벼운 우회전으로 수평을 맞춰야 함
						}else 
						{
							// 거리가 가까워 지면 좌회전으로 수평을 맞춰야 함.
						}
					}

				}
			}else{
				on_park = 1;  // 주차장을 잡았으면
				speed_set(500); // 속도를 줄인다. 

				// 주차공간의 시작점을 찾는다. 
				while(1){
					if(check_distance(3)>=500){
						// dm 거리값 초기화 후에 break;
						prev_dist = mDistance(); 
						break;
					}
					else{ // 주차장과 수평을 맞추기 위한 작업을 수행한다. 
						if(tmp_dis > check_distance(3)) // 멀어지면
						{
							// 가벼운 우회전으로 수평을 맞춰야 함
						}else 
						{
							// 거리가 가까워 지면 좌회전으로 수평을 맞춰야 함.
						}
					}
				}
			}
		}

	}
}
