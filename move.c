#include <stdio.h>
#include <wiringPi.h>
#include <softPwm.h>
#include "sense.h"
#include <pthread.h>
#include <stdlib.h>
#include <math.h>

/* Definimos todas las especificaciones del robot */
#define PINR 0 // Pin para la rueda derecha
#define PINL 1 // Pin para la rueda izquierda
#define PERI 22 //Perímetro (en cm) de las ruedas.
#define INTER 20.8 //Distancia (en cm) entre las ruedas

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

//Definimos pi porque por algún motivo math.h no lo tiene
#define PI 3.141592

//Variables globales para los hilos
int fri=0, frd=0; //franjas recorridas
char sti, std; //franja actual (0 = negro, 1 = blanco)
char sensen=0, girando=0;
// sensen nos dice si los motores están en marcha
// girando nos dice si estamos yendo en línea recta (0) o girando hacia la derecha (-1) o hacia la izquierda (1)
int di=0, dd=0, d=0, angulo=0;
int dt, nt; //distancia target, ángulo target y cuentas target
extern int mypos[2], ori; //actuales posición y orientación



void startCounting(){
    sensen = 1;
    fri = 0;
    frd = 0;
    sti = leeSens(0);
    std = leeSens(1);
    di = 0;
    dd = 0;
    d = 0;
    angulo = 0;
}

//Para.
void stop(){
    softPwmWrite(PINL,0);
    softPwmWrite(PINR,0);
    sensen = 0;
}

/* A continuación se escribe la función principal */
//Va hacia delante (1) o hacia atrás 0.
void palante(int dir){
    girando = 0;
    startCounting();
    
    if(dir == 1){
        softPwmWrite(PINL,FWL);
        softPwmWrite(PINR,FWR);
    } else {
        softPwmWrite(PINL,BKL);
        softPwmWrite(PINR,BKR);
    }
}

void gofw(int dir){
    dt = -1;
    palante(dir);
}

//Va hacia delante cm cm.
void advance(int cm){
    startCounting();
    
    if(cm > 0){
        palante(1);
    } else {
        palante(0);
        cm = -cm;
    }
    
    dt = cm;
    while(sensen){}
	
    stop();
}

//angle < 0 -> gira a la izquierda
//angle > 0 -> gira a la derecha
void gira(int angle){
    int mchl, mchr;
    startCounting();
    
    nt = abs(PI*angle*INTER*FRANJAS/(360*PERI)); //calculamos cuántas franjas nos hace falta girar
    printf("Quiero girar %dº y para ello necesito %d vueltas\n",angle,nt);
    //En función de la dirección, asignamos una marcha a cada motor.
    if(angle > 0){
        mchl = BKL;
        mchr = FWR;
        girando = 1;
    } else if(angle < 0){
        mchl = FWL;
        mchr = BKR;
        girando = -1;
    } else {
        mchl = 0;
        mchr = 0;
        girando = 2;
    }
    
    softPwmWrite(PINL,mchl);
    softPwmWrite(PINR,mchr);
    while(sensen){}
    stop();
}

//CONTADOR DE FRANJAS Y TAL
PI_THREAD(stable){
    int leci, lecd, frm;
    while(1){
        delay(25);
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
            
            if((fri%(FRANJAS/4) == 0) || (frd%(FRANJAS/4)==0)){ //cada media vuelta, mide distancias
                switch(girando){
                    case 0: //si estamos yendo en línea recta
                        //cálculo de las distancias recorridas
                        di = PERI*fri/FRANJAS;
                        dd = PERI*frd/FRANJAS;
                        d = (di+dd)/2;
                        if((d >= dt)&&(dt>=0)){
                            sensen = 0;
                            angulo = 180*(dd-di)/(PI*INTER); //medimos el ángulo girado
                            ori += angulo;
                            mypos[0] += cos(PI*ori/180); //actualizamos posición x
                            mypos[1] += sin(PI*ori/180); //actualizamos posición y
                        }
                        break;
                    case 1: //si está girando
                    case (char)-1:
                        if(fri > frd){ //miramos cuál es el máximo
                            frm = fri;
                        } else {
                            frm = frd;
                        }
                        if(frm > nt){
                            sensen = 0;
                            ori += girando*PERI*PI*frm/(90*INTER*FRANJAS);
                            
                        }
                        break;
                    default: //si algún imbécil viene a tocarnos las narices poniendo valores que no son
                        printf("¿Qué quieres tú, matao?\n");
                }
            }
        }
    }
}

void motoresSetup(){
    sti = leeSens(0);
    std = leeSens(1);
    int x = piThreadCreate(stable);
}
