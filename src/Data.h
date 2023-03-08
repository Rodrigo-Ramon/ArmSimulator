#include <math.h>
#include <time.h>
#include <stdio.h>
#include <signal.h>
#include <pthread.h>
#include <sys/wait.h>

FILE* file_pointer;

int file_Init(void);