#include "cam_values.h"

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


