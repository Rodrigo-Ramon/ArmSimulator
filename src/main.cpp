#include "main.h"


string GetLocalTimeWithMs(void)
{
    struct timeval curTime;
    gettimeofday(&curTime, NULL);
    int milli = curTime.tv_usec / 1000;
 
    char buffer[80] = {0};
    struct tm nowTime;
    localtime_r(&curTime.tv_sec, &nowTime); // let time value put into the memory
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &nowTime);
    char currentTime[84] = {0};
    snprintf(currentTime, sizeof(currentTime), "%s:%03d", buffer, milli);
    return currentTime;
}

void Timer_IRQ(union sigval v)
{
    
    //printf("POS - %f , %f\n", elbow_motor.get_position(), shoulder_motor.get_position());

    //     speed[1] = speed[0];
    //     speed[0] = 0;
    //     speed_buf[0] = 0xA1;
    //     speed_buf[1] = 0x00;
    //     speed_buf[2] = 0x00;
    //     speed_buf[3] = 0x00;
    //     speed_buf[4] = Id;
    //     speed_buf[5] = Id >> 8;
    //     speed_buf[6] = 0x00;
    //     speed_buf[7] = 0x00;

    //     CAN_BUS_Send(speed_buf, 0x141);
    //     speed_Can_recevied_buf = CAN_BUS_Receive();
    //     if (speed_Can_recevied_buf != NULL)
    //     {
    //         memmove(rec_buf,speed_Can_recevied_buf,8);
    //         // for (int i = 0; i < 8; i++)
    //         // {
    //         //     rec_buf[i] = recevied_buf[i];
    //         // }
    //         raw_current = (rec_buf[3] << 8) + (rec_buf[2]);
    //         raw_speed = (rec_buf[5] << 8) + (rec_buf[4]);
    //         raw_angle = (rec_buf[7] << 8) + (rec_buf[6]);
    //     }


    //     //printf("encoder:%d\tIMU:%7.3f\r\n",encoder_value,IMU_data[6]*100);
        
    //     // if(raw_speed > 32768)
    //     // {
    //     //     speed[0] = raw_speed - 65535;
    //     // }
    //     // else
    //     // {
    //     //     speed[0] = raw_speed;
    //     // }

    //     angle[0] = raw_angle;

    //     if(counter++ == 10)
    //     {
    //         angle[1] = angle[0];
    //         counter = 0 ;

    //     }
    //     speed[0] = raw_speed;
    //     rad_acceleration = ((float)(speed[0]-speed[1])/57.3)*20.0; 
    //     std::string current_time = GetLocalTimeWithMs();
    //     std::cout << current_time 
    //               << "\t"
    //               << "Angle:"
    //               << setw(5)
    //               << left
    //               << raw_angle
    //               << "\tCurrent:"
    //               << setw(5)
    //               << left
    //               << raw_current
    //               << "\tSpeed:"
    //               << setw(5)
    //               << left
    //               << raw_speed
    //               << "\tAcceleration:"
    //               << setw(5)
    //               << left
    //               << rad_acceleration
    //               << "\tIMU:"
    //               << setw(5)
    //               << left
    //               << servo_val
    //               << std::endl;
    //    // printf("Angle:%5d\tCurrent:%5d\tSpeed:%5d\tAcceleration:%7f\tIMU:%d\r\n", raw_angle, raw_current*100, raw_speed,rad_acceleration, servo_val);

    //     // pthread_mutex_unlock(&mutex); // unlock
    //     // pthread_cond_signal(&cond);   // send the condition signal
   

    
}

int init_timer(timer_t *timerid, struct sigevent *evp, struct itimerspec *it)
{
    if (!evp || !it)
        return -1;

    memset(evp, 0, sizeof(struct sigevent));

    evp->sigev_value.sival_int = 111;
    evp->sigev_notify = SIGEV_THREAD;
    evp->sigev_notify_function = Timer_IRQ; // timer function

    if (timer_create(CLOCK_REALTIME, evp, timerid) == -1)
    {
        perror("fail to timer_create");
        return -1;
    }

    printf("timer_create timerid = %d\n", *timerid);
    it->it_interval.tv_sec = 0;
    it->it_interval.tv_nsec = 50000000; // set time loop 50000000 = 20Hz
    it->it_value.tv_sec = 1;             // start time, that means after power on 1s, then start timer
    it->it_value.tv_nsec = 0;

    return 0;
}

int start_timer(timer_t *timerid, struct itimerspec *it)
{
    if (it == NULL)
    {
        return -1;
    }
    if (timer_settime(*timerid, TIMER_ABSTIME, it, NULL) == -1)
    {
        perror("fail to timer_settime");
        return -1;
    }

    return 0;
}

// float M = 0.0747;
// float M = 0.15;
// float Ka = 1 ;
// float D = 0.3;
// float Ks = 5; 
// float s_d = 2*(pi/180.0);
// float p_d = pi/2;
// int8_t direction = 1; 

//Left arm
// float Delta_Im = -0.01;
// float Im = 0;
// float Max_Im = -0.4;
// float Max_speed = 30;

//Right arm
float Delta_Im = 0.01;
float Im = 0;
float Max_Im = 1.2;
float Max_speed = 30;


