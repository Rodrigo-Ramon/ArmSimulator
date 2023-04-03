/************************************************************************************
 *Lab: HCPS in FIU
 *Filename: IMU_WT901.h IMU_WT901.c
 *Author : Kaida(Dan) Wu
 *Version: v1.0
 *Update Date: 12-21-2022
 *Function: get IMU sensor WT901 all data directly one time.
 *          data: Acceleration[3],Angular[3],Angle[3],magnet[3]
 *Use guildline: Step1: call WT901_Init()
 *               Step2: declare a point to receive array from WT901_Get_data()
 *                      float *p = NULL;
 *               Step3:  p = WT901_Get_data();
 *               Step4: p[0],p[1],p[2] are acceleration x,y,z
 *                      p[3],p[4],p[5] are angular x,y,z
 *                      p[6],p[7],p[8] are angle x,y,x
 *                      p[9],p[10],p[11] are magnet x,y,z
 *                      
 ***********************************************************************************
*/





#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
#include<unistd.h>
#include<assert.h>
#include<termios.h>
#include<string.h>
#include<sys/time.h>
#include<time.h>
#include<sys/types.h>
#include<errno.h>
#include <errno.h>
#include <string.h>

#define BAUD 115200 //115200 for WT901 ,9600 for others

//float Angle[3];

int uart_open(int fd,const char *pathname);
int uart_set(int fd,int nSpeed, int nBits, char nEvent, int nStop);
int uart_close(int fd);
int send_data(int  fd, char *send_buffer,int length);
int recv_data(int fd, char* recv_buffer,int length);
void ParseData(char chr);
void WT901_Init();
float* WT901_Get_data();
//void WT901_Get_data();
void WT901_Close();
