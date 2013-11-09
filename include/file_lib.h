#include <stdio.h>
#include "miso_values.h"

#define DATAFILE "img_data.dat"

int fread_data(struct image_data* idt)
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
		fscanf(ifp,"[%d] %d %d %d (%d,%d) (%d,%d) (%d,%d) %d %d %d\n",
			&tmp,
			&idt[i].dist, &idt[i].flag, &idt[i].mid_flag, 
			&idt[i].bot[0].x,&idt[i].bot[0].y,
			&idt[i].bot[1].x,&idt[i].bot[1].y,
			&idt[i].bot[2].x,&idt[i].bot[2].y,
			&idt[i].angle[0],&idt[i].angle[1],&idt[i].angle[2] );

		i++;
	}
	fclose(ifp);
	return 0;

}

int fwrite_data(struct image_data* idt, int cnt)
{
	FILE* ofp = fopen(DATAFILE,"w");
	int i;
	if(ofp == NULL  )
	{
		printf("output file error is NULL\n");
		return 1;
	}

	for( i = 0; i < cnt; i++)
	{
		fprintf(ofp,"[%d] %3d %3d %3d (%3d,%3d) (%3d,%3d) (%3d,%3d) %3d %3d %3d\n", 
			i,
			idt[i].dist, idt[i].flag, idt[i].mid_flag, 
			idt[i].bot[0].x,idt[i].bot[0].y,
			idt[i].bot[1].x,idt[i].bot[1].y,
			idt[i].bot[2].x,idt[i].bot[2].y,
			idt[i].angle[0], idt[i].angle[1], idt[i].angle[2] );
	}

	fclose(ofp);
	return 0;
}
