rm main
g++ -c CAN_BUS_HCPS.c
g++ -c keyboard_event.c
g++ -c IMU_WT901.c
g++ -c motor.cpp
g++ -c joystick.cc
g++ -c kinematics.cpp
g++ -c main.cpp 
g++ -lpthread -lrt -lm motor.o kinematics.o joystick.o CAN_BUS_HCPS.o keyboard_event.o main.o IMU_WT901.o -o main
rm CAN_BUS_HCPS.o keyboard_event.o motor.o kinematics.o IMU_WT901.o joystick.o main.o 