#include <stdio.h>
#include <pthread.h>
#include <assert.h>
#include <stdlib.h>
#include "thread_pool.h"

void         *thread_pool_init(unsigned int max_thread_num);
static void  *thread_routine(void *arg);
void          add_task(void *pool, void *(*task)(void *arg), void *arg);

void *
thread_pool_init(unsigned int max_thread_num)
{
    int i;
    CThread_Pool *pool = NULL;

    printf("thread_pool_init\n");
    pool = (CThread_Pool *) malloc(sizeof(CThread_Pool));
    memset(pool, NULL, sizeof(CThread_Pool));
    pthread_mutex_init(&(pool->queue_lock), NULL);
    pthread_cond_init(&(pool->queue_ready), NULL);
    pool->shutdown = 0;
    pool->max_thread_num = max_thread_num;
    pool->cur_queue_size = 0;
    pool->task_list_queue_head = NULL;
    pool->thread_tids = (pthread_t *)malloc(sizeof(pthread_t) * max_thread_num);
    memset(pool->thread_tids, NULL, sizeof(pthread_t) * max_thread_num);

    for (i = 0; i < max_thread_num; i++) {
         //pthread_create(&(pool->thread_tids[i]), NULL, thread_routine(arg), (void *) arg);
         pthread_create(&(pool->thread_tids[i]), NULL, thread_routine, NULL);
    }

    return pool;
}

static void *
thread_routine(void *arg)
{
    Task_List_t *worker = NULL;

    worker = (Task_List_t *) malloc(sizeof(Task_List_t));
    printf("starting working thread = %x\n", pthread_self()); 
    while (1) {
        pthread_mutex_lock(&(pool->queue_lock));
        while (pool->cur_queue_size == 0 && pool->shutdown == 0) {
            printf("thread 0x%x is wainting\n", pthread_self());
            pthread_cond_wait(&(pool->queue_ready), &(pool->queue_lock));
        }

        if (pool->shutdown) {
            pthread_mutex_unlock(&(pool->queue_lock));
            printf("the thread 0x%x will exiting\n",  pthread_self());
            pthread_exit(NULL);
        }

        printf ("thread 0x%x is starting to work\n", pthread_self ());

        assert(pool->cur_queue_size != 0);  
        assert(pool->task_list_queue_head != NULL);

        pool->cur_queue_size--;             
        worker = pool->task_list_queue_head;
        pool->task_list_queue_head = worker->next;

        pthread_mutex_unlock(&(pool->queue_lock));

        //execute mask;
        (*(worker->task))(worker->arg);
        free(worker);
        worker = NULL;
    }
    //pthread_exit (NULL);该行代码不执行
}

void
add_task(void *pool, void *(*task)(void *arg), void *arg)
{
    Task_List_t *worker = NULL;
    Task_List_t *newworker = (Task_List_t *) malloc(sizeof(Task_List_t));

    newworker->task = task;
    newworker->arg = arg;
    newworker->next = NULL;

    pthread_mutex_lock(&(pool->queue_lock));
    worker = pool->task_list_queue_head;

    if (pool->task_list_queue_head == NULL) {
        pool->task_list_queue_head = newworker;
    } else {
        while (worker->next != NULL) {
            worker = worker->next;
        }
        worker->next = newworker;
    }
    
    /*
     * judge whether task_list_queue_head is NULL,
     * if task_list_queue_head is NULL, prints an error message to standard error; 
     */
    assert(pool->task_list_queue_head != NULL);

    pool->cur_queue_size++;
    pthread_mutex_unlock(&(pool->queue_lock));
    pthread_cond_signal(&(pool->queue_ready));
}


void
destroy_pool(CThread_Pool *pool)
{
    int i = 0;
    Task_List_t *head = NULL;

    if (pool->shutdown) {
            printf("please not call function destroy_pool() twice;\n");
            exit(-1);
    }

    pool->shutdown = 1;
    pthread_cond_broadcast (&(pool->queue_ready));

    //join with a terminated thread;
    for (i = 0; i < pool->max_thread_num; i++) {
        pthread_join(pool->thread_tids[i], NULL);
    }
    free(pool->thread_tids);
    pool->thread_tids = NULL;

    //destory waiting queue;
    while (pool->task_list_queue_head != NULL) {
        head = pool->task_list_queue_head;
        pool->task_list_queue_head = (pool->task_list_queue_head)->next;
        free(head);
    }

    //destroy the mutex object referenced  and the given condition variable specified
    pthread_mutex_destroy(&(pool->queue_lock));
    pthread_cond_destroy(&(pool->queue_ready));

    free(pool);
    pool = NULL;
}


/*
   static void *
   test_process (void *arg)
   {
   printf("working thread's id is %x, working on task %d \n",  pthread_self(), *(int *) arg);
   sleep(1);

   return ((void *)0);
   }



   int
   main()
   {
   int i;
   int *task = NULL;

   thread_pool_init(4);
//assign 10 task;
task = (int *)malloc(sizeof(int)*10);

for (i = 0; i < 10; i++) {
task[i] = i + 1;
add_task(pool, test_process, &task[i]); 
printf("%d task has been executed successful\n", i);
}

sleep(20);
destroy_pool(pool);
free(task);
task = NULL;

return 0;
}
*/