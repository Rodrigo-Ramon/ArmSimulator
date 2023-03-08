#include "Data.h"
#include "Motor.h"
#include "Serial.h"
#include "botton_key.h"
#include "CAN_BUS_HCPS.h"

float l = 0.3;                     // Arm length: 0.3m
float g = 9.8;                     // Gravity, Unit is m/s*s
float dt = 0.02;                   // Sample time between force value, Unit is second

float mv = 0.2;                    // Virtual inertia, Unit is kg

float m_l = 0.5;                   // Load weight, Unit is kg
float m_dst = 0;                   // External Pressure

float m_r = 0.9;                   // Robotic arm weight(include two sensors), Unit is kg
float b_r = 6;                     // Robotic arm damping, Unit is Ns/m

float m_h = 0;                     // Human arm weight, Unit is kg
float b_h = 5;                     // Human are damping, Unit is Ns/m
float k_h = 0.2;                   // Huaman are spring, Unit is N/m

float kd = 1;                      // Speed deviation coefficient
float kt = 2.0;                    // Torque coefficient, converted to desired current

char send_buffer[8] = {0};
char receive_buffer[8] = {0};
char *received_buffer = NULL;

float average_force[2] = {0};
float Serial_value[15] = {0};     // Raw force value

int16_t Max_Speed = 30;           // Maximum speed, Unit is dps(degree per second)
int16_t Max_Angle = 150;          // Maximum Angle is 150 degree
int16_t Max_Iq = 800;             // Maximum current is 8A

int16_t Min_Angle = 0;            // Minimum Angle is 0 degree

/* Control Input */
int16_t Iq_Control = 0;
int32_t Angle_Control = 0;
int32_t Speed_Control = 0;

/* Control Output */
int16_t Current_Iq = 0;
int16_t Current_Speed = 0;
int16_t Current_Angle = 0;

pthread_t force_read_tid;
pthread_t motor_control_tid;
pthread_t botton_event_tid;

timer_t timerid = 0;
struct sigevent evp;
struct itimerspec it;

int System_Init(void);
int Motor_Start(void);
void Timer_Calculate(union sigval v);
int Timer_Init(timer_t *timerid, struct sigevent *evp, struct itimerspec *it);
int Timer_start(timer_t *timerid, struct itimerspec *it);
static void *force_read_thread(void *arg);
static void *Motor_control_thread(void *arg);
static void *botton_event_thread(void *arg);
void Power_down();
int main(void);