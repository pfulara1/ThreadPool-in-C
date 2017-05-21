
/* ========================== STRUCTURES ============================ */

/* Thread structure */
struct ThreadId{                      
	pthread_mutex_t  work_mutex;
	pthread_cond_t   work_cond;
	pthread_cond_t   done_cond;
	int id;
	pthread_t  pthread;  
	int *work;
	int *not_ideal; 
	void*   (*function)(void* arg);       /* function pointer          */
	void* arg; 
	void* ret;                        
}; 


/* Threadpool structure */
struct ThreadPool{    
    struct ThreadId *threads;              
	volatile int alive;      
	volatile int working;    
	pthread_mutex_t  thread_count_lock;                        
};


/* =================================== API ======================================= */


int ThreadPool_construct(struct ThreadPool* pool);

void* ThreadPool_run(struct ThreadPool*, struct ThreadId *, void* (*function_p)(void*), void* arg_p);

int ThreadPool_join(struct ThreadId *,void **);

int ThreadPool_destruct(struct ThreadPool*);

