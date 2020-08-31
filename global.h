/* This file contains the variable and function declarations */

# ifndef _GLOBAL_H_
# define _GLOBAL_H_

# define INF 1.0e14
# define EPS 1.0e-14
# define E  2.71828182845905
# define PI 3.14159265358979

# define ncon 3
# define nreal 5
# define nobj 2

# define voltage 180*1000 

# define master 0

typedef struct
{
    int rank;
    double constr_violation;
    double *xreal;
    double *obj;
    double *constr;
    double crowd_dist;
}
individual;

typedef struct
{
    int rank;
    double constr_violation;
    double xreal[nreal];
    double obj[nobj];
    double constr[ncon];
    double crowd_dist;
}
individual1;

typedef struct
{
    individual *ind;
}
population;

typedef struct
{
	individual1 *ind1;
}
population1;

typedef struct lists
{
    int index;
    struct lists *parent;
    struct lists *child;
}
list;

extern int popsize;
extern double pcross_real;
extern int chunksize;

extern double pmut_real;

extern double eta_c;
extern double eta_m;
extern int ngen;

extern int nrealmut;

extern int nrealcross;

extern double *min_realvar;
extern double *max_realvar;


extern int angle1;
extern int angle2;

extern double cavity_geometry[nreal];
extern double sf1[6];


void allocate_memory_pop (population *pop, int size);
void allocate_memory_ind (individual *ind);
void allocate_memory_pop1 (population1 *pop, int size);
void deallocate_memory_pop (population *pop, int size);
void deallocate_memory_ind (individual *ind);
void deallocate_memory_pop1 (population1 *pop1, int size);

double maximum (double a, double b);
double minimum (double a, double b);

void crossover (individual *parent1, individual *parent2, individual *child1, individual *child2);
void realcross (individual *parent1, individual *parent2, individual *child1, individual *child2);


void assign_crowding_distance_list (population *pop, list *lst, int front_size);
void assign_crowding_distance_indices (population *pop, int c1, int c2);
void assign_crowding_distance (population *pop, int *dist, int **obj_array, int front_size);

void onthefly_display (population *pop, FILE *gp, int ii);

int check_dominance (individual *a, individual *b);

void evaluate_pop (population *pop, int myid);
void evaluate_ind (individual *ind, int myid);

void fill_nondominated_sort (population *mixed_pop, population *new_pop);
void crowding_fill (population *mixed_pop, population *new_pop, int count, int front_size, list *cur);

void initialize_pop (population *pop);
void initialize_ind (individual *ind);

void inherit (population *pop);

void insert (list *node, int x);
list* del (list *node);

void merge(population *pop1, population *pop2, population *pop3);
void copy_ind (individual *ind1, individual *ind2);

void mutation_pop (population *pop);
void mutation_ind (individual *ind);
void bin_mutate_ind (individual *ind);
void real_mutate_ind (individual *ind);

void test_problem (double *xreal, double *obj, double *constr, int myid);

void assign_rank_and_crowding_distance (population *new_pop);

void report_pop (population *pop, FILE *fpt);
void report_feasible (population *pop, FILE *fpt);
void report_ind (individual *ind, FILE *fpt);

void quicksort_front_obj(population *pop, int objcount, int obj_array[], int obj_array_size);
void q_sort_front_obj(population *pop, int objcount, int obj_array[], int left, int right);
void quicksort_dist(population *pop, int *dist, int front_size);
void q_sort_dist(population *pop, int *dist, int left, int right);

void selection (population *old_pop, population *new_pop);
individual* tournament (individual *ind1, individual *ind2);

int sfcalculation( int myid);
void sfresults(int index);

# endif
