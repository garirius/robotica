
#include <stdio.h>
#include <wiringPi.h>
#include <softPwm.h>

/* Definimos todas las especificaciones del robot */
#define INIVAL 0 /* Valor inicial del PWM */
#define RANGO 100 /* Rango del PWM */
#define PINR 0 /* Es el pin correspondiente a la rueda derecha */
#define PINL 1 /* Es el pin correspondiente a la rueda izquierda */

//Qué valores pasar al PWM y demás cosis.
#define FWL 20
#define FWR 5
#define BKL 10
#define BKR 15

//Vueltas/segundo de cada rueda
#define FWLV 0.860585
#define FWRV 0.8365
#define BKLV 0.9208
#define BKRV 0.8421

/* A continuación se escribe la función principal */
void gofw(){
    softPwmWrite(PINL,FWL);
    softPwmWrite(PINR,FWR);
}

void stop(){
    softPwmWrite(PINL,0);
    softPwmWrite(PINR,0);
}

int main(){
	wiringPiSetup(); /* Inicializamos WiringPi */
	int marcha;
	/* Ya procedemos a crear los PWM */
	softPwmCreate(PINL, INIVAL, RANGO);
	softPwmCreate(PINR, INIVAL, RANGO);
	
    while(1){
        printf("¡Pulsa ENTER para darle marcha a esto!\n");
        getchar();

        gofw();
        printf("¡Pulsa ENTER para parar esto!\n");
        getchar();
        stop();
    }
}
