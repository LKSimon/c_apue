#include <stdio.h>                                                                                                                                                                                  
#include <stdlib.h>
#include <pthread.h>
     
void *
func(void * arg)
{    
    //int a = 100001;该操作会有问题，需分配堆内存；
    int *a = NULL;
     
    a = (int *) malloc(sizeof(int));
    *a = 123;
    printf("a= %d\n", *a);
    pthread_exit((void *)a);
    //局部变量为栈内存，该线程推出时，其状态码由pthread_join(tid, void**p)中p指针指向该状态码地址，
    //但栈内存在程序结束后自动释放，故线程状态码传递的信息，若在另一程序或线程中运行需要分配堆内存；
}    
     
int  
main()
{    
    pthread_t tid = 0;
    void *p = NULL;
     
    pthread_create(&tid, NULL, func, NULL);
    sleep(1);
    pthread_join(tid, (void *)&p);
    printf("%d\n", *((int *)p));
     
    free(p);
     
    return 0;
}    