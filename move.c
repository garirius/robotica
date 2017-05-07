#include <stdio.h>
#include <wiringPi.h>
#include <softPwm.h>
#include "sense.h"
#include <pthread.h>
#include <stdlib.h>

/* Definimos todas las especificaciones del robot */
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

//Variables globales para los hilos
int fri=0, frd=0; //franjas recorridas
char sti, std; //franja actual (0 = negro, 1 = blanco)
char sensen=0, correcting=0; //permite contar

void startCounting(){
    sensen = 1;
    fri = 0;
    frd = 0;
    sti = leeSens(0);
    std = leeSens(1);
}

//Para.
void stop(){
    softPwmWrite(PINL,0);
    softPwmWrite(PINR,0);
    sensen = 0;
}

/* A continuación se escribe la función principal */
//Va hacia delante (1) o hacia atrás 0.
void gofw(int dir){
    startCounting();
    
    if(dir == 1){
        softPwmWrite(PINL,FWL);
        softPwmWrite(PINR,FWR);
    } else {
        softPwmWrite(PINL,BKL);
        softPwmWrite(PINR,BKR);
    }
}

//Va hacia delante cm cm.
void advance(int cm){
    startCounting();
    
    if(cm > 0){
        softPwmWrite(PINL,FWL);
        softPwmWrite(PINR,FWR);
    } else {
        softPwmWrite(PINL,BKL);
        softPwmWrite(PINR,BKR);
        cm = -cm;
    }
    
    delay(cm*10/SPEED);
    stop();
}

//angle < 0 -> gira a la izquierda
//angle > 0 -> gira a la derecha
void gira(int angle){
    int mchl, mchr;
    startCounting();
    //En función de la dirección, asignamos una marcha a cada motor.
    if(angle > 0){
        mchl = BKL;
        mchr = FWR;
    } else if(angle < 0){
        mchl = FWL;
        mchr = BKR;
    } else {
        mchl = 0;
        mchr = 0;
    }
    
    short int time = 1000*angle/(SPIN*360);
    softPwmWrite(PINL,mchl);
    softPwmWrite(PINR,mchr);
    delay(time);
    stop();
}

PI_THREAD(stable){
    int leci, lecd;
    while(1){
        if(sensen){
            leci = leeSens(0);
            lecd = leeSens(1);
            
            if(leci != sti){
                sti = leci;
                fri++;
            }
            if(lecd != std){
                std = lecd;
                frd++;
            }
            
            if((fri%FRANJAS/2 == 0) || (frd%FRANJAS/2==0)){ //cada media vuelta, comprueba
                if(abs(fri-frd)>FRANJAS/4 && !correcting){
                    correcting = 1;
                    //Paramos uno de los motores.
                    if(fri > frd){
                        softPwmWrite(PINL,0);
                    } else {
                        softPwmWrite(PINR,0);
                    }
                }
            }
            
            if(correcting && frd == fri){
                softPwmWrite(PINL,FWL);
                softPwmWrite(PINR,FWR);
                correcting = 0;
            }
        }
        delay(10);
    }
}

void motoresSetup(){
    sti = leeSens(0);
    std = leeSens(1);
    int x = piThreadCreate(stable);
}