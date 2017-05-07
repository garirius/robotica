#include <stdio.h>
#include <wiringPi.h>
#include <softPwm.h>
#include "move.h"
#include <mcp3004.h>
#include "sense.h"

#define INIVAL 0 // Valor inicial del PWM
#define RANGO 100 // Rango del PWM
extern float dista[2];

int main(){
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
    
    //Avanzar 2m y girar 90 cm
    /*advance(50);
    printf("¡Aparcao!\n");
    delay(250);
    printf("¡Voy a girar...!\n");
    gira(-90);
    printf("¡Ya he girao!\n");*/
	/*gofw(1);
	while(dista[0] == -1 && dista[1] == -1){}
	stop(); //para al detectar algo
    delay(10);
    if(dista[0] == -1 && dista[1] != -1){
        who = 1;
    } else if(dista[0] != -1 && dista[1] == -1){
        who = 0;
    }
    gofw(1);
    while(dista[who] > 20){}
	delay(100);
	gira(90);
    delay(100);
    advance(40);
    delay(100);
    gira(-90);
    delay(100);
    advance(50);*/
    return 0;
}
