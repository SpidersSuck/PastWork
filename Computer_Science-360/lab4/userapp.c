#include "userapp.h"
#include <stdio.h>
#include <stdlib.h>
/************************Register process with kernel module************************/
static void pid_register() {
    char * buf = malloc(MAX_BUF_SIZE * sizeof(char));
    int len;
    memset(buf, 0, MAX_BUF_SIZE * sizeof(char));
    int fd = open("/proc/kmlab/status", O_RDWR);
    pid_t pid = getpid();
    len = sprintf(buf, "%d", pid);
    write(fd, buf, len);
    printf("PID[%d]", pid);
    free(buf);
}
/******************************************************************/

/******************Read from the proc file ********************************************/
static void pid_list() {
    char * buf = malloc(MAX_BUF_SIZE * sizeof(char));
    int len;
    memset(buf, 0, MAX_BUF_SIZE * sizeof(char));
    int fd = open("/proc/kmlab/status", O_RDWR);
    len = read(fd, buf, MAX_BUF_SIZE * sizeof(char));
    printf("%s", buf);
    free(buf);
}
static unsigned long factor(unsigned n) {
    if (n <= 1) 
	return 1;
    return factor(n - 1) * (unsigned long)n;
}
/******************************************************************/

/******************************************************************/
int main(int argc, char* argv[])
{
   // int __expire = 10;
   // time_t start_time = time(NULL);
   // if (argc == 2) {
    //    __expire = atoi(argv[1]);
    //}
    unsigned i, j;
    pid_register();
    for (i = 1; i < 200000; i++) {
	for (j = 1; j < 16; j++)
	    printf(" %d: %lu\n", j, factor(j));
    }
    pid_list();// Terminate user application if the time is expired
    return 0;
}
