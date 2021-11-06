#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <math.h>
#include <time.h>

double montecarlo(int samples)
{
	int i, count;
	double x,y;
	double pi;
	
	count = 0;
	for( i = 0; i < samples; i++) {
		x = (double) rand() / RAND_MAX;
		y = (double) rand() / RAND_MAX;
		if (x*x + y*y <= 1)
			count++;
	}
	return count;
}

int main (int argc, char *argv[])
{
  double counts, count_sum, pi_final, begin=0.0, end=0.0;
  int task_id, n_tasks, rc, sample;
  
  // Obtain number of tasks and task ID
  MPI_Init(&argc,&argv);
  MPI_Comm_size(MPI_COMM_WORLD,&n_tasks);
  MPI_Comm_rank(MPI_COMM_WORLD,&task_id);
  //time start
  MPI_Barrier(MPI_COMM_WORLD);
  begin = MPI_Wtime();
  
  // different seed for random number generator for each task
  srandom (task_id);
  sample = atoi(argv[1]);
    // all tasks will execute montecarlo() to get counts
   
     counts = montecarlo(sample/n_tasks);   

    // MPI_reduce
    rc = MPI_Reduce(&counts, &count_sum, 1, MPI_DOUBLE, MPI_SUM,
                    0, MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);
    end = MPI_Wtime();                
    if (task_id == 0) {
      printf("elapsed Time=%fs\n",end-begin);
      pi_final = 4.0 * (double)count_sum/(double)sample;
      printf("%d samples gives: pi %.12f:\n",
             (sample), pi_final);
    }
   
  MPI_Finalize();
  return 0;
}