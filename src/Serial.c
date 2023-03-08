/***********************************************************************
* Name        : Serial.c
* Date        : 01.05.2023
* Author      : Peihao Xiang(Noah)
* Version     : 1.1
* Copyright   : HCPS Lab ag 2015-2023
* Description : Serial comunication initialization and detection 
                Read float value from force sensor
***********************************************************************/
#include "Serial.h"

int Serial_Init(void)
{
    struct termios serial_attributes;

    Serial_file_descriptor = open(MODEMDEVICE, O_RDWR | O_NOCTTY | O_NONBLOCK);  // Open the serial device
    if(Serial_file_descriptor == -1)
    {
        perror(MODEMDEVICE);
        return -1;
    }

    memset(&serial_attributes, 0, sizeof(serial_attributes));                    // Open up memory space

    if(tcgetattr(Serial_file_descriptor, &serial_attributes) != 0)               // Get serial attributes
    {
        perror("Get serial attributes failed");
        return -1;
    }

    if(cfsetospeed(&serial_attributes, BAUDRATE) != 0)                           // Set the baud rate
    {
        perror("Set the baud rate failed in send");
        return -1;
    }

    if(cfsetispeed(&serial_attributes, BAUDRATE) != 0)
    {
        perror("Set the baud rate failed in receive");
        return -1;
    }

    /* Set new attributes */
    serial_attributes.c_cflag = (serial_attributes.c_cflag & ~CSIZE) | CS8;
    serial_attributes.c_iflag &= ~IGNBRK;
    serial_attributes.c_lflag = 0;
    serial_attributes.c_oflag = 0;
    serial_attributes.c_cc[VMIN]  = 0;
    serial_attributes.c_cc[VTIME] = 0;

    serial_attributes.c_iflag &= ~(IXON | IXOFF | IXANY);
    serial_attributes.c_cflag |= (CLOCAL | CREAD);
    serial_attributes.c_cflag &= ~(PARENB | PARODD);
    serial_attributes.c_cflag |= 0;
    serial_attributes.c_cflag &= ~CSTOPB;
    serial_attributes.c_cflag &= ~CRTSCTS;

    if(tcsetattr (Serial_file_descriptor, TCSANOW, &serial_attributes) != 0)     // Set new serial attributes
    {
        perror("Set new serial attributes failed");
        return -1;
    }

    /* Set the I/O event occurrence attributes */
    Serial_file_descriptor_event[0].fd = Serial_file_descriptor;
    Serial_file_descriptor_event[0].events = POLLRDNORM;

    return 1;
}

int Serial_detection(void)
{
    if(poll(Serial_file_descriptor_event, 1, -1) > 0)                            // Triggering event
    {
        if(Serial_file_descriptor_event[0].revents & POLLERR)                    // Check serial events
        {
            perror("An error has occurred on the device");
            return -1;
        }

        if(Serial_file_descriptor_event[0].revents & POLLHUP)
        {
            perror("The device has been disconnected");
            return -1;
        }

        if(Serial_file_descriptor_event[0].revents & POLLNVAL)
        {
            perror("The specified file descriptor value is invalid");
            return -1;
        }

        if(Serial_file_descriptor_event[0].revents & POLLRDNORM)
        {
            return 1;
        }
    }
    else
    {
        perror("Serial detection failed");
        return -1;
    }
}

float Serial_read(void)
{
    float value;
    int character_length;
    char sensor_buffer[255] = {0};

    if(Serial_detection() == 1)
    {
        character_length = read(Serial_file_descriptor, sensor_buffer, 255);

        if(character_length > 0)
        {
            sensor_buffer[character_length] = 0;                                 // Terminate buffer
            sscanf(sensor_buffer, "%f", &value);                                 // Convert decimal data type

            return value;
        }
        else
        {
            perror("Failed to read serial data");
            return 0;
        }
    }
    else
    {
        perror("Failed to read serial data");
        return 0;
    }
}