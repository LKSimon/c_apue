#ifndef THREAD_POOL_H
#define THREAD_POOL_H
#include <pthread.h>

typedef struct Task_List_s {
    void               *arg;
    void               *(*task)(void *arg);
    struct Task_List_s *next;
}Task_List_t;

typedef struct {
    pthread_mutex_t queue_lock;
    pthread_cond_t  queue_ready;

    int             shutdown;//1:destory thread pool.
    int             max_thread_num;//max thread number which is allowed in thread pool.
    int             cur_queue_size;//the task number of current waiting queue.
 
    Task_List_t    *task_list_queue_head;
    pthread_t      *thread_tids;//storage thread id.
}CThread_Pool;

void         *thread_pool_init(unsigned int max_thread_num);
static void  *thread_routine(void *arg);
void          add_task(void *pool, void *(*task)(void *arg), void *arg);
#endif