#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <net/if.h>
#include <linux/can.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <linux/can/raw.h>

#define CAN_Send_address0 0x141           // The address of the device you want to send
#define CAN_Receive_address0 0x241        // The address of this device

#define CAN_Send_address1 0x142           // The address of the device you want to send
#define CAN_Receive_address1 0x242        // The address of this device

int CAN_Send_file_descriptor;
int CAN_Receive_file_descriptor;
struct can_frame frame_send;
struct can_frame frame_receive;

int CAN_BUS_Init(void);
int CAN_BUS_Send(char *command_buffer);     // Input the 8 bytes data (such as an array of 8)
char *CAN_BUS_Receive();                    // Declare an point to get the 8 bytes data.
int CAN_BUS_Close(void);