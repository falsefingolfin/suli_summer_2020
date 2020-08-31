/* Test problem definitions */

# include <stdio.h>
# include <stdlib.h>
# include <math.h>

# include "global.h"
# include "rand.h"

/* # define sch1 */
/* # define sch2 */
/* # define fon */
/* # define kur */
/* # define pol */
/* # define vnt */
/* # define zdt1 */
/* # define zdt2 */
/* # define zdt3 */
/* # define zdt4 */
/* # define zdt5 */
/* # define zdt6 */
/* # define bnh */
/* # define osy */
/* # define srn */
/* # define tnk */
/* # define ctp1 */
/* # define ctp2 */
/* # define ctp3 */
/* # define ctp4 */
/* # define ctp5 */
/* # define ctp6 */
/* # define ctp7 */
/*# define ctp8*/
# define sf

/* superfish calculation */

#ifdef sf
void test_problem (double *xreal, double *obj, double *constr, int myid)
{
	int flag;
	int i;
	for(i=0;i<nreal;i++)
		cavity_geometry[i]=xreal[i];
	/*
	cavity_geometry[0] outer radius 
	cavity_geometry[1] total length 
	cavity_geometry[2] gap 
	cavity_geometry[3]=xreal[3] inner nose  
	cavity_geometry[4]=xreal[4] nose flat 
	cavity_geometry[5]=xreal[5] outer nose radius 
	cavity_geometry[6]=xreal[6] outer nose angle 
	cavity_geometry[7]=xreal[7] nose flat length 
	cavity_geometry[8]=xreal[8] inner face radius 
	cavity_geometry[9]=xreal[9] middle face radius  
	cavity_geometry[10]=xreal[10] corner radius 1 
	cavity_geometry[11]=xreal[11] corner radius 2 
	cavity_geometry[12]=xreal[12] corner radius 3 
	cavity_geometry[13]=xreal[13] back angle 
	cavity_geometry[14]=xreal[14] inner back radius 
	cavity_geometry[15]=xreal[15] cathode flat 
	*/
	
  
  
  
	
flag=sfcalculation(myid); /* suferfish calculation */
	if(flag==0)
	sfresults(myid);    /* extract key numerical results from superfish output file */
    else
	{sf1[0]=0;
	 sf1[1]=100;
	 sf1[2]=100000;
	 sf1[3]=0;
	 sf1[4]=0;
	 sf1[5]=0;
	}
	
	/*
	sf1[0]=frequency;
	sf1[1]=qualityQ;
	sf1[2]=power;
	sf1[3]=pdmax at v=180kV;
	sf1[4]=emax at v=180kV;
	sf1[5]=rOverQ;
	*/
	
	obj[0]=sf1[2]/10000;  /* power as small as possible */
	obj[1]= 10000/sf1[1];  /* quality factor as large as possible */
	
	constr[0]= 1-sf1[3]/35.0; /* peak power density smaller than 35 w/cm^2 */
	constr[1]=1-fabs(sf1[0]-1500)/50; /*frequency within 1500 +/-30 MHz */
	constr[2]=sf1[5]/60-1; /* r/Q is larger than 60 */

}

#endif


