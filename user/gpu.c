#include "kernel/types.h"
#include "kernel/riscv.h"
#include "kernel/memlayout.h"
#include "user/user.h"

/* Example:  
 *
 * Suppose we have N GPUs in the system, and we have 7 processes that may need to use them.
 * We have an array of data structures gpus[5] describing one gpu in every element
 * We need to define two operations
 *
 *  - get_gpu() allocates a GPU to the process and returns its integer id
 *  - release_gpu(int id) “returns” specified GPU back to the system
 *
 * If a process cannot acquire a GPU it goes to sleep in get_gpu() and 
 * is automatically woken up when another process “returns” gpu via release_gpu()
 * 
 * Solution using semaphores: 
 *
 * Define two semaphores Sg, and Sm. 
 * Initialise Sg=N, Sm=1
 *
 * Pseudocode of get_gpu() and release_gpu(id) is a s follows:
 *
 * int get_gpu():
 *    wait(Sg);
 *    wait(Sm);
 *    critical section: find unused GPU and mark it as used
 *    signal(Sm);
 *    return index of the found gpu
 *
 * release_gpu(int id):
 *    wait(Sm);
 *    critical section: mark specified GPU as used
 *    signal(Sm);
 *    signal(Sg);
 */
 
enum semaphores {
  Sg = 0,
  Sm = 1
};

// Number of GPUS available 
#define NGPUS (5)

struct gpu {
  char in_use;
};

// situate gpus[] array in the shared virtual memory page
volatile struct gpu *gpus = (volatile struct gpu*) 0x3FFFFFD000;

int get_gpu() {
   int i;
   sem_wait(Sg,1);
   sem_wait(Sm,1);
   // critical section
   for (i=0; i<NGPUS; i++) 
     if (!(gpus[i].in_use))
     {
        gpus[i].in_use = 1;
        printf("allocated GPU #%d\n",i);
	break;
     }	
   sem_post(Sm,1);
   return i;
}

void release_gpu(int id) {
   sem_wait(Sm,1);
   // critical section
   gpus[id].in_use = 0;
   printf("released GPU #%d\n",id);
   sem_post(Sm,1);
   sem_post(Sg,1);
}


void process()
{
   int id;
   
   id = get_gpu();
   //do some work
   for (int i = 100000000; i > 0; i--);
   release_gpu(id);
   
   exit(0);
}
	
int
main(int argc, char *argv[])
{
  // initialise gpus[] array
  for (int i=0; i<NGPUS; i++) gpus[i].in_use=0;

  // initialise semaphores 
  sem_open(Sg,NGPUS);
  sem_open(Sm,1);

  // create processes
  if (fork() == 0) process(); // process 1
  if (fork() == 0) process(); // process 2
  if (fork() == 0) process(); // process 3
  if (fork() == 0) process(); // process 4
  if (fork() == 0) process(); // process 5
  if (fork() == 0) process(); // process 6
  if (fork() == 0) process(); // process 7

  // wait for the 7 child processes to terminate
  wait(0); 
  wait(0);
  wait(0);
  wait(0);
  wait(0);
  wait(0);
  wait(0);

  // release semaphore objects
  sem_close(Sg);
  sem_close(Sm);

  return 0;
}
