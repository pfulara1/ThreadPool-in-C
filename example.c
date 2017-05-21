
#include <stdio.h>
#include <pthread.h>
#include "threadpool.h"
#include <assert.h>
#include <stdlib.h>
#include <time.h>

  void* task(){
  sleep(3);
  return 0;
}




int main(){
  
  struct ThreadPool * pool; 

  struct ThreadId* Id;

  pthread_t p[10];
  void* ret[10];
 
  clock_t t;  
  int rv,m;
  
  pool = (struct ThreadPool*)malloc(sizeof(struct ThreadPool));


   fprintf(stderr, "%s\n","Please wait for the tasks to complete!\n");

   t=clock();
   rv= ThreadPool_construct(pool);
   assert(rv==0);
   int i,up,down;

   for(i=0;i<10;i++) {
   Id=(struct ThreadId*)ThreadPool_run(pool, Id, task, NULL);
   sleep(1);
   }
   
   ThreadPool_join((struct ThreadId*)Id,(void**)&m);

   t=clock()-t;

   double time_taken=((double)t)/CLOCKS_PER_SEC;

   fprintf(stderr, "Time taken to complete 10 task with threadpool:%f\n",time_taken);

   ThreadPool_destruct(pool);


   fprintf(stderr,"\n%s\n","*****Now testing with pthreads!*****");
   
   t=clock();
   for(i=0;i<10;i++){
   pthread_create(&p[i],NULL,task,NULL);
   sleep(1);
  }
 
   for(i=0;i<10;i++) 
   pthread_join(p[i],&ret[i]);
  
   t=clock()-t;

   time_taken=((double)t)/CLOCKS_PER_SEC;
   
   fprintf(stderr, "Time taken to complete 10 task with pthread_create:%f\n",time_taken);


   return 0;
}
