/**********************************************************************************
 *Lab: HCPS in FIU
 *Filename: CAN_BUS_HCPS.h CAN_BUS_HCPS.c
 *Author : Kaida(Dan) Wu
 *Version: v1.1
 *Update Date: 03-10-2023
 *Function: Embedeed Liunx CAN BUS comunication, include "send" and "recevice"
 * log: Update multiple motors CAN BUS comunication
 * Example: 
 *  
 * step 1: Set the address of Receive_CAN_BUS_addr1 in macro definition of this file
 * step 2: According to following demo for your code.
 int main()
    {
        char send_buf[8]={'1','2','3','4','5','6','7','8'};
        char rec_buf[10]={0}; // (rec_buf[0]<<8 + rec_buf[1]) is the received address
                              // The data is from rec_buf[3] to rec_buf[9] 
        char *recevied_buf = NULL;

        CAN_BUS_Init();
        usleep(1000);
        CAN_BUS_Send(send_buf,0x141); // set send address (motor address)
        usleep(1000);
    
        while(1)
        {
                    
            recevied_buf = CAN_BUS_Receive();
            if(recevied_buf != NULL)
            {  
                for(int i=0;i<10;i++)  
               {
                rec_buf[i] = recevied_buf[i];
               }

               if(((rec_buf[0]<<8 + rec_buf[1])) == 0x241)
                {
                    for(int i=3;i<10;i++) 
                    printf("%X\t",rec_buf[i]);
                }
            }
        }

        CAN_BUS_Close();
        return 0;
    }                    
 ********************************************************************************
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <sys/types.h>
#include <fcntl.h>
#include "keyboard_event.h"

//#define Send_CAN_BUS_addr 0x141   // The address of the device you want to send
#define Receive_CAN_BUS_addr1 0x241 // The address of this device group1
#define Receive_CAN_BUS_addr2 0x242 // The address of this device group1


const int readDelay = 1500; // in uS;
static char *can_reply = NULL;
static char rec_buf[9] = {0};

static char set_zero_pos_msg[8] =   {0x64,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
static char zero_pos_msg[8] =       {0xA4,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
static char shutdown_msg[8] =       {0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
static char get_position_msg[8] =   {0x92,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
static char get_abs_position_msg[8]=   {0x92,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
static char get_pid_msg[8] =        {0x30,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

//char cmd_buf[8]={0};
//char tx_flag = 0;

int CAN_BUS_Init();
void set_zero_cmd (int Send_Can_BUS_addr);
bool move_to_zero_cmd (int Send_Can_BUS_addr);
void shutdown_cmd (int Send_Can_BUS_addr);
float get_position_cmd(int Send_Can_BUS_addr);

void CAN_BUS_Send(char *command_buf,int Send_CAN_BUS_addr); //input the 8 bytes data (such as an array of 8)
char *CAN_BUS_Receive(int Send_CAN_BUS_addr, char cmd_number); // declare an point to get the 8 bytes data.
void CAN_BUS_Close();