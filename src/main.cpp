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
    
    //printf("Joy#%d,V%d, \t JoyX#%d,V%f, \t JoyY#%d,V%f,\n",joy_number,joy_value,joy_axis_number,joy_x_value_norm,joy_axis_number,joy_y_value_norm);
    printf("\nX:%3.2f Y:%3.2f ",arm_x_pos,arm_y_pos);
    printf("E:%3.2f S:%3.2f ",elbow_pos,shoulder_pos);
    printf("Es:%3.2f Ss:%3.2f ",elbow_set_pos,shoulder_set_pos);
    printf("Es:%3.2f Ss:%3.2f ",joy_x_current,joy_y_current);
    kinematics.forward_kinematics(shoulder_pos, elbow_pos, arm_x_pos, arm_y_pos);
    //kinematics.inverse_kinematics( joy_x_current, joy_y_current, shoulder_pos, elbow_pos, shoulder_set_pos, elbow_set_pos);
    //kinematics.forward_kinematics(shoulder_pos, elbow_pos, arm_x_pos, arm_y_pos);
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
        //time_last = high_resolution_clock::now();
        pthread_mutex_lock(&mutex); // lock

        elbow_motor.read_position();
        //elbow_pos = elbow_motor.get_position();
        pthread_mutex_unlock(&mutex);
        pthread_cond_signal(&cond);

        //time_now = high_resolution_clock::now();
        //time_duration = time_now-time_last;
        usleep(can_delay);
        
        shoulder_motor.read_position();
        usleep(can_delay);
        elbow_pos = elbow_motor.get_position();
        usleep(can_delay);
        shoulder_pos = shoulder_motor.get_position();
        usleep(can_delay);
        //printf("POS2 - %f\n", motor2Pos);
        
        // elbow_motor.set_position(elbow_pos);
        // usleep(can_delay);
        // shoulder_motor.set_position(shoulder_pos);  
        // usleep(can_delay);
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
            if (key == 0x62)
            {
                if (flip == true)
                {
                    elbow_set_pos = 20;
                }
                else
                {
                    elbow_set_pos = 0.0;
                }
                
                flip = !flip;
                pthread_mutex_lock(&mutex); // lock
                pthread_cond_wait(&cond, &mutex); // wait for the condition
                
                elbow_motor.set_position(elbow_set_pos);
                usleep(can_delay);
                shoulder_motor.set_position(elbow_set_pos);

                pthread_mutex_unlock(&mutex); // unlock
            }
            if (key == 0x7A)
            {
                pthread_mutex_lock(&mutex); // lock
                pthread_cond_wait(&cond, &mutex); // wait for the condition

                elbow_motor.set_zero_cmd();
                usleep(can_delay);
                shoulder_motor.set_zero_cmd();
                printf("ZERO");
                pthread_mutex_unlock(&mutex); // unlock
            }
        }
    }
    pthread_exit(NULL);
}

void *joystick_thread(void* arg){
	while(1){
			joystick.sample(&joyEvent);
			if (joyEvent.isButton())
			{
				joy_value = joyEvent.value;
				joy_number = joyEvent.number;
			}
			else if (joyEvent.isAxis())
			{
				if (joyEvent.number == 0)
                {
                    joy_x_value = joyEvent.value*-1;
                    joy_axis_number = joyEvent.number;
                    joy_x_value_norm=joy_x_value/32767.0;
                    joy_x_current=joy_x_value_norm*0.63f;
                }
                if (joyEvent.number == 1)
                {
                    joy_y_value = joyEvent.value*-1;
                    joy_axis_number = joyEvent.number;
                    joy_y_value_norm=joy_y_value/32767.0;
                    joy_y_current=joy_y_value_norm*0.63f;
                }
                else 
                {
                    //joy_axis_value = joyEvent.value*-1;
                    //joy_axis_number = joyEvent.number;
                    //joy_axis_value_norm=joy_axis_value/32767.0;
                    //joy_current=joy_axis_value_norm;
                }
                
                
                
			}
            usleep(50000);
	}
	return NULL;
}

void close_all()
{
    printf("Quitting...");
    timer_delete(timerid);
    
    elbow_motor.shutdown_cmd();
    
    usleep(50000);

    shoulder_motor.shutdown_cmd();

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

bool motor_init()
{
    elbow_motor.read_position();
    usleep(can_delay);
    shoulder_motor.read_position();
    usleep(can_delay);
    elbow_pos = elbow_motor.get_position();
    usleep(can_delay);
    shoulder_pos = shoulder_motor.get_position();
    usleep(can_delay);
    fprintf(stderr,"currentPos= %3.2f, %3.2f",elbow_pos,shoulder_pos);
    
    elbow_motor.set_position(20.0);
    usleep(can_delay);
    shoulder_motor.set_position(20.0); 
    printf("moving") ;
    usleep(1000000000);
    printf("moved");
    return 1;
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
    

    if (!joystick.isFound())
	{
		printf("Joystick Not Found.\n");
		return -1;
	}
    else
    {
        printf("Joystick Connected\n");
    }
    
    

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

    // if ( motor_init() == -1 )
    // {
    //     exit(1);
    // }

    ret = pthread_create(&joy_tid, NULL, joystick_thread, NULL);
    if (ret)
    {
        fprintf(stderr, "Joystick_thread_create error: %s\n", strerror(ret));
        exit(-1);
    }


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
