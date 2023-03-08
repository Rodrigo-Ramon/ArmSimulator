/***********************************************************************
* Name        : CAN_BUS_HCPS.c
* Date        : 01.09.2023
* Author      : Kaida Wu(Dan)  Peihao Xiang(Noah)
* Version     : 1.2
* Copyright   : HCPS Lab ag 2015-2023
* Description : CAN BUS comunication initialization and close 
                Send or receive 8 bytes in the encoder
***********************************************************************/
#include "CAN_BUS_HCPS.h"

int CAN_BUS_Init(void)
{
    struct ifreq ifr_send;
    struct ifreq ifr_receive;
    struct sockaddr_can addr_send;
    struct sockaddr_can addr_receive;
    

    memset(&frame_send, 0, sizeof(struct can_frame));                          // Open up the memory space for CAN
    memset(&frame_receive, 0, sizeof(struct can_frame));

    if(system("sudo ip link set can0 type can bitrate 1000000") == -1)         // Open and set CAN interface
    {
        perror("sudo set failed");
        return -1;
    }

    if(system("sudo ifconfig can0 up") == -1)
    {
        perror("sudo ifconfig failed");
        return -1;
    }

    CAN_Send_file_descriptor = socket(PF_CAN, SOCK_RAW, CAN_RAW);              // Create socket
    CAN_Receive_file_descriptor = socket(PF_CAN, SOCK_RAW, CAN_RAW);

    if(CAN_Send_file_descriptor == -1) 
    {
        perror("socket PF_CAN failed in send");
        return -1;
    }

    if(CAN_Receive_file_descriptor == -1) 
    {
        perror("socket PF_CAN failed in receive");
        return -1;
    }
    
    strcpy(ifr_send.ifr_name, "can0");                                         // Device rename
    strcpy(ifr_receive.ifr_name, "can0");

    if(ioctl(CAN_Send_file_descriptor, SIOCGIFINDEX, &ifr_send) == -1)         // Control device
    {
        perror("ioctl failed in send");
        return -1;
    }

    if(ioctl(CAN_Receive_file_descriptor, SIOCGIFINDEX, &ifr_receive) == -1)
    {
        perror("ioctl failed in recv");
        return -1;
    }
    
    /* Set the socket attributes */
    addr_send.can_family = AF_CAN;
    addr_send.can_ifindex = ifr_send.ifr_ifindex;

    addr_receive.can_family = PF_CAN;
    addr_receive.can_ifindex = ifr_receive.ifr_ifindex;

    if(bind(CAN_Send_file_descriptor, (struct sockaddr *)&addr_send, sizeof(addr_send)) != 0)             // Bind the socket to can:
    {
        perror("bind failed in send");
        return -1;
    }

    if(bind(CAN_Receive_file_descriptor, (struct sockaddr *)&addr_receive, sizeof(addr_receive)) != 0)
    {
        perror("bind failed in receive");
        return -1;
    }

    fcntl(CAN_Receive_file_descriptor, F_SETFL, FNDELAY);                                                 // Set receive filer rule

    if(setsockopt(CAN_Send_file_descriptor, SOL_CAN_RAW, CAN_RAW_FILTER, NULL, 0) != 0)
    {
        perror("Set rule failed in send");
        return -1;
    }

    struct can_filter rfilter_receive[2];

    rfilter_receive[0].can_id = CAN_Receive_address0;
    rfilter_receive[0].can_mask = CAN_SFF_MASK;

    rfilter_receive[1].can_id = CAN_Receive_address1;
    rfilter_receive[1].can_mask = CAN_SFF_MASK;

    if(setsockopt(CAN_Receive_file_descriptor, SOL_CAN_RAW, CAN_RAW_FILTER, &rfilter_receive, sizeof(rfilter_receive)) != 0)
    {
        perror("Set rule failed in receive");
        return -1;
    }

    return 1;
}

int CAN_BUS_Send(char *command_buffer, bool motorNum)
{
    int send_character_length;
    
    frame_send.can_id = motorNum ? CAN_Send_address0, CAN_Send_address1 ;            // Set the identifier and DLC
    frame_send.can_dlc = 8;

    for(int i=0; i<8; i++)  
    {
        frame_send.data[i] = command_buffer[i];      // Fill in the data frame
    }

    send_character_length = write(CAN_Send_file_descriptor, &frame_send, sizeof(frame_send));

    if(send_character_length != sizeof(frame_send)) 
    {
        printf("CAN send failed\n");
        return -1;
    }

    return 1;
}

char* CAN_BUS_Receive()
{
    int receive_character_length;
    static char receive_buffer[8] = {0};

    receive_character_length = read(CAN_Receive_file_descriptor, &frame_receive, sizeof(frame_receive));

    if(receive_character_length > 0) 
    {
        for(int i = 0; i < 8; i++)
        {
            receive_buffer[i] =  frame_receive.data[i];           // Read frame data
        }
        
        receive_character_length = 0;
        memset(&frame_receive, 0, sizeof(struct can_frame));      // Clear cache

        return receive_buffer;
    }
    else
    {
        return NULL;
    }        
}

int CAN_BUS_Close(void)
{
    if(close(CAN_Send_file_descriptor) != 0)
    {
        perror("CAN BUS send close failed");
        return -1;
    }

    if(close(CAN_Receive_file_descriptor) != 0)
    {
        perror("CAN BUS receive close failed");
        return -1;
    }

    if(system("sudo ifconfig can0 down") == -1)
    {
        perror("CAN interface close failed");
        return -1;
    }

    return 1;
}