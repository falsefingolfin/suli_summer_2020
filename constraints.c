/* routines for updating constraint limits when population reaches them */

# include <stdio.h>
# include <stdlib.h>
# include <math.h>
# include <stdbool.h>

# include "global.h"
# include "rand.h"

void update_constraints(population *pop, FILE *log, int gen) {
    int i, j;
    int rank1[popsize];
    double curr_xreal;
    double norm;
    int min_count[nreal];
    int max_count[nreal];
    int nrank1;
    double cutoff, max_lim, min_lim, increment;

    max_lim = 0.9;
    min_lim = 0.1;
    cutoff = 0.5;
    increment = 0.1;
    
    nrank1 = 0;

    for (i=0; i<popsize; i++) {
        rank1[i] = -1;
    }

    for (j=0; j<nreal; j++) {
        min_count[j] = 0;
        max_count[j] = 0;
    }

    /* find all rank 1 individuals and add their population array location to the list */
    for (i=0; i<popsize; i++) {
        if ((pop->ind[i].constr_violation == 0.0) && (pop->ind[i].rank == 1)) {
            rank1[nrank1] = i;
            nrank1++;
        }
    }

    /* quick exit condition */
    if (nrank1 == 0) {
        return;
    }

    /* for each rank 1 individual, count if its close to max/min constraint */
    for (i=0; i<nrank1; i++) {
        for (j=0; j<nreal; j++) {
            int loc = rank1[i];
            curr_xreal = pop->ind[loc].xreal[j];
            norm = (curr_xreal - min_realvar[j]) / (max_realvar[j] - min_realvar[j]);

            if (norm <= min_lim) {
                (min_count[j])++;
            }

            else if (norm >= max_lim) {
                (max_count[j])++;
            }
        }
    }

    /*
    for (j=0; j<nreal; j++) {
        fprintf(stderr, "min_count[%d] = %d\n", j, min_count[j]);
        fprintf(stderr, "max_count[%d] = %d\n", j, max_count[j]);
    }
    */

    /* adjust variable constraints if too many individuals are too close to the limit */
    for (j=0; j<nreal; j++) {
        if (min_count[j] >= popsize*cutoff) {
            double initial = min_realvar[j];
            min_realvar[j] = min_realvar[j] * (1-increment);

            /* report change to log file */
            fprintf(log, "Gen = %d:    min_realvar[%d] changed from %f to %f\n",
                gen, j, initial, min_realvar[j]);
        }

        if (max_count[j] >= popsize*cutoff) {
            double initial = max_realvar[j];
            max_realvar[j] = max_realvar[j] * (1+increment);
            
            /* report change to log file */
            fprintf(log, "Gen = %d:    max_realvar[%d] changed from %f to %f\n",
                gen, j, initial, max_realvar[j]);
        }
    }

    fflush(log);
    return;
}