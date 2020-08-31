/* Developed as an intern with ATAP group at Lawrence Berkeley National Labs */

/* NSGA-II routine (implementation of the 'main' function) */

# include <stdio.h>
# include <stdlib.h>
# include <stdbool.h>
# include <math.h>
# include <time.h>
# include <unistd.h>

# include "global.h"
# include "rand.h"

# include "mpi.h"


int popsize;
int chunksize;
int numtasks;
double pcross_real;
double pmut_real;
double eta_c;
double eta_m;
int ngen;
int nrealmut;
int nrealcross;
double *min_realvar;
double *max_realvar;
int obj1;
int obj2;
int obj3;
int angle1;
int angle2;

double sf1[6];
double cavity_geometry[nreal];
double nose_left_limit;
double outerradius;

int main (int argc, char **argv ) {
    int i,k,l,m;
	int myid;
	int tag_data,tag_offset;
    double start, end;
    int sent, pending;
    bool sending, receiving;
    int free_proc;
    int offset_recv;

	
    population *parent_pop;
    population *child_pop;
    population *mixed_pop;
	
	population1 *data_pop;
	
	popsize = 12; /* population size */
	ngen = 10; /*number of generation */
    m = 0;
	
	MPI_Status status;
	
	MPI_Datatype individualtype, oldtypes[2];   /* required variables*/
    int          blockcounts[2];

    /* MPI_Aint type used to be consistent with syntax of MPI_Type_extent routine */
    MPI_Aint    offsets[2], extent;

    MPI_Init(&argc,&argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &myid);
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);

    MPI_Barrier(MPI_COMM_WORLD);
    start = MPI_Wtime();

	
	tag_data = 1;
    tag_offset = 2;
    sent = 0;
    pending = 0;
    sending = true;
    receiving = true;
	
	/* setup description of the 1 MPI_INT for rank*/
    offsets[0] = 0;
    oldtypes[0] = MPI_INT;
    blockcounts[0] = 1;

    /* setup description of the 1+nreal+ncon+nobj+1 MPI_DOUBLE 
     need to first figure offset by getting size of MPI_INT */
    MPI_Type_get_extent(MPI_INT, &offsets[0], &extent);
	
    offsets[1] = 1 * extent;
    oldtypes[1] = MPI_DOUBLE;
    blockcounts[1] = 1+nreal+ncon+nobj+1;

    /* define structured type and commit it */
    MPI_Type_create_struct(2, blockcounts, offsets, oldtypes, &individualtype);
    MPI_Type_commit(&individualtype);	
	
	
	chunksize=popsize/numtasks;
	
    
   
    if(myid == master) {
	   
    	FILE *fpt1;
        FILE *fpt2;
        FILE *fpt3;
        FILE *fpt4;
        FILE *fpt5;
        FILE *fpt6;
        
        seed = 0.25;
    	
        fpt1 = fopen("initial_pop.out","w");
        fpt2 = fopen("final_pop.out","w");
        fpt3 = fopen("best_pop.out","w");
        fpt4 = fopen("all_pop.out","w");
        fpt5 = fopen("params.out","w");
        fpt6 = fopen("constraints_log.txt", "w");
    	
        fprintf(fpt1,"# This file contains the data of initial population\n");
        fprintf(fpt2,"# This file contains the data of final population\n");
        fprintf(fpt3,"# This file contains the data of final feasible population (if found)\n");
        fprintf(fpt4,"# This file contains the data of all generations\n");
        fprintf(fpt5,"# This file contains information about inputs as read by the program\n");
    	

    	min_realvar = (double *)malloc(nreal*sizeof(double));
        max_realvar = (double *)malloc(nreal*sizeof(double));
    	min_realvar[0]=4.3;
    	max_realvar[0]=4.5;
    	min_realvar[1]=3.85;
    	max_realvar[1]=3.95;
    	min_realvar[2]=1.05;
    	max_realvar[2]=1.15;
    	min_realvar[3]=0.45;
    	max_realvar[3]=0.55;
    	min_realvar[4]=37.5;
    	max_realvar[4]=38.5;

    	
    	pcross_real=0.8; /* probability of crossover of real variable (0.6-1) */
    	pmut_real=0.5; /* probability of mutation of real variables 1/nreal between 0-1 */
    	eta_c=14; /* distribution index for crossover (5-20) */
    	eta_m=20; /* distribution index for mutation (5-50) */
    	
        printf("\n Input data successfully entered, now performing initialization \n");
        fprintf(fpt5,"\n Population size = %d",popsize);
        fprintf(fpt5,"\n Number of generations = %d",ngen);
        fprintf(fpt5,"\n Number of objective functions = %d",nobj);
        fprintf(fpt5,"\n Number of constraints = %d",ncon);
        fprintf(fpt5,"\n Number of real variables = %d",nreal);
 
        for (i=0; i<nreal; i++)
        {
            fprintf(fpt5,"\n Lower limit of real variable %d = %e",i+1,min_realvar[i]);
            fprintf(fpt5,"\n Upper limit of real variable %d = %e",i+1,max_realvar[i]);
        }
        fprintf(fpt5,"\n Probability of crossover of real variable = %e",pcross_real);
        fprintf(fpt5,"\n Probability of mutation of real variable = %e",pmut_real);
        fprintf(fpt5,"\n Distribution index for crossover = %e",eta_c);
        fprintf(fpt5,"\n Distribution index for mutation = %e",eta_m);
		
        fprintf(fpt5,"\n Seed for random number generator = %e",seed);
    	
        nrealmut = 0;
        nrealcross = 0;

    	
        parent_pop = (population *)malloc(sizeof(population));
        child_pop = (population *)malloc(sizeof(population));
        mixed_pop = (population *)malloc(sizeof(population));

    	data_pop = (population1 *)malloc(sizeof(population1));
    	
        allocate_memory_pop (parent_pop, popsize);
        allocate_memory_pop (child_pop, popsize);
        allocate_memory_pop (mixed_pop, 2*popsize);

    	allocate_memory_pop1 (data_pop, popsize);
    	
        randomize();
        initialize_pop (parent_pop);
    	
    	
        /* copy whole population into something we can send */
        for (i=0; i<popsize; i++){
            data_pop->ind1[i].rank = parent_pop->ind[i].rank;
            data_pop->ind1[i].constr_violation = parent_pop->ind[i].constr_violation;
            for(k=0;k<nreal;k++)
                data_pop->ind1[i].xreal[k] = parent_pop->ind[i].xreal[k];
            for(k=0;k<nobj;k++)
                data_pop->ind1[i].obj[k] = parent_pop->ind[i].obj[k];
            for(k=0;k<ncon;k++)
                data_pop->ind1[i].constr[k] = parent_pop->ind[i].constr[k];
            data_pop->ind1[i].crowd_dist = parent_pop->ind[i].crowd_dist;
        }
    	
        /* initial send to all worker processes */
    	for (i=1; i<numtasks; i++) {

    		MPI_Send(&sent, 1, MPI_INT, i, tag_offset, MPI_COMM_WORLD);
            MPI_Send(&data_pop->ind1[i-1], 1, individualtype, i, tag_data, MPI_COMM_WORLD);
            sent++;
            pending++;
        }

    	
    	while (sending) {
            MPI_Recv(&offset_recv, 1, MPI_INT, MPI_ANY_SOURCE, tag_offset, MPI_COMM_WORLD, &status);
            free_proc = status.MPI_SOURCE;
            MPI_Recv(&data_pop->ind1[offset_recv], 1, individualtype, free_proc, tag_data, MPI_COMM_WORLD, &status);

            if (sent < popsize) { /* waiting for results from any process */
                MPI_Send(&sent, 1, MPI_INT, free_proc, tag_offset, MPI_COMM_WORLD);
                MPI_Send(&data_pop->ind1[sent], 1, individualtype, free_proc, tag_data, MPI_COMM_WORLD);
                sent++;
            }
            else { /* all items sent, waiting for final receives */
                pending--;
            }

            if (pending == 0) {
                sending = false;
            }
        }

        for (i = 1; i < numtasks; i++) { /* initial send to all worker processes */
                int quit = -1;
                MPI_Send(&quit, 1, MPI_INT, i, tag_offset, MPI_COMM_WORLD); /* QUIT command for worker processes */
        }

        for (i=0; i<popsize; i++){ /* copy evaluated data back into population */
            parent_pop->ind[i].rank = data_pop->ind1[i].rank ;
            parent_pop->ind[i].constr_violation = data_pop->ind1[i].constr_violation;
            for(l=0;l<nreal;l++)
                parent_pop->ind[i].xreal[l] = data_pop->ind1[i].xreal[l];
            for(l=0;l<nobj;l++)
                parent_pop->ind[i].obj[l] = data_pop->ind1[i].obj[l];
            for(l=0;l<ncon;l++)
                parent_pop->ind[i].constr[l] = data_pop->ind1[i].constr[l];
            parent_pop->ind[i].crowd_dist = data_pop->ind1[i].crowd_dist;
        }

    	
    	assign_rank_and_crowding_distance (parent_pop); 
        report_pop (parent_pop, fpt1); 
        fprintf(fpt4,"# gen = 1\n");
        report_pop(parent_pop,fpt4);

        update_constraints(parent_pop, fpt6, m+1);

    	
        fprintf(stderr,"\n gen = 1 \n\n");
        fflush(stdout);
        fflush(fpt1);
        fflush(fpt2);
        fflush(fpt3);
        fflush(fpt4);
        fflush(fpt5);
    	
    	for (m=1; m<ngen; m++) {
            selection (parent_pop, child_pop);
            mutation_pop (child_pop);

            sent = 0;
            pending = 0;
            sending = true;
            		
    	
            /* copy whole population into something we can send */
            for (i=0; i<popsize; i++){
                data_pop->ind1[i].rank = child_pop->ind[i].rank;
                data_pop->ind1[i].constr_violation = child_pop->ind[i].constr_violation;
                for(k=0;k<nreal;k++)
                    data_pop->ind1[i].xreal[k] = child_pop->ind[i].xreal[k];
                for(k=0;k<nobj;k++)
                    data_pop->ind1[i].obj[k] = child_pop->ind[i].obj[k];
                for(k=0;k<ncon;k++)
                    data_pop->ind1[i].constr[k] = child_pop->ind[i].constr[k];
                data_pop->ind1[i].crowd_dist = child_pop->ind[i].crowd_dist;
            }

            /* initial send to all worker processes */
            for (i=1; i<numtasks; i++) {
                MPI_Send(&sent, 1, MPI_INT, i, tag_offset, MPI_COMM_WORLD);
                MPI_Send(&data_pop->ind1[i-1], 1, individualtype, i, tag_data, MPI_COMM_WORLD);
                sent++;
                pending++;
            }

            while (sending) {
                MPI_Recv(&offset_recv, 1, MPI_INT, MPI_ANY_SOURCE, tag_offset, MPI_COMM_WORLD, &status);
                free_proc = status.MPI_SOURCE;
                MPI_Recv(&data_pop->ind1[offset_recv], 1, individualtype, free_proc, tag_data, MPI_COMM_WORLD, &status);

                if (sent < popsize) { /* waiting for results from any process */
                    MPI_Send(&sent, 1, MPI_INT, free_proc, tag_offset, MPI_COMM_WORLD);
                    MPI_Send(&data_pop->ind1[sent], 1, individualtype, free_proc, tag_data, MPI_COMM_WORLD);
                    sent++;
                }
                else { /* all items sent, waiting for final receives */
                    pending--;
                }

                if (pending == 0) {
                    sending = false;
                }
            }

            for (i = 1; i < numtasks; i++) { /* initial send to all worker processes */
                    int quit = -1;
                    MPI_Send(&quit, 1, MPI_INT, i, tag_offset, MPI_COMM_WORLD); /* QUIT command for worker processes */
            }

            for (i=0; i<popsize; i++){ /* copy evaluated data back into population */
                child_pop->ind[i].rank = data_pop->ind1[i].rank ;
                child_pop->ind[i].constr_violation = data_pop->ind1[i].constr_violation;
                for(l=0;l<nreal;l++)
                    child_pop->ind[i].xreal[l] = data_pop->ind1[i].xreal[l];
                for(l=0;l<nobj;l++)
                    child_pop->ind[i].obj[l] = data_pop->ind1[i].obj[l];
                for(l=0;l<ncon;l++)
                    child_pop->ind[i].constr[l] = data_pop->ind1[i].constr[l];
                child_pop->ind[i].crowd_dist = data_pop->ind1[i].crowd_dist;
            }

    		
            merge (parent_pop, child_pop, mixed_pop);
            fill_nondominated_sort (mixed_pop, parent_pop);
            /* Comment following four lines if information for all
            generations is not desired, it will speed up the execution */
            fprintf(fpt4,"# gen = %d\n", m+1);
            report_pop(parent_pop,fpt4);
            fflush(fpt4);
            fprintf(stderr,"\n gen = %d \n\n", m+1);

            update_constraints(parent_pop, fpt6, m+1);
        }


    	fprintf(stderr,"\n Generations finished, now reporting solutions\n");
        report_pop(parent_pop,fpt2);
        report_feasible(parent_pop,fpt3);
        if (nreal!=0)
        {
            fprintf(fpt5,"\n Number of crossover of real variable = %d",nrealcross);
            fprintf(fpt5,"\n Number of mutation of real variable = %d",nrealmut);
        }
        fflush(stdout);
        fflush(fpt1);
        fflush(fpt2);
        fflush(fpt3);
        fflush(fpt4);
        fflush(fpt5);
        fclose(fpt1);
        fclose(fpt2);
        fclose(fpt3);
        fclose(fpt4);
        fclose(fpt5);
        if (nreal!=0)
        {
            free (min_realvar);
            free (max_realvar);
        }
        deallocate_memory_pop (parent_pop, popsize);
        deallocate_memory_pop (child_pop, popsize);
        deallocate_memory_pop (mixed_pop, 2*popsize);
    /* 	deallocate_memory_pop (chunk_pop,chunksize);
    	
    	deallocate_memory_pop1 (data_pop,chunksize); */	
    	
    	free (parent_pop);
    	free (child_pop);
    	free (mixed_pop);
    /* 	free (chunk_pop);
    	free (data_pop); */
    }
   

    if (myid > master) {

        int offset;
        individual1 recv;
        individual *recv_copy;
        recv_copy = (individual *)malloc(sizeof(individual));
        allocate_memory_ind(recv_copy);
    	
    	/*printf("data_pop check on %d before receiving: %d,%f\n", myid, data_pop->ind1[0].rank,data_pop->ind1[0].xreal1); 

        Receive my portion of array from the master task */
    	
    	while (m<ngen){
            receiving = true;
    		
            while (receiving) {
                MPI_Recv(&offset, 1, MPI_INT, 0, tag_offset, MPI_COMM_WORLD, &status);
                if (offset < 0) { /* exit condition */
                    receiving = false;
                    continue; /* breaks while loop */
                }
                MPI_Recv(&recv, 1, individualtype, 0, tag_data, MPI_COMM_WORLD, &status);
    	
                /* copy data for evaluation */
        		recv_copy->rank = recv.rank;
        		recv_copy->constr_violation = recv.constr_violation;
        		for(l=0;l<nreal;l++)
        		  recv_copy->xreal[l] = recv.xreal[l];
        		for(l=0;l<nobj;l++)
        		  recv_copy->obj[l] = recv.obj[l];
        	    for(l=0;l<ncon;l++)
        		  recv_copy->constr[l] = recv.constr[l];
        		recv_copy->crowd_dist = recv.crowd_dist;

        		evaluate_ind(recv_copy, myid); /* evaluate the individual */
    	
                /* copy data for sending */
        		recv.rank = recv_copy->rank;
        		recv.constr_violation = recv_copy->constr_violation;
        		for(l=0;l<nreal;l++)
        		  recv.xreal[l] = recv_copy->xreal[l];
        		for(l=0;l<nobj;l++)
        		  recv.obj[l] = recv_copy->obj[l];
        	    for(l=0;l<ncon;l++)
        		  recv.constr[l] = recv_copy->constr[l];
        		recv.crowd_dist = recv_copy->crowd_dist;

                /* Send my results back to the master task */
                MPI_Send(&offset, 1, MPI_INT, 0, tag_offset, MPI_COMM_WORLD);
                MPI_Send(&recv, 1, individualtype, 0, tag_data, MPI_COMM_WORLD);
        	}
            m++;
        }

        /*
    	deallocate_memory_pop (chunk_pop,chunksize);
    	deallocate_memory_pop1 (data_pop,chunksize);
    	
    	free (chunk_pop);
    	free (data_pop);
        */
    }


    MPI_Barrier(MPI_COMM_WORLD);
    end = MPI_Wtime(); 
   
 
	MPI_Type_free(&individualtype);
    MPI_Finalize();

/*    if (myid == 0) {
        printf("RUNTIME: %f seconds\n", (end - start));
    }*/

    return(0);
}
