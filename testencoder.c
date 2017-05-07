#include <stdio.h>
#include <wiringPi.h>
#include <softPwm.h>
#include "move.h"
#include <mcp3004.h>
#include "sense.h"

#define INIVAL 0 // Valor inicial del PWM
#define RANGO 100 // Rango del PWM
extern float dista[2];
short left[300], right[300];
int i=0;
char yas = 1;
extern int fri, frd;

PI_THREAD(mideme){
	while(yas){
		right[i] = leeSens(1);
		left[i] = leeSens(0);
		i++;
		delay(10);
	}
}
int main(){
	int x;
    char who; //0 sensor izq triggered y 1 sensor dcho triggered
	wiringPiSetup(); /* Inicializamos WiringPi */
	mcp3004Setup(100,0); //Inicializamos las cosillas del ADC
    motoresSetup();
    sensoresSetup();
    
	/* Ya procedemos a crear los PWM */
	softPwmCreate(PINR, INIVAL, RANGO);
	softPwmCreate(PINL, INIVAL, RANGO);
	int izq=0, dch=0; /* Marchas PWM */
	stop();
    
	printf("Pulsa ENTER para ser guay.\n");
	getchar();
    x = piThreadCreate(mideme);
    gofw(1);
    delay(3000);
    stop();
    yas = 0;
	printf("izq: %d cuentas | cha: %d cuentas\n",fri,frd);
    
    FILE* f = fopen("log.txt","w");
    for(i=0; i< 300; i++){
		fprintf(f,"%d %d\n",left[i],right[i]);
	}
}
