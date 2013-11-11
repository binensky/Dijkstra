#include <stdio.h>
#include "miso_values.h"

#define DATAFILE "img_data.dat"

int fread_data(struct drive_data* dat)
{
	FILE* ifp = fopen(DATAFILE,"r");
	int i,tmp;
	if(ifp == NULL )
	{
		printf("input file open is NULL\n");
		return 1;
	}

	i = 0;

	while(!feof(ifp))
	{
		fscanf(ifp,"[%d] %d %d %d %d %lf %d %d %d\n",
			&tmp,
			&dat[i].dist, &dat[i].flag, &dat[i].mid_flag, &dat[i].angle,
			&dat[i].gradient, &dat[i].intercept, &dat[i].height, &dat[i].line_y);
		i++;
	}
	fclose(ifp);
	return 0;

}

int fwrite_data(struct drive_data* dat)
{
	FILE* ofp = fopen(DATAFILE,"w");
	int i,tmp;
	if(ofp == NULL  )
	{
		printf("output file error is NULL\n");
		return 1;
	}

	for( i = 0; i < g_index; i++)
	{
		fprintf(ofp,"[%d] %d %d %d %d %lf %d %d %d\n", 
			i,
			dat[i].dist, dat[i].flag, dat[i].mid_flag, dat[i].angle,
			dat[i].gradient, dat[i].intercept, dat[i].height, dat[i].line_y);
	}

	fclose(ofp);
	return 0;
}
