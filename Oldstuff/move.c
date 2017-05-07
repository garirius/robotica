#include <wiringPi.h>
#include <softPwm.h>

/* Definimos todas las especificaciones del robot */
#define INIVAL 0 // Valor inicial del PWM
#define RANGO 100 // Rango del PWM
#define PINR 0 // Pin para la rueda derecha
#define PINL 1 // Pin para la rueda izquierda
#define PERIIZQ 0.22 // Perímetro (en m) de las ruedas.

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

//Para.
void stop(){
    softPwmWrite(PINL,0);
    softPwmWrite(PINR,0);
}

//Va hacia delante N metros. N<0 -> hacia atrás.
void go(float metros){
    unsigned int tim=0; //tiempo que tardaremos (grosso modo) en recorrer los metros
    int left=0, right=0; //marchas de las ruedas
    
    if(metros > 0){
        left = FWL;
        right = FWR;
    } else {
        left = BKL;
        right = BKR;
        metros = -metros;
    }
    tim = metros*1000/SPEED;
    
    if(metros!=0){
        softPwmWrite(PINL,left);
        softPwmWrite(PINR,right);
        delay(tim);
        stop();
    }
}

//Gira un cierto ángulo
void turn(float ang){
    int mchl=0, mchr=0; //marchas izquierda y derecha
    unsigned int tim=0;
    
    //ang > 0 hacia la derecha, ang < 0 hacia la izquierda
    if(ang>0){
        mchl = FWL;
        mchr = BKR;
    } else {
        mchl = BKL;
        mchr = FWR;
    }
    
    tim = ang*1000/(360*SPIN); //luego meteremos el encoder y tal
    
    if(ang!=0){
        softPwmWrite(PINL,mchl);
        softPwmWrite(PINR,mchr);
        delay(tim);
        stop();
    }
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