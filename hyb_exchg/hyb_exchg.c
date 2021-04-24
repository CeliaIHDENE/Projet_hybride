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

 int tag =10 ;
 int rank ;
 MPI_Status status ;

sem_wait(&(sh_ex->semaphore));

 
   
    //on les initialise a 0  comme ca pr les processus des bords on a pas a les remetre a 0
    *val_to_rcv_left = 0.0;
    *val_to_rcv_right = 0.0;
    
    //on recupere le rang
    rank = mpi_decomp->mpi_rank ;
    
    // on teste notre rang pour savoir si on est un processus intermediaire le premier ou le dernier  

    if (  mpi_decomp->mpi_nproc == 1 )
        {
           printf("Nous avons juste 1 processus  : pas d'envois ni de reception   ");
          
          *val_to_rcv_left = sh_ex->gauche;
          *val_to_rcv_right = sh_ex->droit;
        }
   
  else
  
        if (mpi_decomp->mpi_rank == 0)  
        {
          
         // printf("je suis le processus 1 \n ");
          MPI_Send(&(sh_arr[mpi_decomp->mpi_nloc - 1]), 1, MPI_DOUBLE, rank+1, tag, MPI_COMM_WORLD);
          MPI_Recv(&(sh_ex->droit), 1, MPI_DOUBLE, rank+1, tag, MPI_COMM_WORLD, &status );
          
 
          *val_to_rcv_right = sh_ex->droit;
      
        }
      else 
      
      if (mpi_decomp->mpi_rank == mpi_decomp->mpi_nproc -1  ) 
       
        {
          // printf("je ss le  dernier processus  \n ");           
          MPI_Recv(&(sh_ex->gauche), 1, MPI_DOUBLE, rank-1, tag, MPI_COMM_WORLD,&status );
          MPI_Send(&(sh_arr[0]), 1, MPI_DOUBLE, rank-1, tag, MPI_COMM_WORLD);
             
             
             
           *val_to_rcv_left = sh_ex->gauche;
 
             
        }
      else  
        {        if (mpi_decomp->mpi_rank != mpi_decomp->mpi_nproc -1 && mpi_decomp->mpi_rank !=0 ) { 
                   
          // printf("je ss celui  du milieu \n  ");     
 
          MPI_Recv(&(sh_ex->gauche), 1, MPI_DOUBLE, rank-1, tag, MPI_COMM_WORLD,&status );
          MPI_Send(&(sh_arr[0]), 1, MPI_DOUBLE, rank-1, tag, MPI_COMM_WORLD);
          MPI_Send(&(sh_arr[mpi_decomp->mpi_nloc - 1]), 1, MPI_DOUBLE, rank+1, tag, MPI_COMM_WORLD);    
          MPI_Recv(&(sh_ex->droit), 1, MPI_DOUBLE, rank+1, tag, MPI_COMM_WORLD, &status ); 
          //ce quon a recu des 2 coté 
           *val_to_rcv_left = sh_ex->gauche;
          *val_to_rcv_right = sh_ex->droit;
          }
        }

           
    
  
      *val_to_rcv_left = sh_ex->gauche;
      *val_to_rcv_right = sh_ex->droit;
      
      sem_post(&(sh_ex->semaphore));
    
 
  }
