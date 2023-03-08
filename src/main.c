/***********************************************************************
* Name        : main.cpp
* Date        : 02.27.2023
* Author      : Peihao Xiang(Noah)
* Version     : 2.3
* Copyright   : HCPS Lab ag 2015-2023
* Description : Torque Control
***********************************************************************/
#include "main.h"

int System_Init(void)
{
    if(Button_Init() == -1)
    {
        printf("Fail to Button_Init\n");
        return -1;
    }

    if(Serial_Init() == -1)
    {
        printf("Fail to Serial_Init\n");
        return -1;
    }

    if(CAN_BUS_Init() == -1)
    {
        printf("Fail to CAN_BUS_Init\n");
        return -1;
    }

    usleep(10000);

    if(Motor_Init() == -1)
    {
        printf("Fail to Motor_Init\n");
        return -1;
    }

    if(file_Init() == -1)
    {
        printf("Fail to file_Init");
        return -1;
    }

    if(Timer_Init(&timerid, &evp, &it) == -1)
    {
        printf("Fail to Tomer_Init\n");
        return -1;
    }

    return 1;
}

int Motor_Start(void)
{
    /* Motor start command */
    send_buffer[0] = 0xA4;
    send_buffer[1] = 0x00;
    send_buffer[2] = (u_int8_t)Max_Speed;
    send_buffer[3] = (u_int8_t)(Max_Speed >> 8);
    send_buffer[4] = (u_int8_t)Angle_Control;
    send_buffer[5] = (u_int8_t)(Angle_Control >> 8);
    send_buffer[6] = (u_int8_t)(Angle_Control >> 16);
    send_buffer[7] = (u_int8_t)(Angle_Control >> 24);

    if(CAN_BUS_Send(send_buffer) == -1)
    {
        perror("Motor homing send failed");
        return -1;
    }

    usleep(10000);

    received_buffer = CAN_BUS_Receive();

    if(received_buffer != NULL)
    {
        memmove(receive_buffer, received_buffer, 8);

        Current_Iq = (receive_buffer[3]<<8) + (receive_buffer[2]);
        Current_Speed = (receive_buffer[5]<<8) + (receive_buffer[4]);
        Current_Angle = (receive_buffer[7]<<8) + (receive_buffer[6]);

        printf("Original Angle:  %d degree\n",Current_Angle);
    }
    else
    {
        perror("Motor homing received failed");
        return -1;
    }

    sleep(5);

    /* Motor data command */
    send_buffer[0] = 0x9C;
    send_buffer[1] = 0x00;
    send_buffer[2] = 0x00;
    send_buffer[3] = 0x00;
    send_buffer[4] = 0x00;
    send_buffer[5] = 0x00;
    send_buffer[6] = 0x00;
    send_buffer[7] = 0x00;

    if(CAN_BUS_Send(send_buffer) == -1)
    {
        perror("Motor data send failed");
        return -1;
    }

    usleep(10000);

    received_buffer = CAN_BUS_Receive();

    if(received_buffer != NULL)
    {
        memmove(receive_buffer, received_buffer, 8);

        Current_Iq = (receive_buffer[3]<<8) + (receive_buffer[2]);
        Current_Speed = (receive_buffer[5]<<8) + (receive_buffer[4]);
        Current_Angle = (receive_buffer[7]<<8) + (receive_buffer[6]);

        printf("Current Iq: %.3f A\t", Current_Iq * 0.01);
        printf("Current Speed: %d dps\t", Current_Speed);
        printf("Current Angle:  %d degree\n", Current_Angle);
    }
    else
    {
        perror("Motor data received failed");
        return -1;
    }

    Iq_Control = Current_Iq;

    return 1;
}

