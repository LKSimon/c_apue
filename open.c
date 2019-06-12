#include <stdio.h>
#include <sys/stat.h>
#include <string.h>                                                                                                                                                                                 
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

static void                                                                                                                                                                                         
write_chld_pid(pid_t chld_pid, unsigned int write_byte_num)
{                                                                                                                                                            
    int chid_pfd;
    char buf[write_byte_num];
 
    memset(buf, 0, write_byte_num);
    chid_pfd = open("/root/homework/masterWorker/chld_pid", O_RDWR | O_TRUNC | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if (sprintf(buf, "%d", chld_pid) < 0) {
        printf("Error in function sprintf()\n");
    }
    write(chid_pfd, buf, strlen(buf));
    close(chid_pfd);
}
 
 
int
main()
{
    write_chld_pid(getpid(), 100);
 
    sleep(1000);
 
    return 0;
}