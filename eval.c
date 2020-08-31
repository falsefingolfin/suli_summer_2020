/* Routine for evaluating population members  */

# include <stdio.h>
# include <stdlib.h>
# include <math.h>

# include "global.h"
# include "rand.h"

/* Routine to evaluate objective function values and constraints for a population */
void evaluate_pop (population *pop, int myid)
{
    int i;
    for (i=0; i<chunksize; i++)
    {
        evaluate_ind (&(pop->ind[i]),myid);
    }
    return;
}

/* Routine to evaluate objective function values and constraints for an individual */
void evaluate_ind (individual *ind, int myid)
{
    int j;
	
	char commandline[200];
	char createfoler[]="mkdir -p %d";
	char copysf[]="cp AUTOFISH.EXE %d";
	
	sprintf(commandline,createfoler,myid);
    system(commandline);
	
	
    sprintf(commandline,copysf,myid);
    system(commandline);
	
	
	
    test_problem (ind->xreal,ind->obj, ind->constr, myid);
    if (ncon==0)
    {
        ind->constr_violation = 0.0;
    }
    else
    {
        ind->constr_violation = 0.0;
        for (j=0; j<ncon; j++)
        {
            if (ind->constr[j]<0.0)
            {
                ind->constr_violation += ind->constr[j];
            }
        }
    }
	
 	char rmfolder[]="rm -r %d";	
	sprintf(commandline,rmfolder,myid);
    system(commandline);
	
    return;
}
