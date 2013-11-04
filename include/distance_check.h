//#include "car_lib.h"

void* distance_check(void* data)
{

	/*
	 *
	 * Latest Update : 2012 / 10 / 26
	 * File Name : Parking Module
	 * Programmer : Geonsang Yoo
	 * Content : Vertical Parking / Horizontal Parking
	 *
	 */

	int ON_=1; //주차상태임
	int OFF_ =0; //주차상태아님
	int PARK_H=1; //수평주차중
	int PARK_V=2; //수직주차중

	int park_flag[3]; //3단계 주차플래그 (주차가 총 3단계임)
	int W_PARK = 0; //현재 주차 상태 (수평 or 수직)
	int a = 0; //임시 변수 (현재까지 몇 cm 이동하였는지 거리를 저장한다)
	int remember_distance = 0; //수평주차때 이용되는 변수로 몇cm 이동하였는지 임시로 저장한다.

	int exit = 0; //현재 주차 끝났으면 1, 주차중이면 0

	/*	
	car_connect(); //차랑 연결

	sleep(1);
	//stop();
	speed_set(2000);//스피드 결정
	distance(1,0x1); //한번에 갈 거리 결정
	accel(0xff); //엑셀 (별 의미 없음)
	 */
	distance_reset(); //total_distance 초기화
	int i =0;
	park_flag[0] = ON_; //1단계 주차 단계 맞음 (default)
	park_flag[1] = OFF_; //2단계 주차 단계 아님
	park_flag[2] = OFF_; //3단계 주차 단계 아님
	
   sleep(1);

   //dm_angle(0x05,0xf5); //딱 중간 각도

	
	for( i=0; i<10; i++)
		check_distance(3);

	//wait(2);

	while(1)
	{

		while(check_distance(3) >= 200) //1st sensoring - 장애물을 처음으로 인식했을때부터  주차장 인식할때까지
		{
			if(park_flag[1] != ON_) //2단계 주차단계 플래그가 아님이라고 되어있다면
				park_flag[1] = ON_; //바꿔주기

			accel(0xff); 
			forward_dis(); 
			printf("1st sensoring \n");
		}
		/////////////////////////////////////////////////////////////////

		if(park_flag[1] == ON_) //2단계 주차중이라면
		{
			while(check_distance(3) < 200) //2nd sensoring -> 주차장 인식하고, 주차장의 폭의 길이를 측정한다. 주차장 끝날때까지 측정한다.
			{
				if(park_flag[2] != ON_) //3단계 주차단계 플래그가 아님이라고 되어있다면
				{
					distance_reset(); //주차장에 들어왔으니 길이 재기 위하여 다시 한번 total_distance 리셋!
					park_flag[2] = ON_; //플래그 바꿔주기
				}

				// accel(0xff);
				//forward_dis(); // 센서값 안쓸때는 일단 직진하기

				if( (park_flag[0] == ON_) && (park_flag[1] == ON_) && (park_flag[2] == ON_) ) //주차 3단계라면 계속 있기
				{
					a =  total_distance(); //주차장의 길이를 계속 측정 (주차장을 벗어날때까지)
				}

				printf("2nd sensoring!!!\n");
			}

			//pass the parking area - 주차장을 빠져나감

			if( (park_flag[0] == ON_) && (park_flag[1] == ON_) && (park_flag[2] == ON_) ) //주차 3단계까지 플래그 다 OK이면
			{
				g_drive_flag = DF_VPARK; 
				printf("%d cm long....",a); //주차장은 몇센치입니다.
				stop(); //멈춰
				sleep(1); //3초쉬고
				printf("stop!!!\n");
				distance_reset(); //거리 초기화하고
				//lm_speed(0x05,0xe0); //스피드 올리고


				////////////////이제부터 수평.수직주차 모드가 나뉩니다.////////////////////////////////


				///////////////////주차장이 40cm 이하라면 수직주차

				if( (a > 0) && (a < 40) ) //Vertical Parking Mode (value modified)
				{
					g_drive_flag = DF_VPARK; 
					a = 0; //저장 거리 넣을 변수 초기화
					W_PARK = PARK_V; //수직주차
					printf("Vertical Parking Mode Start!!!!\n");

					if(W_PARK == PARK_V) //수직주차 플래그라면
					{
						wait(1);
						backward_das();
						usleep(180000);
						stop();

						wait(1);

						dm_angle(0x02, 0x80); 
						usleep(2);

						distance(0xd4,0x1);
						accel(0xff);

						backward_dis();
						//////////////////////////////////////////
						uwait(1120000);

						distance(0xff,0xff);
						dm_angle(0x05,0xe5);
						usleep(2);
						backward_dis();
						//////////////////////////////////////////
						uwait(920000);
						a=0;

						stop();
						wait(1); //vertial parking done.....

						/////////////////////////////////

						distance(0xd4,0x1);
						accel(0xff);

						uwait(1);
						forward_dis();
						uwait(900000);

						uwait(1);
						dm_angle(0x00,0x00);

						uwait(1);
						distance(0xd4,0x1);
						forward_dis();
						uwait(1100000);

						dm_angle(0x05,0xe5);
						stop();

					}
					g_drive_flag = DF_DRIVE;
				}
				/////////////////////////////////////////////////////////////////////////여기까지가 수직주차끝

				////////////////////////////주차장이 40cm 이상이면 수평주차////////////////////////////////////////////////////////////
				else if(a >= 40) //Horizontal Parking Mode
				{
					g_drive_flag = DF_PPARK;
					W_PARK = PARK_H;
					printf("Horizontal Parking Mode Start!!!\n");

					if(W_PARK == PARK_H)
					{
						a=0; //역시 거리저장값 초기화

						front_light(ON); //수평주차니깐 특별히 앞등 키고
						break_light(ON); //뒷등 키고

						wait(1);
						///////////////////////////special moving behind
						dm_angle(0x05,0xf5); //그냥 후진하면 거리가 안맞아서 아주잠~깐 전진하고
						backward_dis();
						uwait(200000); //잠깐전진!!!
						stop();
						wait(1);

						//**********************Step 1 start

						dm_angle(0x00,0x00); //좀만 후진하고
						distance(0xff,0xff);                                    
						backward_dis();
						uwait(800000); //(첫번째 후진)
						//**********************Step 1 end


						stop();
						usleep(1);
						dm_angle(0x05,0xf0); //각 중간으로 맞추고
						backward_dis(); //그대로 좀만 후진
						uwait(720000); //(2번째 후진)


						stop();
						uwait(10);                    

						// distance_reset();
						wait(1);
						dm_angle(0xff,0xff);
						stop();


						////////////////////// 
						backward_dis();
						uwait(800000); //(3번째 후진)

						///////////
						stop();
						usleep(1);
						dm_angle(0x05,0xf5);
						wait(1);

						usleep(1);
						forward_dis();
						uwait(200000);

						stop();
						uwait(1);
						front_light(OFF);
						break_light(OFF);
						wait(3);

						backward_dis();
						uwait(200000);
						stop();

						//usleep(1);

						//distance_reset();
						//lm_speed(0x5e,0xff);
						//distance(0x00,0xff);




						usleep(2); 
						dm_angle(0xff,0xff);
						stop();                     

						forward_dis();                      
						uwait(1050000); //(거꾸로 전진)
						stop();                       
						//a = 0;

						//stop();
						// wait(1);

						usleep(1);
						dm_angle(0x00,0x00);
						stop();
						// distance_reset();


						forward_dis();
						uwait(1300000);
						//(거꾸로 전진)
						stop();
						uwait(1);
						dm_angle(0x05,0xf0);

						//end
						stop();
						a=0;
						remember_distance = 0;
						distance_reset();
						//front_light(OFF);
						//break_light(OFF);
						wait(1); //끝났습니다.
					}
					g_drive_flag = DF_DRIVE;
				}

				/////////////////////////////////////수평주차 끝/////////////////////////////////////////////////////////



				else //neither H_P nor V_P
				{
				}
				/////////////////////////////////////////////////////////////////////////////////////


				park_flag[0]=OFF_; //플래그 초기화
				park_flag[1]=2;
				park_flag[2]=3;
				W_PARK = 0;
				printf("program is ended !! \n");
				front_light("OFF"); 
				break_light("OFF"); 
				//sleep(2);
				//exit = 1;	
			}


		}


/*
		//여기 아래 두줄은 평상시 아무 장애물 없을때..... 그냥 직진하는 코드입니다.
		//common state 

		if(exit == 1) //현재 주차 끝났으면 while문을 나온다.
			break;


		//아니면 계속 직진한다.
		accel(0xff);
		forward_dis();
*/
	}
}