void Timer_Calculate(union sigval v)
{
    float state = 0;
    float Sum_force = 0;
    float effort_force=0;
    float Current_force = 0;

    float radian = 0;
    float Gravity_torque = 0;

    float Reference_speed = 0;
    float Reference_torque = 0;
    float Reference_Iq = 0;

    /* Method 1 Torque Control, Convert the force value to the desired torque */
    radian = Current_Angle * (M_PI / 180);

    for(int i=0; i<15; i++)
    {
        Current_force = Serial_value[i] - (m_dst*(1 - sin(radian)));
        Sum_force = Sum_force + Current_force;
    }

    average_force[1] = Sum_force / 15;
    state = average_force[1] - average_force[0];
    if(state > -0.5 && state < 0.5)
    {
        effort_force = average_force[1];
        average_force[0] = effort_force;
    }

    if(state >= 0.5)
    {
        effort_force = average_force[0] - 0.3;
        average_force[0] = average_force[1];
    }

    if(state <= -0.5)
    {
        effort_force = average_force[0] + 0.3;
        average_force[0] = average_force[1];
    }

    printf("Average force: %.3f\t", effort_force);

    Reference_torque = effort_force * g * l;
    printf("Reference Torque: %.3f\t", Reference_torque);

    Gravity_torque = m_r * g * l * sin(radian);
    printf("Gravity Torque: %.3f\t", Gravity_torque);

    Reference_Iq = (Gravity_torque + Reference_torque) / kt;
    printf("Reference Iq: %.3f\t", Reference_Iq);

    Iq_Control = Reference_Iq * 100;
    printf("Iq Control: %d\t", Iq_Control);

    // int Speed_error = 0;

    // float Sum_force = 0;
    // float Current_force = 0;

    // float Reference_speed = 0;
    // float Reference_torque = 0;
    // float Reference_Iq = 0;

    /* Method 2 Admittance Control, Convert desired speed to desired torque */
    // for(int i=0; i<15; i++)
    // {
    //     Current_force = Serial_value[i] + m_h;
    //     Sum_force = Sum_force + (Current_force * dt);
    // }

    // if(abs(Sum_force) < 0.2)
    // {
    //     Sum_force = 0;
    // }

    // printf("Sum_force: %.3f\t", Sum_force);

    // Reference_speed = Sum_force / (mv*l*l);

    // printf("Reference Angular Velocity: %.3f rad/s\t", Reference_speed);

    // Speed_Control = (int)(Reference_speed * (180 / M_PI));

    // printf("Reference Speed: %d dps \t", Speed_Control);

    // if(abs(Speed_Control) > Max_Speed)
    // {
    //     if(Speed_Control < 0)
    //     {
    //         Speed_Control = -1 * Max_Speed;
    //     }
    //     else
    //     {
    //         Speed_Control = Max_Speed;
    //     }
    // }

    // Speed_error = Speed_Control - Current_Speed;

    // for(int i=0; i<15; i++)
    // {
    //     Sum_error = Sum_error + (Error_value[i] * dt);
    //     // printf("Error value: %d\n", Error_value[i]);
    // }

    // printf("Sum_error: %.3f\t", Sum_error);

    // Reference_torque = (kd * Speed_error);
    // printf("Refence Torque: %.3f A\t", Reference_torque);

    // Iq_Control = (int)(Reference_Iq + Iq_Control);
    // printf("Iq Control: %d A\t", Iq_Control);

    printf("Calculate successful!\n");
}

int Timer_Init(timer_t *timerid, struct sigevent *evp, struct itimerspec *it)
{
    if(!evp || !it)
    {
        return -1;
    }

    memset(evp, 0, sizeof(struct sigevent));

    evp->sigev_value.sival_int = 111;
    evp->sigev_notify = SIGEV_THREAD;
    evp->sigev_notify_function = Timer_Calculate;                         // timer function

    if(timer_create(CLOCK_REALTIME, evp, timerid) == -1)
    {
        perror("fail to timer_create");
        return -1;
    }

    printf("Timer create successfully!\n");

    it->it_interval.tv_sec = 0; 
    it->it_interval.tv_nsec = 300000000;                                  // set time loop, 300ms
    it->it_value.tv_sec = 1;                                              // start time, that means after power on 1s, then start timer
    it->it_value.tv_nsec = 0;

    return 1;
}

int Timer_start(timer_t *timerid, struct itimerspec *it)
{
    if(it == NULL)
    {
        return -1;
    }

    if(timer_settime(*timerid, TIMER_ABSTIME, it, NULL) == -1)
    {
        perror("fail to timer setting");
        return -1;
    }

    return 1;
}

static void *force_read_thread(void *arg)
{
    int i = 0;
    int n = 0;
    float x;
    float value;
    float difference_value[2] = {0};

    while(1)
    {
        difference_value[1] = Serial_read();

        x = difference_value[1] - difference_value[0];

        if(abs(x) < 1)
        {
            value = difference_value[1];
            difference_value[0] = difference_value[1];
        }

        if(n < 15)
        {
            Serial_value[n] = value;
            n++;

            if(n == 15)
            {
                n = 0;
            }
        }

        fprintf(file_pointer, "%d  %.3f \t", i, Serial_value[n]);
        i++;

        usleep(20000);
    }

    pthread_exit(NULL);
}

