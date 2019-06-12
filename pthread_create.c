#include <stdio.h>                                                                                                                                                                                  
#include <stdlib.h>
#include <pthread.h>
     
void 
printtid(const char *string)
{    
    pid_t       pid;
    pthread_t   tid;
     
    pid = getpid();
    tid = pthread_self();
     
    printf("%s pid = %lu, tid = %lu\n", string, pid, tid);
}    
     
     
     
void *
thread_func(void *arg)
{    
     printtid("new thread: ");
     return ((void *)0);
}    
     
     
int  
main()
{    
    pthread_t   ntid;
    if ((ntid = pthread_create(&ntid, NULL, thread_func, NULL)) != 0) {
        printf("Error in pthread_create\n");
    }
     
    sleep(2);
    printf("******pid = %d, tid = %d \n", getpid(), pthread_self());
    printf("******pid = %d, tid = %d \n", getpid(), pthread_self());
     
	return 0;
}