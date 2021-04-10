#include "hyb_exchg.h"
#include <stdio.h>
#include <mpi.h>

/*
 * Initialisation/destruction d'une structure shared_exchg_t
 * nthreads : nombre de threads (du processus MPI) qui vont participer a l'echange
 */
void shared_exchg_init(shared_exchg_t *sh_ex, int nthreads)
{
  sh_ex->droit = 0.0;
  sh_ex->gauche = 0.0;
  sh_ex->nthreads = nthreads;
  sem_init(&(sh_ex->semaphore), 1, 1);

  
 }

void shared_exchg_destroy(shared_exchg_t *sh_ex)
{
  sem_destroy(&(sh_ex->semaphore));
}


/*
 * Echange hybride MPI/pthread
 * Si processus MPI existe "a gauche", lui envoie la valeur sh_arr[0] et recoit de lui *val_to_rcv_left
 * Si processus MPI existe "a droite", lui envoie la valeur sh_arr[mpi_decomp->mpi_nloc-1] et recoit de lui *val_to_rcv_right
 * Si processus voisin n'existe pas, valeur correspondante affectee a 0
 */


void hyb_exchg(
               double *sh_arr,
               shared_exchg_t *sh_ex,
               double *val_to_rcv_left, double *val_to_rcv_right,
               mpi_decomp_t *mpi_decomp)
{

 
}
