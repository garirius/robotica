#include <wiringPi.h>
#include <stdio.h>
#include <softPWM.h>

#define MOTSAL1 0
#define MOTSAL2 1

int main(){
    
        pinMode(MOTSAL,PWM_OUTPUT);
	pwmSetMode(PWM_MODE_MS);
	pwmSetRange(2000);	
	pwmSetClock(192);

    printf("¡Pulsa ENTER para arrancar motores!\n");
    getchar(); //básicamente, el programa para hasta que pulsamos enter
    
    //SE ENCIENDEN LOS MOTORES
	pwmWrite(MOTSAL,150);
    
    printf("\n ¡Pulsa ENTER para apagar los motores!\n");
    getchar();
    
    //SE APAGAN LOS MOTORES
	pwmWrite(MOTSAL,);
}
