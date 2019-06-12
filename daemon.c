#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <fcntl.h>


void daemon()
{
    pid_t pid;
    int   fd;

    if ((pid = fork()) < 0) {
        perror("Error in fork");
    } else if (pid > 0) {
        exit(0);
    }

    fd = open("/dev/null", O_RDWR);
    if (fd < 0) {
        perror("Error in open");
        exit(1);
    }
    dup2(fd, 0);
    dup2(fd, 1);
    dup2(fd, 2);
    if (fd > 2) {
        close(fd);
    }//若0或1或2未copy到fd上，则fd为原来的值，此必大于2，需关闭；

    umask(0);

    setsid();

    chdir("/");

    return;
}

int main() {
    int   i;
    FILE *fp;

    daemon();

    fp = fopen("/tmp/daemon.log", "w+");
    if (fp < 0) {
        perror("Error in fopen()");
        exit(1);
    }

    for (i = 0; ; i++) {
        fprintf(fp, "%d\n", i);
        fflush(fp);
        sleep(1);
    }

    return 0;
}
