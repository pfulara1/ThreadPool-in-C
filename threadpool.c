#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/prctl.h>
#include "threadpool.h"
#include <assert.h>

volatile int init;

int ThreadPool_construct(struct ThreadPool * pool);
void* ThreadPool_run(struct ThreadPool* pool, struct ThreadId * Id , void* (*function_p)(void*), void* arg_p);
int  ThreadPool_destruct(struct ThreadPool* pool);
void thread_init (struct ThreadPool* pool, struct ThreadId *thread_p, int id);


//assign work to the threads from the job queue
static void* work_task(void * threadpool){

   struct ThreadId *pool_t=(struct ThreadId*)threadpool;
	/* Mark thread as alive (initialized) */
	while(1){
         pthread_mutex_lock(&pool_t->work_mutex);
 		 while(*pool_t->work==0){
  			pthread_cond_wait(&pool_t->work_cond, &pool_t->work_mutex);
 		 }
 		 *pool_t->not_ideal=1;
 		 pthread_mutex_unlock(&pool_t->work_mutex);
 		
         void* (*funcq)(void*);
         void*  arg;
         funcq= pool_t->function;
         arg  = pool_t->arg;
         pool_t->ret=funcq(arg); 
         
         pthread_mutex_lock(&pool_t->work_mutex);     
        *pool_t->work=0;
        *pool_t->not_ideal=0; 
         pthread_cond_signal(&pool_t->done_cond); 
         pthread_mutex_unlock(&pool_t->work_mutex);       

	}

	  return NULL;
}

/* Initialise thread pool */
int ThreadPool_construct(struct ThreadPool * pool){

	init=0;
	if (pool == NULL){
		fprintf(stderr,"ThreadPool_construct(): Could not allocate memory for thread pool\n");
		return -1;
	}
    //allocating space for 1000 threads
	pool->threads= malloc(1000*sizeof(struct ThreadId));
    
    pthread_mutex_init(&pool->thread_count_lock,NULL);

	fprintf(stderr, "%s\n","***Thread pool initialisation completed!***");

	return 0;
}


void thread_init (struct ThreadPool* pool, struct ThreadId *thread_p, int id){
	thread_p->id = id;
	thread_p->work= malloc(sizeof(int));
	thread_p->not_ideal=malloc(sizeof(int));
	*thread_p->work=0;
	*thread_p->not_ideal=1;    
	pthread_mutex_init(&thread_p->work_mutex,NULL);
	pthread_cond_init (&thread_p->work_cond, NULL);
    pthread_cond_init (&thread_p->done_cond, NULL);
	int rv =pthread_create(&thread_p->pthread, NULL, (void*)work_task, (void*)thread_p);
        assert(rv==0);

}

/* Add work to the thread pool */
void* ThreadPool_run(struct ThreadPool* pool, struct ThreadId* Id , void* (*function_p)(void*), void* arg_p){	
    int i=0,non_ideal_flag=0;
    pthread_mutex_lock(&pool->thread_count_lock);
	for(i=0;i<init;i++){
       struct ThreadId  *t=(pool->threads)+i;
       if(*t->not_ideal==0){
        t->function=function_p;
        t->arg=arg_p;
        *t->work=1;
        pthread_cond_signal(&t->work_cond);
        non_ideal_flag=1;
        Id=t;
        break;
     }
   }   
   if(non_ideal_flag==0){
    thread_init(pool,(pool->threads)+init,init);
    Id=(pool->threads)+init;
    Id->function=function_p;
    Id->arg=arg_p;
    *Id->work=1; 
    pthread_cond_signal(&Id->work_cond);
    init+=1;   
   }

   pthread_mutex_unlock(&pool->thread_count_lock);

   return Id;


}
  
int  ThreadPool_join(struct ThreadId * Id, void **x){	
  pthread_mutex_lock(&Id->work_mutex);
  while(*Id->not_ideal==1){
  	pthread_cond_wait(&Id->done_cond, &Id->work_mutex);
  }
  pthread_mutex_unlock(&Id->work_mutex);
  
  *x=Id->ret;

  return 0;
}

/* Destroy the threadpool */
int  ThreadPool_destruct(struct ThreadPool* pool){
	/* No need to destory if it's NULL */
	if (pool == NULL) return -1;

	free(pool);

	return 0;
}

