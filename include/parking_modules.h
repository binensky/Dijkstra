
#ifndef _PARKING_MODULES_H
#define _PARKING_MODULES_H

#include "miso_car_lib.h"
#include "miso_values.h"

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

#define AVR_CNT 5

#define CLOSE_DIST 80
#define FAR_DIST 60

#define VERTICAL_DIST 1700
#define OVER_DIST 5000
#define UNDER_DIST 800

#define PARK_NONE 0
#define PARK_FIND 1
#define PARK_INFO_SAVE 2
#define PARK_WAITING 3
#define PARK_READY 4
#define PARK_ON 5

#define PARK_VER 61
#define PARK_HOR 62

static int park_mode = PARK_NONE;
static int distFD = 0;
unsigned short rxbuf[4];

int get_dist_sensor(int index);
void park_ready();
void check_car_angle();

void* parking_distance_check(void* p_data)
{
	int tmp;

	if((distFD = open("/dev/FOUR_ADC", O_RDONLY )) < 0)
	{      
		perror("open faile /dev/FOUR_ADC");
		exit(-1);
	}

	int i = 0;
	while(1)
	{	
		i++;
		tmp= get_dist_sensor(2);
		printf("tmp[%d] : %d \n",i,tmp);
	}
}

void* parking_check(void* p_data)
{
	int i;
	int prev_dist = -1;
	int front_dist[2];
	int vh_info = PARK_NONE;
	int park_dist = mDistance() - prev_dist;

	if((distFD = open("/dev/FOUR_ADC", O_RDONLY )) < 0)
	{      
		perror("open faile /dev/FOUR_ADC");
		exit(-1);
	}

	for(i=0; i<10; i++){
		get_dist_sensor(2);
	}
	
	while(1)
	{
		if(g_wait_thread == WAIT_THREAD || g_wait_thread == END_THREAD)
			continue;

		//printf(">mode : %d / dist(2) %d / dist(3) %d\n",park_mode, get_dist_sensor(2),get_dist_sensor(3)); 
		switch( park_mode )
		{
			case PARK_NONE:
				// 주차 시작점을 잡는다.
				if( g_drive_mode == CM_MODE && get_dist_sensor(3) >= CLOSE_DIST )
				{
					printf("/////// PARK_NONE -> PARK_FIND\n");
					front_dist[0] = get_dist_sensor(3);
					park_mode = PARK_FIND;
				}
				// 모드를 바꿔줘서 다음 플래그로 이동한다. 
				break;
			case PARK_FIND:
				// 다시 멀어지는 지점을 지난다. 
				if( get_dist_sensor(3) <= FAR_DIST )
				{
					prev_dist = mDistance();
					printf("////// PARK_FIND -> PARK_INFO_SAVE\n");
					park_mode = PARK_INFO_SAVE;
				}
				break;
			case PARK_INFO_SAVE:
				if( (g_park_dis = get_dist_sensor(3)) >= CLOSE_DIST)
				{
					// check dist range 
					park_dist = mDistance() - prev_dist;

					if(park_dist > OVER_DIST || park_dist < UNDER_DIST)	// 이정도면 잘못 잡은거 
					{
						printf("it is not parking area!\n");
						park_mode = PARK_NONE;
						continue;
					}	
					else if(park_dist < VERTICAL_DIST)
						g_drive_flag = PARK_VER;
					else
						g_drive_flag = PARK_HOR;
					printf(">>> mDistance() : %d, ver / hor : %d\n",park_dist, g_drive_flag);
					// 어느 주차모드인지 H인지 V인지 확인한다.
					stop();

					front_dist[1] = get_dist_sensor(3); // 다시 거리가 가까워 진다면 dist를 잡는다. 
					printf(">>>> diff dists %d - %d = %d \n", front_dist[0], front_dist[1], front_dist[0]-front_dist[1]);

					// 차이를 구하고 그 값으로 차를 돌린다. 
					//check_car_angle(front_dist[0], front_dist[1]);
					//park_mode = PARK_WAITING;
					printf("////// PARK_INFO_SAVE -> PARK_ON\n");
					park_mode = PARK_ON;
				}
				break;

			// 4단계로 축소 (앞쪽 센서 사용 x)
			/*
			case PARK_WAITING:
				if( get_dist_sensor(2) >= CLOSE_DIST && get_dist_sensor(3) <=FAR_DIST)
				{
					park_mode = PARK_READY;
				}

				break;

			case PARK_READY:
				if( get_dist_sensor(3) >= CLOSE_DIST_SEN3)
				{
					stop();
					g_drive_flag = vh_info;
					park_mode = PARK_ON;
					printf("ready for parking here??\n");
					// run module in the main with flag
				}
				break;
			*/

			case PARK_ON:
				if(g_drive_flag == DF_DRIVE)
				{
					vh_info = PARK_NONE;
					park_mode = PARK_NONE;
					printf("////// PARK_ON -> PARK_NONE\n");
					
					if(g_wait_thread == INIT_THREAD)
						g_wait_thread = WAIT_THREAD;
					else
					{
						g_wait_thread = END_THREAD;
						had_change_line = FALSE;
						sleep(1000);
					}
				}
				break;
			default:
				break;
		}
	}
	close(distFD);
}

