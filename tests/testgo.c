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
#define BKR 25

//Vueltas/segundo de cada rueda
#define FWLV 0.860585
#define FWRV 0.8365
#define BKLV 0.9208
#define BKRV 0.8421

//Vueltas/segundo del giro global
#define SPIN 0.326

/* A continuación se escribe la función principal */
//Va hacia delante.
void gofw(){
    softPwmWrite(PINL,FWL);
    softPwmWrite(PINR,FWR);
}

//Para.
void stop(){
    softPwmWrite(PINL,0);
    softPwmWrite(PINR,0);
}

//Se pone a girar en una dirección (IZQ ó DCH)
void turn(int tow){
    int mchl, mchr;
    
    //En función de la dirección, asignamos una marcha a cada motor.
    switch(tow){
        case 0:
            mchl = BKL;
            mchr = FWR;
            break;
        case 1:
            mchl = FWL;
            mchr = BKR;
            break;
        default:
            mchl = 0;
            mchr = 0;
    }
    
    softPwmWrite(PINL,mchl);
    softPwmWrite(PINR,mchr);
    delay(250/SPIN);
    stop();
    
}

int main(){
	wiringPiSetup(); /* Inicializamos WiringPi */
	int marcha;
	/* Ya procedemos a crear los PWM */
	softPwmCreate(PINL, INIVAL, RANGO);
	softPwmCreate(PINR, INIVAL, RANGO);
	
    while(1){
        printf("0 = IZQ         1=DCH\n");
        scanf("%d",&marcha);
        getchar();
        turn(marcha);
        printf("¡Pulsa ENTER para parar esto!\n");
        getchar();
        stop();
    }
}
