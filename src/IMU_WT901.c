#include "IMU_WT901.h"

static int ret;
static int fd;

char r_buf[1024] ={0};
//bzero(r_buf,1024);

//float Accel[3],Angular[3],Angle[3],Magneto[3];
//Structure of real data : Accel[3],Angular[3],Angle[3],Magneto[3];
float real_data[12] = {0}; 

void WT901_Init()
{
    fd = uart_open(fd,"/dev/ttyS0");/*Serial port number/dev/ttySn,USB number/dev/ttyUSBn */ 
    if(fd == -1)
    {
        fprintf(stderr,"uart_open error\n");
        exit(EXIT_FAILURE);
    }

    if(uart_set(fd,BAUD,8,'N',1) == -1)
    {
        fprintf(stderr,"uart set failed!\n");
        exit(EXIT_FAILURE);
    }


}



//****Uart config******
int uart_open(int fd,const char *pathname)
{
    fd = open(pathname, O_RDWR|O_NOCTTY); 
    if (-1 == fd)
    { 
        perror("Can't Open Serial Port"); 
		return(-1); 
	} 
    else
		printf("open %s success!\n",pathname);
    if(isatty(STDIN_FILENO)==0) 
		printf("standard input is not a terminal device\n"); 
    else 
		printf("isatty success!\n"); 
    return fd; 
}

int uart_set(int fd,int nSpeed, int nBits, char nEvent, int nStop)
{
     struct termios newtio,oldtio; 
     if  ( tcgetattr( fd,&oldtio)  !=  0) {  
      perror("SetupSerial 1");
	  printf("tcgetattr( fd,&oldtio) -> %d\n",tcgetattr( fd,&oldtio)); 
      return -1; 
     } 
     bzero( &newtio, sizeof( newtio ) ); 
     newtio.c_cflag  |=  CLOCAL | CREAD;  
     newtio.c_cflag &= ~CSIZE;  
     switch( nBits ) 
     { 
     case 7: 
      newtio.c_cflag |= CS7; 
      break; 
     case 8: 
      newtio.c_cflag |= CS8; 
      break; 
     } 
     switch( nEvent ) 
     { 
     case 'o':
     case 'O': 
      newtio.c_cflag |= PARENB; 
      newtio.c_cflag |= PARODD; 
      newtio.c_iflag |= (INPCK | ISTRIP); 
      break; 
     case 'e':
     case 'E': 
      newtio.c_iflag |= (INPCK | ISTRIP); 
      newtio.c_cflag |= PARENB; 
      newtio.c_cflag &= ~PARODD; 
      break;
     case 'n':
     case 'N': 
      newtio.c_cflag &= ~PARENB; 
      break;
     default:
      break;
     } 

     /*set baud rate*/ 

    switch( nSpeed ) 
    { 
            case 2400: 
            cfsetispeed(&newtio, B2400); 
            cfsetospeed(&newtio, B2400); 
            break; 
            case 4800: 
            cfsetispeed(&newtio, B4800); 
            cfsetospeed(&newtio, B4800); 
            break; 
            case 9600: 
            cfsetispeed(&newtio, B9600); 
            cfsetospeed(&newtio, B9600); 
            break; 
            case 115200: 
            cfsetispeed(&newtio, B115200); 
            cfsetospeed(&newtio, B115200); 
            break; 
            case 460800: 
            cfsetispeed(&newtio, B460800); 
            cfsetospeed(&newtio, B460800); 
            break; 
            default: 
            cfsetispeed(&newtio, B9600); 
            cfsetospeed(&newtio, B9600); 
            break; 
    } 

     if( nStop == 1 ) 
      newtio.c_cflag &=  ~CSTOPB; 
     else if ( nStop == 2 ) 
      newtio.c_cflag |=  CSTOPB; 
     newtio.c_cc[VTIME]  = 0; 
     newtio.c_cc[VMIN] = 0; 
     tcflush(fd,TCIFLUSH); 

if((tcsetattr(fd,TCSANOW,&newtio))!=0) 
     { 
      perror("com set error"); 
      return -1; 
     } 
     printf("set done!\n"); 
     return 0; 
}

int uart_close(int fd)
{
    assert(fd);
    close(fd);

    return 0;
}


int send_data(int  fd, char *send_buffer,int length)
{
	length=write(fd,send_buffer,length*sizeof(unsigned char));
	return length;
}


int recv_data(int fd, char* recv_buffer,int length)
{
	length=read(fd,recv_buffer,length);
	return length;
}



void ParseData(char chr)
{
		static char chrBuf[100];
		static unsigned char chrCnt=0;
		signed short sData[4];
		unsigned char i = 0;
		char cTemp=0;
		//time_t now;
		chrBuf[chrCnt++]=chr;
		if (chrCnt<11) return;
		for (i=0;i<10;i++) cTemp+=chrBuf[i];
		if ((chrBuf[0]!=0x55)||((chrBuf[1]&0x50)!=0x50)||(cTemp!=chrBuf[10])) 
        {
            printf("Error:%x %x\r\n",chrBuf[0],chrBuf[1]);
            memcpy(&chrBuf[0],&chrBuf[1],10);
            chrCnt--;
            return;
        }
		
		memcpy(&sData[0],&chrBuf[2],8);
		switch(chrBuf[1])
		{
            case 0x51: // Accelerate
                //for (i=0;i<=2;i++) real_data[i] = (float)sData[i]/32768.0*16.0;
                // time(&now);
                // printf("\r\nT:%s a:%6.3f %6.3f %6.3f ",asctime(localtime(&now)),a[0],a[1],a[2]);
                break;
            case 0x52: // Angular
               // for (i=3;i<=5;i++) real_data[i] = (float)sData[i-3]/32768.0*2000.0;
                // printf("w:%7.3f %7.3f %7.3f ",w[0],w[1],w[2]);					
                break;
            case 0x53: // Angle
                for (i=6;i<=8;i++) real_data[i] = (float)sData[i-6]/32768.0*180.0;
                //for (i=0;i<3;i++) Angle[i] = (float)sData[i]/32768.0*180.0;
                //printf("A:%7.3f %7.3f %7.3f\r\n ",Angle[0]+95,Angle[1],Angle[2]);
                break;
            case 0x54: // Magneto
                //for (i=9;i<=11;i++) real_data[i] = (float)sData[i-9];
                // printf("h:%4.0f %4.0f %4.0f ",h[0],h[1],h[2]);
                break;
		}		
		chrCnt=0;		
}


/*
option: 1


*/

float* WT901_Get_data()
{
    ret = recv_data(fd,r_buf,44);
    if(ret == -1)
    {
        fprintf(stderr,"uart read failed!\n");
        //exit(EXIT_FAILURE);
    }

    for (int i=0;i<ret;i++) 
    {
        ParseData(r_buf[i]);
    } 
    
    return real_data; // get WT901 data and print out.


}

// void WT901_Get_data()
// {
//     ret = recv_data(fd,r_buf,44);
//     if(ret == -1)
//     {
//         fprintf(stderr,"uart read failed!\n");
//         //exit(EXIT_FAILURE);
//     }

//     for (int i=0;i<ret;i++) 
//     {
//         ParseData(r_buf[i]);
//     } 

// }

void WT901_Close()
{
    ret = uart_close(fd);
    if(ret == -1)
   {
    fprintf(stderr,"uart_close error\n");
    //exit(EXIT_FAILURE);
   }
}