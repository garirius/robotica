#include <stdio.h>
#include <wiringPi.h>
#include <softPwm.h>

/* Definimos todas las especificaciones del robot */
#define INIVAL 0 // Valor inicial del PWM
#define RANGO 100 // Rango del PWM
#define PINR 0 // Pin para la rueda derecha
#define PINL 1 // Pin para la rueda izquierda
#define PERIIZQ 0.22 //Perímetro (en m) de las ruedas.

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
#define SPIN 0.326 //rev/s
//Velocidad (grosso modo) de avance del robot.
#define SPEED 0.174 //m/s
//s/vuelta pivotando
#define PIVL 7.52
#define PIVR 7.2

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

//Pivota un cierto ángulo
void pivot(float ang){
    int mchl=0, mchr=0; //marchas izquierda y derecha
    unsigned int tim=0;
    float vel=0;
    
    //ang > 0 hacia la derecha, ang < 0 hacia la izquierda
    if(ang>0){
        mchl = FWL;
        vel = PIVR;
    } else {
        mchr = FWR;
        vel = PIVL;
    }
    
    tim = ang*1000*vel*360; //luego meteremos el encoder y tal
    
    if(ang!=0){
        softPwmWrite(PINL,mchl);
        softPwmWrite(PINR,mchr);
        delay(tim);
        stop();
    }
}

int main(){
	wiringPiSetup(); /* Inicializamos WiringPi */
	
	/* Ya procedemos a crear los PWM */
	softPwmCreate(PINR, INIVAL, RANGO);
	softPwmCreate(PINL, INIVAL, RANGO);
	int izq=0, dch=0; /* Marchas PWM */
	stop();
	printf("Pulsa ENTER para ser guay.\n");
	getchar();
	gofw();
	delay(2000/SPEED);
	stop();
	delay(100);
	turn(1);
	return 0;
}
