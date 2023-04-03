rm main
g++ -c CAN_BUS_HCPS.c
g++ -c keyboard_event.c
g++ -c IMU_WT901.c
g++ -c motor.cpp
g++ -c main.cpp 
g++ -lpthread -lrt -lm motor.o CAN_BUS_HCPS.o keyboard_event.o main.o IMU_WT901.o -o main
rm CAN_BUS_HCPS.o keyboard_event.o motor.o IMU_WT901.o main.o