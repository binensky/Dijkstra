
#include "cam_values.h"
#include "miso_values.h"

// cutline 5개씩 10~100까지 30. 
// angle값 3개씩 90까지 18.

// 90 ~ 180 to 2200 ~ 1533  
int left_angle[21][30] ={
//093,0096,0099,0102,0105,0108,0111,0114,0117,0120,0123,0126,0129,0132,####,0138,0141,0144,0147,0150,0153,0156,0159,0162,0165,0168,0171,0174,0177,0180
{1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,2200,2200,2200,2200,2200,2200,2200,2200}, // cutline 1 ~ 5
{1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,2200,2200,2200,2200,2200,2200,2200,2200}, // cutline 6 ~ 10
{1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,2200,2200,2200,2200,2200,2200,2200,2200}, // cutline 11 ~ 15
{1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,2200,2200,2200,2200,2200,2200,2200,2200}, // cutline 16 ~ 20
{1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1000,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,2200,2200,2200,2200,2200,2200,2200,2200}, // cutline 21 ~ 25
{1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,2200,2200,2200,2200,2200,2200,2200,2200}, // cutline 26 ~ 30
{1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,2200,2200,2200,2200,2200,2200,2200,2200}, // cutline 31 ~ 35
{1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,2200,2200,2200,2200,2200,2200,2200,2200}, // cutline 36 ~ 40
{1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,2200,2200,2200,2200,2200,2200,2200,2200}, // cutline 41 ~ 45
{1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,2200,2200,2200,2200,2200,2200,2200,2200}, // cutline 46 ~ 50
{1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,2200,2200,2200,2200,2200,2200,2200,2200}, // cutline 51 ~ 55
{1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,2200,2200,2200,2200,2200,2200,2200,2200}, // cutline 56 ~ 60
{1900,1000,1000,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1533,1533}, // cutline 61 ~ 65
{1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1533,1533,1533}, // cutline 66 ~ 70
{1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1533,1533,1533,1533}, // cutline 71 ~ 75
{1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1533,1533,1533,1533,1533}, // cutline 76 ~ 80
{1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1533,1533,1533,1533,1533,1533}, // cutline 81 ~ 85
{1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1533,1533,1533,1533,1533,1533,1533,1533,1533,1533,1533,1533,1533,1533,1533,1533}, // cutline 86 ~ 90
{1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1533,1533,1533,1533,1533,1533,1533,1533,1533,1533,1533,1533,1533,1533,1533,1533}, // cutline 91 ~ 95 #
{1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1533,1533,1533,1533,1533,1533,1533,1533,1533,1533,1533,1533,1533,1533,1533,1533}, // cutline 96 ~ 100 #
{1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1900,1533,1533,1533,1533,1533,1533,1533,1533,1533,1533,1533,1153,1533,1533,1533,1533}, // cutline 101 ~ ?? #
};

//0 ~ 90 to  800 ~ 1533
// blocking & :s/800/0800/g
// blocking & :s/900/0900/g

