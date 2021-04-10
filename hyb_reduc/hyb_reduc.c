#include "hyb_reduc.h"
#include <string.h>
#include <mpi.h>
#include <stdlib.h>

void shared_reduc_init(shared_reduc_t *sh_red, int nthreads, int nvals)
{
    /* A COMPLETER */
 
    
    //on va initialiser avec les val en parametre
  sh_red->nvals = nvals;
  sh_red->nthreads = nthreads;

//on alloue puis on initialise 

           /* Phase d'initialisation  */
  sh_red->red_val = malloc(sizeof(double) * nvals);
  sh_red->semaphore = malloc(sizeof(sem_t));  
  sh_red->mutex  = malloc(sizeof(pthread_mutex_t));
  sh_red->barriere = malloc(sizeof(pthread_barrier_t));

  
            /* Phase d'initialisation  */
  memset(sh_red->red_val, 0, sizeof(double) * nvals);			//initialise a 0
  pthread_mutex_init(sh_red->mutex, NULL); 				       //initialise notre mutex 
  pthread_barrier_init(sh_red-> barriere, NULL, nthreads);		      //initialise notre barriere  
  sem_init(sh_red->semaphore, 0, 0);                                       //initialise notre semaphore
 
    
    
}

void shared_reduc_destroy(shared_reduc_t *sh_red)
{
    /* A COMPLETER */
   //On desaloue tout ce quon a alouer plus haut 
   
  free(sh_red->semaphore);
  pthread_barrier_destroy(sh_red->barriere);
  free(sh_red->barriere);

  pthread_mutex_destroy(sh_red->mutex);
  free(sh_red->mutex);

  free(sh_red->red_val);
    
    
}
void hyb_reduc_sum(double *in, double *out, shared_reduc_t *sh_red)
{
    /* A COMPLETER */
}

/*
 * Reduction  hybride MPI/pthread
 * in  : tableau des valeurs a reduire (de dimension sh_red->nvals)
 * out : tableau des valeurs reduites  (de dimension sh_red->nvals)
 */