static void *Motor_control_thread(void *arg)
{
    int i = 0;

    while(1)
    {
        if(abs(Iq_Control) > Max_Iq)
        {
            Iq_Control = Max_Iq;
        }

        send_buffer[0] = 0xA1;
        send_buffer[1] = 0x00;
        send_buffer[2] = 0x00;
        send_buffer[3] = 0x00;
        send_buffer[4] = (u_int8_t)Iq_Control;
        send_buffer[5] = (u_int8_t)(Iq_Control >> 8);
        send_buffer[6] = 0x00;
        send_buffer[7] = 0x00;

        if(CAN_BUS_Send(send_buffer,0) == -1)
        {
            perror("Motor Control send failed");
        }

        usleep(10000);

        received_buffer = CAN_BUS_Receive();

        if(received_buffer != NULL)
        {
            memmove(receive_buffer, received_buffer, 8);

            Current_Iq = ((receive_buffer[3] << 8) + (receive_buffer[2]));
            Current_Speed = (receive_buffer[5] << 8) + (receive_buffer[4]);
            Current_Angle = (receive_buffer[7] << 8) + (receive_buffer[6]);

            fprintf(file_pointer, "%d  %.3f  %d  %d\n", i, Current_Iq * 0.01, Current_Speed, Current_Angle);
            i++;
        }
        else
        {
            perror("Motor Control received failed");
        }

        usleep(10000);
    }

    pthread_exit(NULL);
}

static void *botton_event_thread(void *arg)
{
    while(1)
    {
        if(Button_detection())
        {
            const int key = getchar();

            if(key == '1')
            {
                Power_down();
                exit(0);
                break;
            }
        }
    }

    pthread_exit(NULL);
}

void Power_down()
{
    if(timer_delete(timerid) == -1)
    {
        perror("Timer delete failed");
        exit(-1);
    }

    if(pthread_cancel(force_read_tid) != 0)
    {
        perror("force read thread cancel error");
        exit(-1);
    }

    if(pthread_cancel(motor_control_tid) != 0)
    {
        perror("motor control thread cancel error");
        exit(-1);
    }

    /* Close Motor and CAN BUS */
    send_buffer[0] = 0x80;
    send_buffer[1] = 0x00;
    send_buffer[2] = 0x00;
    send_buffer[3] = 0x00;
    send_buffer[4] = 0x00;
    send_buffer[5] = 0x00;
    send_buffer[6] = 0x00;
    send_buffer[7] = 0x00;

    if(CAN_BUS_Send(send_buffer,0) == -1)
    {
        perror("Send stop command failed");
        printf("Please turn off the power");
    }
    
    usleep(10000);

    if(CAN_BUS_Close() == -1)
    {
        perror("CAN BUS close failed");
    }

    if(close(Serial_file_descriptor) != 0)
    {
        perror("Serial close failed");
    }

    if(fclose(file_pointer) != 0)
    {
        perror("File close failed");
    }

    printf("System close is successful\n");
}

int main(void)
{
    if(System_Init() == -1)
    {
        printf("Fail to System_Init\n");
        Power_down();
        exit(-1);

        return -1;
    }
    else
    {
        printf("System initialization is successful!\n");
    }

    usleep(10000);

    if(Motor_Start() == -1)
    {
        printf("Motor start failed\n");
        Power_down();
        exit(-1);

        return -1;
    }

    usleep(10000);

    /* Create the sub-threads */
    if(pthread_create(&force_read_tid, NULL, force_read_thread, NULL) != 0)
    {
        perror("force read thread create error");
        exit(-1);
    }

    if(pthread_create(&motor_control_tid, NULL, Motor_control_thread, NULL) != 0)
    {
        perror("motor control thread create error");
        exit(-1);
    }

    if(pthread_create(&botton_event_tid, NULL, botton_event_thread, NULL) != 0)
    {
        perror("botton event thread create error");
        exit(-1);
    }

    sleep(1);

    /* Calculate arm weight */
    for(int i = 0; i < 15; i++)
    {
        m_dst = m_dst + Serial_value[i];

        // usleep(20000);
    }

    // fflush(file_pointer);

    m_dst = m_dst / 15;

    printf("External Pressure: %.2f kG\n", m_dst);

    if(Timer_start(&timerid, &it) == -1)
    {
        perror("Timer start failed");
        exit(-1);
    }

    printf("-----------------Motor Start-----------------\n");

    while(1)
    {   
        if(Current_Angle > Max_Angle || Current_Angle < Min_Angle)
        {
            Power_down();
            exit(-1);
            break;
        }
    }

    /* Wait for the end of the sub-threads */
    if(pthread_join(force_read_tid, NULL) != 0)
    {
        perror("force read join error");
        exit(-1);
    }

    if(pthread_join(motor_control_tid, NULL) != 0)
    {
        perror("force read join error");
        exit(-1);
    }

    if(pthread_join(botton_event_tid, NULL) != 0)
    {
        perror("botton event join error");
        exit(-1);
    }

    return 0;
}