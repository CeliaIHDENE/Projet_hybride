#include "hyb_reduc.h"
#include <mpi.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

 
void shared_reduc_init(shared_reduc_t *sh_red, int nthreads, int nvals)
{
    /* A COMPLETER */
 
    
    //on va initialiser avec les val en parametre
  sh_red->nvals = nvals;
  sh_red->nthreads = nthreads;
  sh_red->maitre= 0;
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
/*
 * Reduction  hybride MPI/pthread
 * in  : tableau des valeurs a reduire (de dimension sh_red->nvals)
 * out : tableau des valeurs reduites  (de dimension sh_red->nvals)
 */
void hyb_reduc_sum(double *in, double *out, shared_reduc_t *sh_red)
{
int maitre_elu =0;  // ai je un maitre 
 

  pthread_mutex_lock(sh_red->mutex);
 
  {

    for (int i = 0; i < sh_red->nvals; i++)
      {        
        sh_red->red_val[i] += in[i];
      }
  if (sh_red->maitre ==0 ) {
        maitre_elu =1;
         sh_red->maitre =1;
  }
     }
  pthread_mutex_unlock(sh_red->mutex);
  pthread_barrier_wait(sh_red->barriere);

 // Choix du thread pricipal

  if (maitre_elu )
    {

      int rank,size;
      MPI_Comm_rank(MPI_COMM_WORLD, &rank);
      MPI_Comm_size(MPI_COMM_WORLD, &size);

      double *tab = malloc(sizeof(double) * size);


      for (int i = 0; i < sh_red->nvals; i++)
        {
          //on met tout dans le tableau 
          MPI_Gather(&(sh_red->red_val[i]), 1, MPI_DOUBLE, tab, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
       
       //on teste si on est le 0
          if (rank == 0)
            {
              for (int j = 0; j < size; j++)
                {
                  out[i] =  out[i] +tab[j];
                }}
                       sh_red->red_val[i] = out[i];
          }
          
          //on en a plus besoin donc on libere 
      free(tab);      
      //on partage
      MPI_Bcast( sh_red->red_val, sh_red->nvals, MPI_DOUBLE, 0, MPI_COMM_WORLD);    
      MPI_Bcast(out, sh_red->nvals, MPI_DOUBLE, 0, MPI_COMM_WORLD);  

 
    }
  else
    {

	//On attends 

      sem_wait(sh_red->semaphore); 
           
      for (int i = 0; i < sh_red->nvals; i++)
        {

          out[i] = sh_red->red_val[i]; 
        }

     
    }

      sem_post(sh_red->semaphore);
   
}

