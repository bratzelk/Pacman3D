/*
 * File:    readfile.c
 * Author:  Kim Bratzel
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "smalloc.h"
#include "readfile.h"
#include "includes.h"

#define STR_BUFFER 1000

//this function reads the input file and stores
int
read_file(char * filename, char line[][15])
{
    char s[STR_BUFFER];
    FILE *file;
    int i=0;

    file = fopen(filename, "r");

    if(file == NULL)
    {
    	return FALSE;
    }


    
    while (fgets(s, STR_BUFFER, file) != NULL)
    {
    	//printf("%d\n",i);
   		sscanf(s, "%c %c %c %c %c %c %c %c %c %c %c %c %c %c %c \n", &line[i][0],&line[i][1],&line[i][2],&line[i][3],&line[i][4],&line[i][5],&line[i][6],&line[i][7],&line[i][8],&line[i][9],&line[i][10],&line[i][11],&line[i][12],&line[i][13],&line[i][14]);
   		i++;
    }

    /*
    for (int j=0;j<15;j++)
    {
    	for (int h=0;h<15;h++)
    	{
    		printf("%c ",line[j][h]);
    	}
    	printf("\n");
    }
    */

    //close the file

    return TRUE;
}

