#include <stdio.h>
#include <wiringPi.h>
#include <softPwm.h>
#include "move.h"
#include <mcp3004.h>
#include "sense.h"

#define INIVAL 0 // Valor inicial del PWM
#define RANGO 100 // Rango del PWM
extern float dista[2];
float left[300], right[300];
int i=0;
int main(){
	int x;
    char who; //0 sensor izq triggered y 1 sensor dcho triggered
	wiringPiSetup(); /* Inicializamos WiringPi */
	
	while(millis() < 3000){
		left[i] = leeIR(2);
		delay(10);
		right[i] = leeIR(3);
		printf("%.2f %.2f\n",left[i],right[i]);
		i++;
		delay(10);
	}
    
    FILE* f = fopen("log.txt","w");
    for(i=0; i< 300; i++){
		fprintf(f,"%.2f %.2f\n",left[i],right[i]);
	}
	fclose(f);
}