void park_ready(int index)
{
	int i = 0;
	int tmp_dist;

	// 값의 범위에 따라서 dm_angle을 설정을 한다. 

	tmp_dist = get_dist_sensor(index);
	if(tmp_dist <= 3000 )
	{
		if(index == 2)
			turn_set(DM_STRAIGHT-100);
		else if(index ==3)
			turn_set(DM_STRAIGHT+100);
	}else if( tmp_dist 
			//	turn_set(DM_STRAIGHT+200);
			//	turn_set(DM_STRAIGHT-200);
			//	turn_set(DM_STRAIGHT);
		)
	{}
}

void check_car_angle()
{

	while(get_dist_sensor(2) >= CLOSE_DIST)
	{}

}


int get_dist_sensor(int index)
{
	int i = 0; 
	int total = 0;

	// 가까우면 큰 값이 나오고 멀리 있으면 작은 값이 나온다. 
	while(i<AVR_CNT)
	{
		read(distFD, rxbuf, sizeof(rxbuf));	// ADC READ
		//printf(" %d %d ",index,(int)rxbuf[index-1]);
		total+=(int)rxbuf[index-1];
		i++;
	}
	return total/AVR_CNT;
}


void park_init()
{
	int i;
	if((distFD = open("/dev/FOUR_ADC", O_RDONLY )) < 0)
	{      
		perror("open faile /dev/FOUR_ADC");
		exit(-1);
	}

	for(i=0; i<10; i++){
		get_dist_sensor(2);
		get_dist_sensor(3);
	}
}

void park_exit()
{
	fclose(distFD);	
}


void parking(int flag)
{
	int n;
	int tmp_dist;

	distance_reset();
	distance_set(100);
	if(flag == IF_PARK_V)
	{
	
		printf("in parking V\n");
		n = mDistance();
		usleep(10000);
		distance_set(2500);
		usleep(10000);
		speed_set(1500);
		usleep(10000);
		turn_set(DM_ANGLE_MIN);
		usleep(10000);
		backward_dis();
		usleep(10000);
		while(mDistance() - n > -1600){}
		turn_straight();
		usleep(10000);
		while(get_dist_sensor(4) < 170){}
		stop();
		sleep(1);
		buzzer_on();
		sleep(1);
		forward_dis();
		//while(get_dist_sensor(4) > 60){}
		while(get_dist_sensor(4) > 100){}
		turn_set(DM_ANGLE_MIN);
		n = mDistance();
		while(mDistance() - n < 2000){}
		turn_straight();
		speed_set(1000);
	}
	else
	{

		printf("in parking H\n");
////////////////////////////??????????????????????????????????????????????????? how????
		printf("g park dis : %d\n", g_park_dis);
		n = mDistance();
		distance_set(2500);
		speed_set(1300);
		backward_dis();
		while(mDistance() - n > -20){}
		turn_set(DM_ANGLE_MIN);
		while(mDistance() - n > -1500 - ( 240 - 2*g_park_dis)){}
		turn_set(DM_ANGLE_MAX);
		while(get_dist_sensor(4) < 290 && get_dist_sensor(3) < 290 ){}
		stop();
		sleep(1);
		buzzer_on();
		sleep(1);
		tmp_dist = get_dist_sensor(4);
		printf("tmp dist : %d\n", tmp_dist);
		forward_dis();
		while(get_dist_sensor(4) > 100 || get_dist_sensor(3) > 100 ){}
		turn_straight();
		n = mDistance();
		while(mDistance() - n < 1300 - (500 - tmp_dist)){}
		turn_set(DM_ANGLE_MIN);
		while(mDistance() - n < 2300){}
		turn_straight();
		speed_set(1000);
		//printf("back : %d, right : %d\n", get_dist_sensor(4), get_dist_sensor(3));
	}
	distance_reset();
	usleep(10000);
	g_drive_flag = DF_DRIVE;
}


#endif
