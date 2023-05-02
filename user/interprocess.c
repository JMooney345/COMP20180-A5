#include "kernel/types.h"
#include "kernel/riscv.h"
#include "kernel/memlayout.h"
#include "user/user.h"

 
// Number of data items in the array
#define N (10)

// Number of worker processes
#define M (3)

enum semaphores {
  SworkerCounter = 0, // Counts available workers
  SarrayLock = 1 // Controls access to shared array
};

struct data {
  int value;
  char processed;
};

struct shared_space {
  struct data a[N];  // array of N items
};

// situate shared_space in the shared virtual memory page
volatile struct shared_space *s = (volatile struct shared_space*) 0x3FFFFFD000;

// This method gets the next piece of data to be processed
int get_item(){
    int i;

    sem_wait(SarrayLock, 1); // locks access to array
    sem_wait(SworkerCounter, 1); // decreases available workers

    for (i = 0; i < N; i++){ // finds unprocessed index
        if (s->a[i].processed == 0){
            s->a[i].processed = 1;
            break;
        }
    }

    sem_post(SarrayLock,1); // unlocks access
    sem_post(SworkerCounter, 1); // frees worker

    if (i == N) {  // has all data been processed?
        i = -1;    // yes, return -1 in place of the item index
    }

    return i; // returns index of array to be processed
}



// worker process - keeps getting items from the array until there is no more
void worker(){
    int i;
    for(i = get_item(); i >= 0; i = get_item()){ // retrieves index to be processed
        sem_wait(SworkerCounter, 1); // reduces available workers
        sem_wait(SarrayLock, 1); // lock for critial section

// --------- do not modify the increment loop  --------
        for (int j=0; j <100000; j++){
            s->a[i].value++; // process i-th element of the array a[] by incrementing it 100000 times.
        }
// ---------------------------------------------------

        sem_post(SworkerCounter, 1); // frees worker
        sem_post(SarrayLock, 1); // unlocks array access

    }
    exit(0);
}



int
main(int argc, char *argv[]){
  int i;

  // initialise shared space
  printf("Initialising shared space...\n");
  for (i=0; i < N; i++){
     s->a[i].value = i;
     s->a[i].processed = 0;
  }
  
  sem_open(SworkerCounter,M); // initialises semaphores
  sem_open(SarrayLock,1);

    // create worker processes
  printf("Creating worker processes ...\n");
  for (i = 0; i < M; i++){
     if (fork() == 0) worker(); // start worker process
  }

  // wait for the child processes to terminate
  for (i=0; i<M; i++){
     wait(0);
  } 

  // print out the processed array
  printf("Processed data in the array:\n");
  for (i=0; i < N; i++){
     printf("a[%d] = %d\n",i,s->a[i].value);
  }

  sem_close(SarrayLock); // closes semaphores
  sem_close(SworkerCounter);

  return 0;
}
