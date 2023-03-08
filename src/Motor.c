/***********************************************************************
* Name        : Motor.c
* Data        : 01.27.2023
* Author      : Peihao Xiang(Noah)
* Version     : 1.0
* Copyright   : HCPS Lab ag 2015-2023
* Description : Motor initialization and detection 
                (Error flag, PID Parameter, Acceleration, 
                 Zero Point, Reset)
***********************************************************************/
#include "Motor.h"

char send_command[8] = {0};
char receive_command[8] = {0};
char *received_command = NULL;

int Motor_detection(void)
{
    uint16_t Error_State;

    /* Read motor status command */
    send_command[0] = 0x9A;
    send_command[1] = 0x00;
    send_command[2] = 0x00;
    send_command[3] = 0x00;
    send_command[4] = 0x00;
    send_command[5] = 0x00;
    send_command[6] = 0x00;
    send_command[7] = 0x00;

    if(CAN_BUS_Send(send_command) == -1)
    {
        perror("Motor detection send failed");
        return -1;
    }

    usleep(10000);

    received_command = CAN_BUS_Receive();

    if(received_command != NULL)
    {
        for(int i=0; i<8; i++)
        {
            receive_command[i] = received_command[i];
        }

        Error_State = (receive_command[7]<<8)+(receive_command[6]);       // Output error status
    }
    else
    {
        perror("Motor detection received failed");
        return -1;
    }

    if(Error_State != 0)
    {
        printf("Error Value: %x\n", Error_State);
        perror("Motor error");
        return -1;
    }

    return 1;
}

int Motor_Init(void)
{
    uint8_t CurrentKP = 50;
    uint8_t CurrentKI = 50;
    uint8_t SpeedKP = 100;
    uint8_t SpeedKI = 5;
    uint8_t PositionKP = 30;
    uint8_t PositionKI = 3;

    uint32_t Speed_accel = 100;                                            // Acceleration value, Unit is dps/s
    int32_t Position_accel;
    int32_t ZeroPoint = -165375;

    if(Motor_detection() == -1)
    {
        return -1;
    }

    usleep(10000);

    /* Set the PID parameter command */
    send_command[0] = 0x32;
    send_command[1] = 0x00;
    send_command[2] = (u_int8_t)CurrentKP;
    send_command[3] = (u_int8_t)CurrentKI;
    send_command[4] = (u_int8_t)SpeedKP;
    send_command[5] = (u_int8_t)SpeedKI;
    send_command[6] = (u_int8_t)PositionKP;
    send_command[7] = (u_int8_t)PositionKI;

    if(CAN_BUS_Send(send_command) == -1)
    {
        perror("Set PID failed");
        return -1;
    }

    usleep(10000);

    /* Read the PID parameter command */
    send_command[0] = 0x30;
    send_command[1] = 0x00;
    send_command[2] = 0x00;
    send_command[3] = 0x00;
    send_command[4] = 0x00;
    send_command[5] = 0x00;
    send_command[6] = 0x00;
    send_command[7] = 0x00;

    if(CAN_BUS_Send(send_command) == -1)
    {
        perror("Read PID failed");
        return -1;
    }

    usleep(10000);

    received_command = CAN_BUS_Receive();

    if(received_command != NULL)
    {
        for(int i=0; i<8; i++)
        {
            receive_command[i] = received_command[i];
        }
        
        printf("Current KP: %d\n", receive_command[2]);
        printf("Current KI: %d\n", receive_command[3]);
        printf("Speed KP: %d\n", receive_command[4]);
        printf("Speed KI: %d\n", receive_command[5]);
        printf("Position KP: %d\n", receive_command[6]);
        printf("Position KI: %d\n", receive_command[7]);
    }
    else
    {
        perror("Read PID failed");
        return -1;
    }

    usleep(10000);
    received_command = CAN_BUS_Receive();

    /* Set the speed acceleration command */
    send_command[0] = 0x43;
    send_command[1] = 0x02;
    send_command[2] = 0x00;
    send_command[3] = 0x00;
    send_command[4] = (u_int8_t)Speed_accel;
    send_command[5] = (u_int8_t)(Speed_accel >> 8);
    send_command[6] = (u_int8_t)(Speed_accel >> 16);
    send_command[7] = (u_int8_t)(Speed_accel >> 24);

    if(CAN_BUS_Send(send_command) == -1)
    {
        perror("Set acceleration failed");
        return -1;
    }

    usleep(20000);
    received_command = CAN_BUS_Receive();

    /* Read position acceleration command */
    send_command[0] = 0x42;
    send_command[1] = 0x00;
    send_command[2] = 0x00;
    send_command[3] = 0x00;
    send_command[4] = 0x00;
    send_command[5] = 0x00;
    send_command[6] = 0x00;
    send_command[7] = 0x00;

    if(CAN_BUS_Send(send_command) == -1)
    {
        perror("Read acceleration failed");
        return -1;
    }

    usleep(10000);

    received_command = CAN_BUS_Receive();

    if(received_command != NULL)
    {
        for(int i=0; i<8; i++)
        {
            receive_command[i] = received_command[i];
        }
        
        Position_accel = (receive_command[7]<<24)+(receive_command[6]<<16)+(receive_command[5]<<8)+(receive_command[4]);
        printf("Position acceleration: %d dps/s\n", Position_accel);
    }
    else
    {
        perror("Read acceleration failed");
        return -1;
    }

    usleep(10000);

    /* Read zero point command */
    send_command[0] = 0x62;
    send_command[1] = 0x00;
    send_command[2] = 0x00;
    send_command[3] = 0x00;
    send_command[4] = 0x00;
    send_command[5] = 0x00;
    send_command[6] = 0x00;
    send_command[7] = 0x00;

    if(CAN_BUS_Send(send_command) == -1)
    {
        perror("Read zero point failed");
        return -1;
    }

    usleep(10000);

    received_command = CAN_BUS_Receive();

    if(received_command != NULL)
    {
        for(int i=0; i<8; i++)
        {
            receive_command[i] = received_command[i];
        }
        
        ZeroPoint = (receive_command[7]<<24)+(receive_command[6]<<16)+(receive_command[5]<<8)+(receive_command[4]);
        printf("Zero Point: %d Pulse\n", ZeroPoint - (-165375));
    }
    else
    {
        perror("Read zero point failed");
        return -1;
    }

    usleep(10000);

    /* Set zero point command */
    if(ZeroPoint != -165375)
    {
        ZeroPoint = -165375;

        send_command[0] = 0x63;
        send_command[1] = 0x00;
        send_command[2] = 0x00;
        send_command[3] = 0x00;
        send_command[4] = (u_int8_t)ZeroPoint;
        send_command[5] = (u_int8_t)(ZeroPoint >> 8);
        send_command[6] = (u_int8_t)(ZeroPoint >> 16);
        send_command[7] = (u_int8_t)(ZeroPoint >> 24);

        if(CAN_BUS_Send(send_command) == -1)
        {
            perror("Set zero point failed");
            return -1;
        }

        usleep(10000);

        send_command[0] = 0x76;
        send_command[1] = 0x00;
        send_command[2] = 0x00;
        send_command[3] = 0x00;
        send_command[4] = 0x00;
        send_command[5] = 0x00;
        send_command[6] = 0x00;
        send_command[7] = 0x00;

        if(CAN_BUS_Send(send_command) == -1)
        {
            perror("Reset failed");
            return -1;
        }

        printf("Reset Successful!\n");
        sleep(5);
    }

    return 1;
}