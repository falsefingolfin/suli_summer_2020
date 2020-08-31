# include <stdio.h>
# include <stdlib.h>
# include <math.h>

# include "global.h"
# include "rand.h"

void inherit ( population *pop)
{
  FILE* file = fopen ("last_pop.out", "r");
  double a[288][23];
  
  int i,j,k;

for(i=0;i<288;i++)
    {  
	for(j=0;j<23;j++)
	{
      fscanf (file, "%lf", &a[i][j]);	  
	}
	}
for(i=0;i<288;i++)
    {
	for(j=6;j<20;j++)
	{
		k=j-6;
        pop->ind[i].xreal[k]=a[i][j];
	}
    }
  fclose (file); 
}