int right_angle[21][30] = {
//003,0006,0009,0012,0015,0018,0021,0024,0027,0030,0033,0036,0039,0042,####,0048,0051,0054,0057,0060,0063,0066,0069,0072,0075,0078,0081,0084,0087,0090
{ 800, 800, 900, 900,1000,1000,1000,1100,1100,1533,1533,1533,1533,1533,1533,1533,1100,1100,1100,1100,1100,1100,1100,1100,1100,1100,1100,1100,1100,1100}, // cutline 1 ~ 5
{ 800, 800, 900, 900,1000,1000,1000,1100,1100,1533,1533,1533,1533,1533,1533,1533,1100,1100,1100,1100,1100,1100,1100,1100,1100,1100,1100,1100,1100,1100}, // cutline 6 ~ 10
{ 800, 800, 900, 900,1000,1000,1000,1100,1100,1533,1533,1533,1533,1533,1533,1533,1100,1100,1100,1100,1100,1100,1100,1100,1100,1100,1100,1100,1100,1100}, // cutline 11 ~ 15
{ 800, 800, 900, 900,1000,1000,1000,1100,1100,1533,1533,1533,1533,1533,1533,1533,1100,1100,1100,1100,1100,1100,1100,1100,1100,1100,1100,1100,1100,1100}, // cutline 16 ~ 20
{ 800, 800, 900, 900,1000,1000,1000,1100,1100,1533,1533,1533,1533,1533,1533,1533,1100,1100,1100,1100,1100,1100,1100,1100,1100,1100,1100,1100,1100,1100}, // cutline 21 ~ 25
{ 800, 800, 900, 900,1000,1000,1000,1100,1100,1533,1533,1533,1533,1533,1533,1533,1100,1100,1100,1100,1100,1100,1100,1100,1100,1100,1100,1100,1100,1100}, // cutline 26 ~ 30
{ 800, 800, 900, 900,1000,1000,1000,1100,1100,1533,1533,1533,1533,1533,1533,1533,1100,1100,1100,1100,1100,1100,1100,1100,1100,1100,1100,1100,1100,1100}, // cutline 31 ~ 35
{ 800, 800, 900, 900,1000,1000,1000,1100,1100,1533,1533,1533,1533,1533,1533,1533,1100,1100,1100,1100,1100,1100,1100,1100,1100,1100,1100,1100,1100,1100}, // cutline 36 ~ 40
{ 800, 800, 900, 900,1000,1000,1000,1100,1100,1533,1533,1533,1533,1533,1533,1533,1100,1100,1100,1100,1100,1100,1100,1100,1100,1100,1100,1100,1100,1100}, // cutline 41 ~ 45
{ 800, 800, 900, 900,1000,1000,1000,1100,1100,1533,1533,1533,1533,1533,1533,1533,1100,1100,1100,1100,1100,1100,1100,1100,1100,1100,1100,1100,1100,1100}, // cutline 46 ~ 50 ####  800 시작점
{ 800, 800, 900, 900,1000,1000,1000,1100,1100,1533,1533,1533,1533,1533,1533,1533,1100,1100,1100,1100,1100,1100,1100,1100,1100,1100,1100,1100,1100,1100}, // cutline 51 ~ 55
{1533,1533, 900, 900,1000,1000,1000,1100,1100,1533,1533,1533,1533,1533,1533,1533,1100,1100,1100,1100,1100,1100,1100,1100,1100,1100,1100,1100,1100,1100}, // cutline 56 ~ 60
{1533,1533, 900, 900,1000,1000,1000,1100,1100,1533,1533,1533,1533,1533,1533,1533,1100,1100,1100,1100,1100,1100,1100,1100,1100,1100,1100,1100,1100,1100}, // cutline 61 ~ 65
{1533,1533, 900, 900,1000,1000,1000,1100,1100,1533,1533,1533,1533,1533,1533,1533,1100,1100,1100,1100,1100,1100,1100,1100,1100,1100,1100,1100,1100,1100}, // cutline 66 ~ 70
{1533,1533,1533, 900,1000,1000,1000,1100,1100,1533,1533,1533,1533,1533,1533,1533,1100,1100,1100,1100,1100,1100,1100,1100,1100,1100,1100,1100,1100,1100}, // cutline 71 ~ 75
{1533,1533,1533,1533,1000,1000,1000,1100,1100,1533,1533,1533,1533,1533,1533,1533,1100,1100,1100,1100,1100,1100,1100,1100,1100,1100,1100,1100,1100,1100}, // cutline 76 ~ 80
{1533,1533,1533,1533,1533,1000,1000,1100,1100,1533,1533,1533,1533,1533,1533,1533,1100,1100,1100,1100,1100,1100,1100,1100,1100,1100,1100,1100,1100,1100}, // cutline 81 ~ 85
{1533,1533,1533,1533,1533,1533,1000,1100,1100,1533,1533,1533,1533,1533,1533,1533,1100,1100,1100,1100,1100,1100,1100,1100,1100,1100,1100,1100,1100,1100}, // cutline 86 ~ 90
{1533,1533,1533,1533,1533,1533,1533,1533,1533,1533,1533,1533,1533,1533,1533,1533,1100,1100,1100,1100,1100,1100,1100,1100,1100,1100,1100,1100,1100,1100}, // cutline 91 ~ 95 #
{1533,1533,1533,1533,1533,1533,1533,1533,1533,1533,1533,1533,1533,1533,1533,1533,1100,1100,1100,1100,1100,1100,1100,1100,1100,1100,1100,1100,1100,1100}, // cutline 96 ~ 100 #
{1533,1533,1533,1533,1533,1533,1533,1533,1533,1153,1533,1533,1533,1533,1533,1533,1100,1100,1100,1100,1100,1100,1100,1100,1100,1100,1100,1100,1100,1100}, // cutline 101 ~ ?? #
};

