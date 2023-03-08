/***********************************************************************
* Name        : batton_key.c
* Date        : 01.05.2023
* Author      : Kaida Wu(Dan)  Peihao Xiang(Noah)
* Version     : 1.0
* Copyright   : HCPS Lab ag 2015-2023
* Description : Keypad initialization and detection 
                For emergency events
***********************************************************************/
#include "botton_key.h"

int Button_Init(void)
{
    static struct termios original_attributes;
    static struct termios current_attributes;

    if(tcgetattr(STDIN_FILENO, &original_attributes) != 0)                           // Get Standard I/O Original Attributes
    {
        perror("Get serial attributes failed");
        return -1;
    }

    memcpy(&current_attributes, &original_attributes, sizeof(current_attributes) );  // Copy the original attribute to the current attribute

    /* Set new attributes to the current attribute */
    current_attributes.c_lflag &= ~ICANON;
    current_attributes.c_cc[VMIN] = 1;
    current_attributes.c_cc[VTIME] = 0;

    if(tcsetattr(STDIN_FILENO, TCSANOW, &current_attributes) != 0)                   // Set terminal current attributes:
    {
        perror("Set new terminal attributes failed");
        return -1;
    }

    return 1;
}

int Button_detection(void)
{
    fd_set Button_file_descriptor;
    struct timeval timeout;
    int Button_detection_flag;

    FD_ZERO(&Button_file_descriptor);
    FD_SET(0, &Button_file_descriptor);

    timeout.tv_sec  = 0;
    timeout.tv_usec = 0;

    Button_detection_flag = select(1, &Button_file_descriptor, NULL, NULL, &timeout);

    if(Button_detection_flag == -1)
    {
        perror("Select error");
        return 0;
    }
    else if(Button_detection_flag)
    {
        return 1;
    }
    else
    {
        return 0;
    }

    return 0;
}