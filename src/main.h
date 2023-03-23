#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <error.h>
#include <errno.h>
#include <sys/wait.h>
#include <math.h>

#include <iostream>
#include <iomanip>
#include <string>
#include <sys/time.h>

#include "CAN_BUS_HCPS.h"
#include "IMU_WT901.h"
#include "keyboard_event.h"

using namespace std;
#define pi 3.14159

pthread_t IMU_tid, Ctl_tid, keyboard_event_tid;
timer_t timerid = 0;
static pthread_mutex_t mutex;
static pthread_cond_t cond;

static u_int32_t servo_val = 200;
static u_int16_t max_speed = 200;
static int16_t Id = 1;

static int16_t raw_speed = 0;
static int16_t raw_angle = 0;
static int16_t raw_current = 0;


static char send_buf[8] = {0x60,0x00,0x00,0x00,0x00,0x00,0x00,0x00};


static char rec_buf[9] = {0};
static char speed_buf[8] = {0};
static int tty_set_flag;

static char *speed_Can_recevied_buf = NULL;
static float rad_acceleration = 0;
static short speed[2] = {0};
static short angle[2] = {0};
static u_int8_t counter = 0;


int32_t pos1_32t = 0;