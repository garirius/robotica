#include <stdio.h>
#include <wiringPi.h>
#include <softPwm.h>

/* Definimos todas las especificaciones del robot */
#define INIVAL 0 /* Valor inicial del PWM */
#define RANGO 100 /* Rango del PWM */
#define PIN0 0 /* Es el pin correspondiente a la rueda derecha */
#define PIN1 1 /* Es el pin correspondiente a la rueda izquierda */
#define MARCHA0 15 /* Es el valor del PWM para que ande la rueda derecha (pin 0) */
#define MARCHA1 15 /* Es el valor del PWM para que ande la rueda izquierda (pin 1) */

/* A continuación se escribe la función principal */
int main(){
	wiringPiSetup(); /* Inicializamos WiringPi */
	int marcha;
	/* Ya procedemos a crear los PWM */
	softPwmCreate(PIN0, INIVAL, RANGO);
	softPwmCreate(PIN1, INIVAL, RANGO);
	
while(1){
	printf("\nDime un número así wapo.\n");
	scanf("%d",&marcha);

	softPwmWrite(PIN0, marcha);
	softPwmWrite(PIN1,marcha);
}
}
