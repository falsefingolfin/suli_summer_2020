/*test to call superfish*/

# include <stdio.h>
# include <stdlib.h>
# include <math.h>

# include "global.h"


int sfcalculation(int myid)
{
	
	int sfflag;
	
	double radian = 3.1415926/180;
	
	double flattop = 0.2298;
	double topradiusA = cavity_geometry[0];
	double topradiusB = cavity_geometry[1];
	double flatside1 = 0.3142;
	double cornerR1 = cavity_geometry[2];
    double slopelength = cavity_geometry[3];
    double cornerR2 = 0.1005;
	double flatside2 = 0.0161;
	double cornerR3 = 0.1001;
	double pipeR = 2.5;
	double slopeangle = cavity_geometry[4];
	
	double pipeL = 10.0;
	
	/*coordinates calculation*/
	
	double a1,a2,b1,b2,c1,c2,d1,d2,e1,e2,f1,f2,g1,g2,h1,h2,i1,i2,j1,j2,k1,k2;
	
	double ezerot;
	
	a1 = 0.0;
	a2 = topradiusB+flatside1+cornerR1*cos(slopeangle*radian)+slopelength*sin(slopeangle*radian)+cornerR2*cos(slopeangle*radian)+flatside2+cornerR3+pipeR;
	
	b1 = flattop;
	b2 = a2;
	
	c1 = b1+topradiusA;
	c2 = b2-topradiusB;

	d1 = c1;
	d2 = c2-flatside1;
	
	e1 = d1-cornerR1+cornerR1*sin(radian*slopeangle);
	e2 = d2-cornerR1*cos(radian*slopeangle);
	
	f1 = e1-slopelength*cos(radian*slopeangle);
	f2 = e2-slopelength*sin(radian*slopeangle);
	
	g1 = f1-cornerR2+cornerR2*sin(radian*slopeangle);
	g2 = f2-cornerR2*cos(radian*slopeangle);
	
	
	h1 = g1;
	h2 = g2-flatside2;
	
	
	i1 = h1+cornerR3;
	i2 = h2-cornerR3;
	
	j1 = i1+pipeL;
	j2 = i2;
	
	
	k1 = j1;
	k2 = 0.0;
		
	
	ezerot = voltage*100/2/j1;
	
	char file[]="%d/input.af"; 
	char filename[80];
	sprintf(filename,file,myid);
	
	FILE *f = fopen (filename,"w");
	if (f == NULL)
	{
		printf("Error opening file! sfcalculation on %d \n",myid);
		exit(1);
	}
	
	fprintf(f,"ALSU HHC nosecone\n");
	fprintf(f,"$reg kprob=1, dx=0.2, freq=1500,xdri=0., ydri=2.0,\n");
	fprintf(f,"norm=1,ezerot=%f $\n",ezerot);
	fprintf(f,"\n");
	fprintf(f,"$po x=%f, y=%f$\n",k1,k2);
	fprintf(f,"$po x=%f, y=%f$\n",0.0,0.0);
    fprintf(f,"$po x=%f, y=%f$\n",a1,a2);
	fprintf(f,"$po x=%f, y=%f$\n",b1,b2);
	fprintf(f,"$po nt=2, x0=%f,y0=%f,a=%f,b=%f,x=%f,y=%f$\n",b1,b2-topradiusB,topradiusA,topradiusB,topradiusA,0.0);
	fprintf(f,"$po x=%f, y=%f$\n",d1,d2);
	fprintf(f,"$po nt=2, x0=%f,y0=%f,r=%f,theta=%f$\n",d1-cornerR1,d2,cornerR1,270+slopeangle);
	fprintf(f,"$po x=%f, y=%f$\n",f1,f2);
	fprintf(f,"$po nt=2, x0=%f,y0=%f,r=%f,theta=%f$\n",g1+cornerR2,g2,cornerR2,180.0);
	fprintf(f,"$po x=%f, y=%f$\n",h1,h2);
	fprintf(f,"$po nt=2, x0=%f,y0=%f,r=%f,theta=%f$\n",h1+cornerR3,h2,cornerR3,270.0);
	fprintf(f,"$po x=%f, y=%f$\n",j1,j2);
	fprintf(f,"$po x=%f, y=%f$\n",k1,k2);
	fclose(f);
	
	char commandline[200];
	char command1[] = "cd %d && ./AUTOFISH.EXE input.af";
	sprintf(commandline,command1,myid);
	sfflag=system(commandline);
	
	return(sfflag);
	
  }

