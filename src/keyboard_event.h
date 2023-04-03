/*
example in main()
int main()
{
        int tty_set_flag;
        tty_set_flag = tty_set();
        while(1) {

                if( kbhit() ) {
                        const int key = getchar();
                        printf("%c pressed\n", key);
                        if(key == 'q')
                                break;
                } else {
                        //fprintf(stderr, "<no key detected>\n");
                }
                
        }

        if(tty_set_flag == 0)
                tty_reset();
        return 0;
}
*/
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <termios.h>
#include <unistd.h>

int tty_reset(void);
int tty_set(void);
int kbhit(void);


