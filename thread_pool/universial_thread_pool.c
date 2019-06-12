#include <stdio.h>
#include <pthread.h>
#include <assert.h>
#include <stdlib.h>
#include "thread_pool.h"

void        *thread_pool_init(unsigned int max_thread_num);
static void *thread_pool_routine( void *arg);
void         add_task(CThread_Pool *pool, void *(*task)(void *arg), void *arg);
static void  creat_thread_pool(CThread_Pool *pool);


void *
thread_pool_init(unsigned int max_thread_num)
{
    CThread_Pool *pool;

    pool = (CThread_Pool *)malloc(sizeof(CThread_Pool));
    pthread_mutex_init(&(pool->queue_lock), NULL);
    pthread_cond_init(&(pool->queue_ready), NULL);
    pool->shutdown = 0;
    pool->max_thread_num = max_thread_num;
    pool->cur_queue_size = 0;
    pool->task_list_queue_head = NULL;
    pool->thread_tids = (pthread_t *)malloc(sizeof(pthread_t) * max_thread_num);

    creat_thread_pool(pool);

    return pool;
}


static void
creat_thread_pool(CThread_Pool *pool)
{
    unsigned int i;

    printf("creat_thread_pool\n");

    for (i = 0; i < pool->max_thread_num; i++) {
         //pthread_create(&(pool->thread_tids[i]), NULL, thread_routine, NULL);
         pthread_create(&(pool->thread_tids[i]), NULL, thread_pool_routine, pool);
    }
}


static void *
thread_pool_routine(void *arg)
{
    Task_List_t *worker = NULL;
    CThread_Pool *pool;

    worker = (Task_List_t *)malloc(sizeof(Task_List_t));
    pool = arg;
    printf("starting working thread = %x\n", pthread_self()); 
    while (1) {
        pthread_mutex_lock(&(pool->queue_lock));
        while (0 == pool->cur_queue_size && 0 == pool->shutdown) {
            printf("thread 0x%x is wainting\n", pthread_self());
            pthread_cond_wait(&(pool->queue_ready), &(pool->queue_lock));
        }

        if (pool->shutdown) {
            pthread_mutex_unlock(&(pool->queue_lock));
            printf("the thread 0x%x will exiting\n",  pthread_self());
            pthread_exit(NULL);
        }

        printf ("thread 0x%x is starting to work\n", pthread_self ());

        //check whether the queue task size is 0 or queue heal is null;
        assert(pool->cur_queue_size != 0);                                 
        assert(pool->task_list_queue_head != NULL);                        

        pool->cur_queue_size--;                                            
        worker = pool->task_list_queue_head;                               
        pool->task_list_queue_head = worker->next;                         

        pthread_mutex_unlock(&(pool->queue_lock)); 

        //execute mask;
        (*(worker->task))(worker->arg);//(worker->task)(worker->arg);
        free(worker);
        worker = NULL;
    }
    //pthread_exit (NULL);该行代码不执行
}

void
add_task(CThread_Pool *pool, void *(*task)(void *arg), void *arg)
{
    Task_List_t *worker = NULL;
    Task_List_t *newworker = (Task_List_t *)malloc(sizeof(Task_List_t));

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
    int          i = 0;
    Task_List_t *head = NULL;

    pthread_mutex_lock(&(pool->queue_lock));
    if (pool->shutdown) {
            printf("please not call function destroy_pool() twice;\n");
            exit(-1);
    }

    pool->shutdown = 1;
    pthread_cond_broadcast (&(pool->queue_ready));//唤醒处于挂起状态的线程，
    pthread_mutex_unlock(&(pool->queue_lock));
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
void
creat_produce_thread()
{   
    pthread_t produce_tid;

    if (pthread_create(&produce_tid, NULL, do_produce_thread, NULL) != 0) {
        printf("Error in creating prodece_thread\n");
    }
    printf("producer thread id = 0x%x\n", produce_tid);


    if (pthread_join(produce_tid, NULL) == 0) {
       printf("produce_tid: 0x%x has exit normally\n", produce_tid);
    }
}   


void *
do_produce_thread(void *argument)
{   
    printf("start do_produce_thread, pool->shutdown = %d\n", pool->shutdown);
    while (pool->shutdown == 0) {
        produce_task(TASK_NUM, pool); 
        sleep(20);
    }
    printf("pool->shutdown = %d\n", pool->shutdown);
}   
*/

/*
 ****** the code below is testing thread pool;
 */

/*
   static void *
   test_process (void *arg)
   {
   printf("working thread's id is %x, working on task %d \n",  pthread_self(), *(int *) arg);
   sleep(1);

   return NULL;
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
