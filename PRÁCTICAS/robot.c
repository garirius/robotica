#include <stdio.h>
#include <wiringPi.h>
#include <softPwm.h>


/* Definimos todas las especificaciones del robot */
#define DISTANCIA 0.5 /* Esa la distancia que andará el robot antes de girar */
#define PERIIZQ 0.22 /* Es el diametro de la rueda izquierda */
#define PERIDER 0.22 /* Es el diametro de la rueda derecha */
#define VPSIZQ 0.79 /* Son las vueltas por segundo de la rueda izquierda (en radianes)*/
#define VPSDER 0.79 /* Son las vueltas por segundo de la rueda derecha (en radianes)*/
#define DISTANCIARUEDAS 0.16 /* Definimos la distancia entre las ruedas */
#define ANGULOGIRO 1.6 /* Definimos el ángulo (en radianes) que queremos que gire el robot */
#define INIVAL 0 /* Valor inicial del PWM */
#define RANGO 100 /* Rango del PWM */
#define PIN0 0 /* Es el pin correspondiente a la rueda derecha */
#define PIN1 1 /* Es el pin correspondiente a la rueda izquierda */
#define MARCHA0 14 /* Es el valor del PWM para que ande la rueda derecha (pin 0) */
#define MARCHA1 10 /* Es el valor del PWM para que ande la rueda izquierda (pin 1) */


/* Se declaran las variables globales del código */

int tiempoizquierda1; /* Es quien marca el tiempo de comienzo cuando la rueda izquierda empieza a girar */
int tiempoizquierda2; /*Es quien llevará el contador de tiempo del giro de la rueda izquierda */
int tiempoderecha1; /* Es quien marca el tiempo de comienzo cuando la rueda derecha empieza a girar */
int tiempoderecha2; /*Es quien llevará el contador de tiempo del giro de la rueda derecha */


/* Se escribe la función correspondiente al hilo encargado de controlar el tiempo de giro de la rueda izquierda */

PI_THREAD(izquierda){
	while(1){
		tiempoizquierda2=millis();
	}
}


/* Se escribe la función correspondiente al hilo encargado de controlar el tiempo de giro de la rueda derecha */

PI_THREAD(derecha){
	while(1){
		tiempoderecha2=millis();
	}
}


/* A continuación se escribe la función principal */
int main(){
	wiringPiSetup(); /* Inicializamos WiringPi */
	
	
	/* Empezamos realizando los calculos necesarios para el movimiento */
	float tiempototalizquierda;
	float tiempototalderecha;
	int IZQ;
	int DER;
	if (ANGULOGIRO>=0){
		IZQ=1;
		DER=0;
	}
	if (ANGULOGIRO<0){
		IZQ=0;
		DER=-1;
	}
	tiempototalizquierda=((DISTANCIA/(PERIIZQ*VPSIZQ))+IZQ*((ANGULOGIRO*DISTANCIARUEDAS)/(PERIIZQ*VPSIZQ)))*1000;
	tiempototalderecha=((DISTANCIA/(PERIDER*VPSDER))+DER*((ANGULOGIRO*DISTANCIARUEDAS)/(PERIDER*VPSDER)))*1000;
	/* Ya procedemos a crear los PWM */
	softPwmCreate(PIN0, INIVAL, RANGO);
	softPwmCreate(PIN1, INIVAL, RANGO);
	int valorizquierda; /* Valor del PWM izquierdo */
	int valorderecha; /* Valor del PWM derecho */
	/*Primero lo dejamos parado 3 segundos*/
	softPwmWrite(PIN0,0);
	softPwmWrite(PIN1,0);
	delay(3000);
	/* Ahora creamos los hilos */
	int x; /* Es para los hilos */
	tiempoizquierda1=millis();
	tiempoderecha1=millis();
	x=piThreadCreate(izquierda);
	x=piThreadCreate(derecha);
	while(1){
		/*Ahora hacemos que el robot se mueva tal y como se establece en la parte de los "#define" */
		if ((tiempoderecha2-tiempoderecha1)<(tiempototalderecha)){
			valorderecha=MARCHA0;
		}
		if ((tiempoderecha2-tiempoderecha1)>=(tiempototalderecha)){
			valorderecha=0;
		}
		if ((tiempoizquierda2-tiempoizquierda1)<(tiempototalizquierda)){
			valorizquierda=MARCHA1;
		}
		if ((tiempoizquierda2-tiempoizquierda1)>=(tiempototalizquierda)){
			valorizquierda=0;
		}
		softPwmWrite(PIN0,valorderecha);
		softPwmWrite(PIN1,valorizquierda);
		delay(10);
	}
	return 0;
}
