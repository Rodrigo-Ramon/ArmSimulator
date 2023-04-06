#include "CAN_BUS_HCPS.h"


int ret_send, ret_recv;
int s_send, nbytes_send, s_recv, nbytes_recv;
struct sockaddr_can addr_send, addr_recv;
struct ifreq ifr_send, ifr_recv;
struct can_frame frame_send, frame_recv;

char* CAN_BUS_Receive(int motor_CANBUS_addr, char cmd_number)
{
    static char receive_buf[9] = {0};

    //printf("CANID %X, ", motor_CANBUS_addr);

    int replyID = motor_CANBUS_addr + 256;

    //printf("Expected_ID %X, ", replyID);

    nbytes_recv = read(s_recv, &frame_recv, sizeof(frame_recv));

    //printf("ID-0X%X \n", frame_recv.can_id);

        if(nbytes_recv > 0) 
        {
            
            //printf( "Checking, %X,%X,%X,%X", replyID, frame_recv.can_id ,cmd_number,frame_recv.data[0]);
            
            if ( cmd_number == frame_recv.data[0] &&  frame_recv.can_id == replyID )
            {
                
                receive_buf[0] = (0x0F & motor_CANBUS_addr);
                //printf("YES\n");
                for(int i = 1; i < 9; i++)
                {
                    receive_buf[i] =  frame_recv.data[i-1];
                }
               
                nbytes_recv =0;
                memset(&frame_recv, 0, sizeof(struct can_frame));
                return receive_buf;
            }
            else
            {
                //printf("NO\n");
                
                memset(&frame_recv, 0, sizeof(struct can_frame));
                return NULL;
            }        
        }
        else
        {
            memset(&frame_recv, 0, sizeof(struct can_frame));
            return NULL;
        }  
}

void CAN_BUS_Send(char *command_buf, int Send_CAN_BUS_addr)
{

    frame_send.can_id = Send_CAN_BUS_addr;
    frame_send.can_dlc = 8;
    for(int i=0;i<8;i++)  
        {
        frame_send.data[i] = command_buf[i];
        }
        //memset(cmd_buf, 0, sizeof(cmd_buf));

    //printf("can_id  = 0x%x\r\n", frame_send.can_id);
   // printf("can_dlc = %d\r\n",frame_send.can_dlc);
    //int i = 0;
    //for(i = 0; i < 8; i++)
        //printf("data[%d] = %x\r\n", i, frame_send.data[i]);

    //6.Send message
    nbytes_send = write(s_send, &frame_send, sizeof(frame_send)); 
    if(nbytes_send != sizeof(frame_send)) {
        printf("Send Error frame[0]!\r\n");
        //system("sudo ifconfig can0 down");
    }
    usleep(readDelay);
}

int CAN_BUS_Init()
{

    memset(&frame_send, 0, sizeof(struct can_frame));
    memset(&frame_recv, 0, sizeof(struct can_frame));


    system("sudo ip link set can0 type can bitrate 1000000"); //1Mbps
    system("sudo ifconfig can0 up");
    printf("----CAN communication - HCPS lab----\r\n");
        
    //1.Create socket
    s_send = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    s_recv = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (s_send < 0) {
        perror("socket PF_CAN failed in send");
        return 1;
    }

     if (s_recv < 0) {
        perror("socket PF_CAN failed in recv");
        return 1;
    }
    
    //2.Specify can0 device
    strcpy(ifr_send.ifr_name, "can0");
    strcpy(ifr_recv.ifr_name, "can0");
    ret_send = ioctl(s_send, SIOCGIFINDEX, &ifr_send);
    ret_recv = ioctl(s_recv, SIOCGIFINDEX, &ifr_recv);

    if (ret_send < 0) {
        perror("ioctl failed in send");
        return 1;
    }

    if (ret_recv< 0) {
        perror("ioctl failed in recv");
        return 1;
    }
    
    //3.Bind the socket to can0
    addr_send.can_family = AF_CAN;
    addr_send.can_ifindex = ifr_send.ifr_ifindex;

    addr_recv.can_family = PF_CAN;
    addr_recv.can_ifindex = ifr_recv.ifr_ifindex;

    ret_send = bind(s_send, (struct sockaddr *)&addr_send, sizeof(addr_send));
    ret_recv = bind(s_recv, (struct sockaddr *)&addr_recv, sizeof(addr_recv));

    if (ret_send < 0) {
        perror("bind failed in send");
        return 1;
    }

    if (ret_recv < 0) {
        perror("bind failed in recv");
        return 1;
    }
    
    fcntl(s_recv,F_SETFL,FNDELAY);//set to non-blocking mode for receive 

    //4.Disable filtering rules, do not receive packets, only send
    setsockopt(s_send, SOL_CAN_RAW, CAN_RAW_FILTER, NULL, 0);
    
    struct can_filter rfilter_recv[2];  // Set the group number of your recevied address
    
    rfilter_recv[0].can_id = Receive_CAN_BUS_addr1;
    rfilter_recv[0].can_mask = CAN_SFF_MASK;

    rfilter_recv[1].can_id = Receive_CAN_BUS_addr2;
    rfilter_recv[1].can_mask = CAN_SFF_MASK;

    setsockopt(s_recv, SOL_CAN_RAW, CAN_RAW_FILTER, &rfilter_recv, sizeof(rfilter_recv));
    printf("CAN Initiated Successfully\n");
    return 0;
}

void CAN_BUS_Close()
{
    close(s_send);
    close(s_recv);
    system("sudo ifconfig can0 down");
    printf("Quit!\r\n");

}



