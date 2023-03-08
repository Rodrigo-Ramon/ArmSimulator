#include <poll.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>

#define BAUDRATE B115200
#define MODEMDEVICE "/dev/ttyUSB0"

int Serial_file_descriptor;
struct pollfd Serial_file_descriptor_event[0];

int Serial_Init(void);
int Serial_detection(void);
float Serial_read(void);