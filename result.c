/* read the superfish output file, generate a record file to keep the key parameters, and pass the key values to the variables in C. */ 

# include <stdio.h>
# include <stdlib.h>
# include <math.h>

# include "global.h"

void sfresults(int index)
{
	float frequency, power, qualityQ, rOverQ, pdmax,emax;
	
	FILE *fp;
	char commandline[200];
	
	char temp[]="%d/temp";
	char tempname[80];
	sprintf(tempname,temp,index);
	
	char command1[] = "cd %d && awk '{a[i++]=$0} END{for(j=i-38;j<=i-18;j++) print a[j]}' INPUT.SFO > record";
	sprintf(commandline,command1,index);
	system(commandline);
	
	
    char command2[]="cd %d && awk 'NR==3 {print $3}' record > temp";
	sprintf(commandline,command2,index);
	system(commandline);
	fp=fopen(tempname,"r");
	if (fp == NULL)
	{
		printf("Error opening file! sfresults on %d \n",index);
		exit(1);
	}
	fscanf(fp, "%f",&frequency);
	fclose(fp);
	
	  char command3[]="cd %d && awk 'NR==14 {print $3}' record > temp";
	sprintf(commandline,command3,index);
	system(commandline);
	fp=fopen(tempname,"r");
	if (fp == NULL)
	{
		printf("Error opening file! sfresults on %d \n",index);
		exit(1);
	}
	fscanf(fp, "%f",&qualityQ);
	fclose(fp);
	
	char command4[]="cd %d && awk 'NR==13 {print $4}' record > temp";
	sprintf(commandline,command4,index);
	system(commandline);
	fp=fopen(tempname,"r");
	if (fp == NULL)
	{
		printf("Error opening file! sfresults on %d \n",index);
		exit(1);
	}
	fscanf(fp, "%f",&power);
	fclose(fp);
	
	if(power<1000)
	{
		power=power*1000; /* the unit in SUPERFISH output can be W or kW */ 
	}
	
	power=power*2; /* SUPERFISH is a quarter of the model, total power has a factor of 4 */
	
	char command5[]="cd %d && awk 'NR==18 {print $10}' record> temp";
	sprintf(commandline,command5,index);
	system(commandline);
	fp=fopen(tempname,"r");
	if (fp == NULL)
	{
		printf("Error opening file! sfresults on %d \n",index);
		exit(1);
	}
	fscanf(fp, "%f",&pdmax);
	fclose(fp);
	
	char command6[]="cd %d && awk 'NR==19 {print $8}' record > temp";
	sprintf(commandline,command6,index);
	system(commandline);
	fp=fopen(tempname,"r");
	if (fp == NULL)
	{
		printf("Error opening file! sfresults on %d \n",index);
		exit(1);
	}
	fscanf(fp, "%f",&emax);
	fclose(fp);
	
	char command7[]="cd %d && awk 'NR==16 {print $3}' record > temp";
	sprintf(commandline,command7,index);
	system(commandline);
	fp=fopen(tempname,"r");
	if (fp == NULL)
	{
		printf("Error opening file! sfresults on %d \n",index);
		exit(1);
	}
	fscanf(fp, "%f",&rOverQ);
	fclose(fp);
	
	rOverQ=rOverQ*2;
	
	sf1[0]=frequency;
	sf1[1]=qualityQ;
	sf1[2]=power;
	sf1[3]=pdmax;
	sf1[4]=emax;
	sf1[5]=rOverQ;
	
	/*
	fprintf(stderr, "\nfrequency = %f MHz\n", frequency);
	fprintf(stderr, "power = %f W\n", power);
	fprintf(stderr, "max power density = %f W/cm^2\n",pdmax);
	fprintf(stderr, "max E = %f MV/m\n",emax);
	fprintf(stderr, "qualityfactor = %f \n",qualityQ);
	fprintf(stderr, "RoverQ = %f \n", rOverQ);
	*/
	
	
	
	

	
    return;
}