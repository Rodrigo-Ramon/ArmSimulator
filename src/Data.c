/***********************************************************************
* Name        : Data.c
* Date        : 01.27.2023
* Author      : Peihao Xiang(Noah)
* Version     : 1.0
* Copyright   : HCPS Lab ag 2015-2023
* Description : File initialization
                For record data
***********************************************************************/
#include "Data.h"

int file_Init(void)
{
    file_pointer = fopen("data.dat","wr+");

    if(file_pointer == NULL)
    {
        perror("Open file failed");
        return -1;
    }

    return 1;
}