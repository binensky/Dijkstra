
#ifndef _PARKING_MODULES_H
#define _PARKING_MODULES_H

#include "miso_car_lib.h"
#include "miso_values.h"
#include <time.h>

#define AVR_CNT 5

#define CLOSE_DIST 70
#define FAR_DIST 60

#define VERTICAL_DIST 1700
#define OVER_DIST 7000
#define UNDER_DIST 800

#define PARK_START -2
#define PARK_START_FIND -1
#define PARK_NONE 0
#define PARK_FIND 1
#define PARK_INFO_SAVE 2
#define PARK_WAITING 3
#define PARK_READY 4
#define PARK_ON 5

static int park_mode = PARK_START;
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
	int vh_info = PARK_START;
	int park_dist;

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
			case PARK_START:
				if( get_dist_sensor(3) >= CLOSE_DIST)
				{
					park_mode = PARK_START_FIND;
				}
				break;
			case PARK_START_FIND:
				if( get_dist_sensor(3) <= FAR_DIST)
				{
					park_mode = PARK_NONE;
				}
				break;
			case PARK_NONE:
				// 주차 시작점을 잡는다.
				if( g_drive_mode == CM_MODE && get_dist_sensor(3) >= CLOSE_DIST )
				{
					printf("/////// PARK_NONE -> PARK_FIND\n");
					park_mode = PARK_FIND;

					//d_data[g_index].speed = 1000;
					speed_set(1000);
					usleep(1000);
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

					//g_drive_flag = DF_VPARK;

					if(park_dist > OVER_DIST || park_dist < UNDER_DIST)	// 이정도면 잘못 잡은거 
					{
						printf("it is not parking area!\n");
						park_mode = PARK_NONE;
						continue;
					}	
					else if(park_dist < VERTICAL_DIST)
						g_drive_flag = DF_VPARK;
					else
						g_drive_flag = DF_PPARK;
					printf(">>> mDistance() : %d, ver / hor : %d\n",park_dist, g_drive_flag);
					// 어느 주차모드인지 H인지 V인지 확인한다.

					stop();


					// 차이를 구하고 그 값으로 차를 돌린다. 
					//park_mode = PARK_WAITING;
					printf("////// PARK_INFO_SAVE -> PARK_ON > %d\n",g_drive_flag);
					park_mode = PARK_ON;
					//usleep(10000);
				}
				break;
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
void park_vertical(){
	int i = 0;
	distance_set(2500);
	usleep(1000);
	speed_set(1500);
	usleep(1000);
	turn_set(DM_ANGLE_MIN);
	usleep(1000);


	for( i = 0; i < 100; i++ ){
		//printf( "time:	%ld\n", getCurrentTimeMilli() );
		sleep(1);

	}

}





void parking(int flag)
{
	long n;
	int tmp_dist;

	int TIME_BACK_TURN = 600;
	int TIME_BACK = 300;
	int stopSensor = 250;
	int outSensor = 108;
	int backRightSensor = 170;
	int turnRightSensor = 170;
	int turnRight = 1700;

	int stopTime = 0;
	int speed = 2500;

	int angle_min = 850;
	int angle_max = 2150;

	/*
	   printf( "speed : " );
	   scanf("%d", &speed );
	   printf("sensor (right, in back, outback) : " );
	   scanf("%d %d %d", &rightSensor, &stopSensor, &outSensor );
	   printf( "backTime turnTime rTurn: ");
	   scanf( "%d %d %d", &TIME_BACK, &TIME_BACK_TURN, &turnRight );

	   TIME_BACK_TURN *= 1000;
	   TIME_BACK *= 1000;
	 */

	while( 0 ){
		printf( "%d : %d : %d \n", get_dist_sensor(2), get_dist_sensor(3), get_dist_sensor(4) );

		usleep(500000);
	}



	if(flag == IF_PARK_V)
	{
		int pre;
		int cur;
		int backTurnTime = 1250;
		int frontCheck = 0;
		int frontOut = 0;



		printf("in parking V\n");
		//scanf("%d", &backTurnTime );
		backTurnTime*=1000;
		//init
		distance_set(999999);
		usleep(1000);
		//speed_set( 1000 );
		//usleep(1000);

	
		turn_straight();
		usleep(1000);
		forward_dis();
		n = getCurrentTimeMillis();
		while( getCurrentTimeMillis() - n < 400000 );

		//back straight

		stop();
		sleep(1);

		speed_set(1500);
		usleep(1000);
		turn_set(angle_min);
		usleep(1000);
		backward_dis();
		/*
		   do{
		   if( get_dist_sensor(2) > 90 ){
		   frontCheck = 1;
		   }
		   if( frontCheck && get_dist_sensor(2) < 90 && get_dist_sensor(3) > 200  ){
		   frontOut = 1;
		   }
		   }while( !frontOut );
		 */
		n = getCurrentTimeMillis();
		while( getCurrentTimeMillis() - n < 1320000 );

		turn_straight();
		usleep(1000);
		while( get_dist_sensor(4) < 250 ){
			if( get_dist_sensor(3) > 600 ){
				//turn_set(1800);
			}
		}
		stop();
		turn_straight();
		sleep(1);
		buzzer_on();
		sleep(1);

		turn_straight();
		usleep(1000);
		forward_dis();
		while( get_dist_sensor(4) > 100 );


		turn_set(angle_min);
		usleep(1000);
		n = getCurrentTimeMillis();
		while( getCurrentTimeMillis() - n < 1220000 );

		turn_straight();
		usleep(1000);
		speed_set(START_SPEED);

	}
	else
	{
		   int a = 1150;
		   int b = 1300;
		   int c = 1000;
		   int d = 1300;

		   //scanf( "%d %d %d %d", &a, &b, &c, &d );
		   a *= 1000;
		   b *= 1000;
		   c *= 1000;
		   d *= 1000;

		   distance_set(99999);
		   usleep(1000);
		   speed_set(1500);
		   usleep(1000);


		turn_straight();
		usleep(1000);
		forward_dis();
		n = getCurrentTimeMillis();
		while( getCurrentTimeMillis() - n < 300000 );



		stop();
		sleep(1);

		   turn_set(angle_min);
		   usleep(1000);
		   backward_dis();
		   n = getCurrentTimeMillis();
		   while( getCurrentTimeMillis() - n <1200000 ){
		   }

		turn_straight();
		usleep(1000);
		n = getCurrentTimeMillis();
		while( getCurrentTimeMillis() - n < 400000 );


		   turn_set(angle_max);
		   usleep(1000);
		   n = getCurrentTimeMillis();
		   while( getCurrentTimeMillis() - n < b && get_dist_sensor(4) < 500 ){
		   }

		   turn_straight();
		   usleep(1000);
		   while( get_dist_sensor(4) < 300){
		   }

		   stop();
		   sleep(1);
		//buzzer_on();
		sleep(1);

		turn_set(angle_max);
		usleep(1000);
		forward_dis();
		n = getCurrentTimeMillis();
		while( getCurrentTimeMillis() - n < c );

		turn_set(angle_min);
		usleep(1000);
		n = getCurrentTimeMillis();
		while( getCurrentTimeMillis()-n < d ){
		}


		turn_straight();
		speed_set(START_SPEED);
		//printf("back : %d, right : %d\n", get_dist_sensor(4), get_dist_sensor(3));*/
	}
	//stop();
	d_data[g_index].mid_flag = MID_STRAIGHT;
	//d_data[g_index].speed = 1000;
	//speed_set(1500);
	usleep(10000);
	//distance_reset();
	//usleep(10000);
	g_drive_flag = DF_DRIVE;
}


#endif
