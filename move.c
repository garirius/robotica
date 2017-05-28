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
char sensen=0;
int girando=0, esquivando=0;
// sensen nos dice si los motores están en marcha
// girando nos dice si estamos yendo en línea recta (0) o girando hacia la derecha (-1) o hacia la izquierda (1)
// esquivando nos dice (sopresa sorpresa) si estamos esquivando un obstáculo
int di=0, dd=0, d=0, angulo=0;
int dt, nt; //distancia target, ángulo target y cuentas target
extern int mypos[2], ori; //actuales posición y orientación
extern float dista[2]; //distancias detectadas por los sensores

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
    delay(200);
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
    //printf("Quiero girar %dº y para ello necesito %d cuentas\n",angle,nt);
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
        sensen = 0;
    }
    
    softPwmWrite(PINL,mchl);
    softPwmWrite(PINR,mchr);
    while(sensen){}
    stop();
}

//Ejecuta una maniobra para esquivar obstáculos
void esquiva(){
    int sgn, i;
    float howfar = 0;
    int cuantos=0;
    int acerca;
    delay(1000);
    
    //calculamos a qué distancia nos encontramos del obstáculo
    if(dista[0]>-1){
		cuantos++;
		howfar += dista[0];
	}
    if(dista[1]>-1){
		cuantos++;
		howfar += dista[1];
	}
	
	howfar = howfar/(0.0+cuantos);
	printf("\nEstoy a %.2f cm del obstáculo.\n",howfar);
	acerca = howfar-20;
	printf("\nQuiero avanzar %d cm pa ser guay.\n",acerca);
    //...y simplemente nos vamos hasta estar a 20 cm de él.
    advance(acerca);
    delay(1000);
    
    //determinamos hacia qué dirección girar, en caso de no estar mirando el obstáculo de forma completamente perpendicular
    if((dista[0]<20)||(dista[1]<20)){
        if(dista[1] > 20){
            sgn = -1;
        } else {
            sgn = 1;
        }
    }
    
    //hacemos todas las maniobras pertinentes
    gira(sgn*90);
    delay(100);
    advance(40);
    delay(100);
    gira(-sgn*90);
    delay(100);
    advance(50);
    esquivando = 0;
    printf("Ya está esquivado\n");
}

//CONTADOR DE FRANJAS Y TAL
PI_THREAD(stable){
    int leci, lecd, frm, aux, obstaculo;
    while(1){
        delay(10);
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
            
            if((fri%(FRANJAS/12) == 0) || (frd%(FRANJAS/12)==0)){ //cada media vuelta, mide distancias
                switch(girando){
                    case 0: //si estamos yendo en línea recta
                        //cálculo de las distancias recorridas
                        di = PERI*fri/FRANJAS;
                        dd = PERI*frd/FRANJAS;
                        d = (di+dd)/2;
                        
                        if((dista[0]>-1)||(dista[1]>-1)){
                            //si detecta algún obstáculo, nos metemos en harina
                            if((esquivando==1) && ((dista[0]>20)||(dista[1]>20))){
                                obstaculo = 0;
                            } else {
                                //obstaculo = 1;
                                //printf("\n\n      ¡TENEMOS UN GANADOR!\n\n");
                            }
                        } else {
                            obstaculo = 0;
                        }
                        
                        if(((d >= dt)&&(dt>=0))||(obstaculo==1)){
                            sensen = 0;
                            angulo = 180*(dd-di)/(PI*INTER); //medimos el ángulo girado
                            aux = ori + angulo/2;
                            mypos[0] += d*cos(PI*aux/180); //actualizamos posición x
                            mypos[1] += d*sin(PI*aux/180); //actualizamos posición y
                            ori = (ori+angulo)%360;
                            printf("Ya paro, tranqui\n");
                        }
                        
                        if((obstaculo)&&(!esquivando)){
                            esquivando = 1;
                        }
                        break;
                    case 1: //si está girando
                    case -1:
                        if(fri > frd){ //miramos cuál es el máximo
                            frm = fri;
                        } else {
                            frm = frd;
                        }
                        if(frm > nt){
                            sensen = 0;
                            aux = girando*360*PERI*frm/(PI*INTER*FRANJAS);
                            printf("Yo estaba en %d y he girado %dº",ori,aux);
                            ori = (aux+ori)%360;
                            printf(", con lo que ahora estoy en %dº\n",ori);
//                          mypos[0] += INTER*(1-cos(aux*180/PI))/2;
//                          mypos[1] += INTER*sin(aux*180/PI)/2;

                        }
                        break;
                    default: //si algún imbécil viene a tocarnos las narices poniendo valores que no son
                        printf("¿Qué quieres tú, matao?\n");
                }
            }
        }
    }
}

//hilo para esquivar
PI_THREAD(dodge){
	while(1){
		if(esquivando){
			printf("Esquivando...\n");
			esquiva();
		}
	}
}

void motoresSetup(){
    sti = leeSens(0);
    std = leeSens(1);
    int x = piThreadCreate(stable);
    //int y = piThreadCreate(dodge);
}
