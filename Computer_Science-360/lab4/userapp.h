#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#define MAX_BUF_SIZE 4096
static unsigned long factor(unsigned n);
static void pid_register();
static void pid_list();