static void *Control_thread(void *arg)
{
    char *recevied_buf = NULL;  
    u_int16_t encoder_angle = 0;
    //double motor1Pos;
    //move_to_zero(0x142);
    //set_zero_cmd(0x141);
    // if (move_to_zero_cmd(0x141)==false)
    // {
    //     //printf("QUITTINGMOVEZERO");
    //     //close_all();
    // }
    // usleep(500000);
    // move_to_zero_cmd(0x142);
    // usleep(500000);
    //Right arm
    for (;;)
    {
        //move_to_zero_cmd(0x141);
        
        //usleep(5000);
        time_last = high_resolution_clock::now();
        pthread_mutex_lock(&mutex); // lock
        elbow_motor.read_position();
       
        pthread_mutex_unlock(&mutex);
        pthread_cond_signal(&cond);

        time_now = high_resolution_clock::now();
        time_duration = time_now-time_last;
        //printf("T - %f\t", time_duration);
        //usleep(5000);
        
        shoulder_motor.read_position();
        //printf("POS2 - %f\n", motor2Pos);
        
        
    }
    pthread_exit(NULL);
}

static void *IMU_read_thread(void *arg)
{
    char *recevied_buf = NULL;
    float *IMU_data = NULL;

    for (;;)
    {
        IMU_data = WT901_Get_data();
        //pthread_mutex_lock(&mutex); // lock
        servo_val = (u_int32_t)(((1.0138 * (IMU_data[6] + 71.32) - 2.7326) * 100));
        //pthread_mutex_unlock(&mutex); // unlock
        //pthread_cond_signal(&cond);   // send the condition signal
        //usleep(5000);
    }
    pthread_exit(NULL);
}

static void *keyboard_event_thread(void *arg){
    bool flip = true;
    for (;;)
    {
        if (kbhit())
        {
            int key = getchar();
            printf("%X pressed\n", key);
            if (key == 0x1B)
            {
                close_all();
                break;
            }
            if (key == 0x7A)
            {
                if (flip == true)
                {
                    elbow_set_pos = 10;
                }
                else
                {
                    elbow_set_pos = 0.0;
                }
                
                flip = !flip;
                pthread_mutex_lock(&mutex); // lock
                pthread_cond_wait(&cond, &mutex); // wait for the condition
                
                
                elbow_motor.set_position(elbow_set_pos);    
                    
                pthread_mutex_unlock(&mutex); // unlock
            }
            if (key == 0x6B)
            {
                pthread_mutex_lock(&mutex); // lock
                pthread_cond_wait(&cond, &mutex); // wait for the condition

                //set_zero_cmd(0x141);
                //set_zero_cmd(0x142);
                //printf("ZERO");
                pthread_mutex_unlock(&mutex); // unlock
            }
        }
    }
    pthread_exit(NULL);
}


void close_all()
{
    printf("Quitting...");
    timer_delete(timerid);
    
    elbow_motor.shutdown_cmd();
    
    usleep(50000);

    elbow_motor.shutdown_cmd();

    if ((pthread_cancel(IMU_tid)) == 1)
    {
        fprintf(stderr, "IMU pthread_cancel error: %s\n");
        exit(-1);
    }
    if ((pthread_cancel(Ctl_tid)) == 1)
    {
        fprintf(stderr, "Ctl_tid pthread_cancel error: %s\n");
        exit(-1);
    }
    CAN_BUS_Send(send_buf, 0x142);
    usleep(10000);
    CAN_BUS_Close();
    usleep(10000);
    WT901_Close();
    usleep(10000);
    exit(0);
}


int main(int argc, char *argv[])
{
    int ret;
    struct sigevent evp;
    struct itimerspec it;

    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);
    
    
    tty_set_flag = tty_set();
    CAN_BUS_Init();
    usleep(500000);
    WT901_Init();
    usleep(500000);

    Motor elbow_motor(0x141);

    //printf("%X" , elbow_motor.get_can_address());

    /*creat a timer */
    ret = init_timer(&timerid, &evp, &it);
    if (ret < 0)
    {
        printf("init_timer failed\n");
        return -1;
    }
    printf("Timer set successfully!Timer_Id addr = %d\n", timerid);

    /* create the sub threads */

    // ret = pthread_create(&IMU_tid, NULL, IMU_read_thread, NULL);
    // if (ret)
    // {
    //     fprintf(stderr, "IMU_read_thread_create error: %s\n", strerror(ret));
    //     exit(-1);
    // }

    ret = pthread_create(&Ctl_tid, NULL, Control_thread, NULL);
    if (ret)
    {
        fprintf(stderr, "Control_thread_create error: %s\n", strerror(ret));
        exit(-1);
    }

    ret = pthread_create(&keyboard_event_tid, NULL, keyboard_event_thread, NULL);
    if (ret)
    {
        fprintf(stderr, "keyboard_event_tid_thread_create error: %s\n", strerror(ret));
        exit(-1);
    }

    start_timer(&timerid, &it);
    printf("The initial is done\r\n");

    // wait for the end of the sub threads
    // ret = pthread_join(IMU_tid, NULL);
    // if (ret)
    //     fprintf(stderr, "pthread_join IMU error: %s\n", strerror(ret));
    ret = pthread_join(Ctl_tid, NULL);
    if (ret)
        fprintf(stderr, "pthread_join Ctl error: %s\n", strerror(ret));
    ret = pthread_join(keyboard_event_tid, NULL);
    if (ret)
        fprintf(stderr, "pthread_join key error: %s\n", strerror(ret));

    exit(0);
}
