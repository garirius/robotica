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

//Definimos nombres varios para los cerrojos
typedef enum {FRI=0, FRD, POSICION, NUM_CERROJOS} cerrojo;

//Variables globales para los hilos
int fri=0, frd=0; //franjas recorridas
char sti, std; //franja actual (0 = negro, 1 = blanco)
int girando=1, esquivando=0;
// sensen nos dice si los motores están en marcha
// girando nos dice si estamos yendo en línea recta (0) o girando hacia la derecha (-1) o hacia la izquierda (1)
// esquivando nos dice (sopresa sorpresa) si estamos esquivando un obstáculo
int cuentas=0;
float distot = 0;
extern float mypos[2], ori; //actuales posición y orientación
extern float dista[2]; //distancias detectadas por los sensores

void startCounting(){
	piLock(FRI);
	piLock(FRD);
	
    fri = 0;
    frd = 0;
    sti = leeSens(0);
    std = leeSens(1);
    cuentas = 0;
    distot = 0;
    delay(200);
    
    piUnlock(FRI);
	piUnlock(FRD);
}

//Para.
void stop(){
    softPwmWrite(PINL,0);
    softPwmWrite(PINR,0);
    piLock(POSICION);
    cuentas = 0;
    distot = 0;
    piUnlock(POSICION);
}

/* A continuación se escribe la función principal */
//Va hacia delante (1) o hacia atrás 0.
void palante(int dir){
	piLock(POSICION);
    girando = 0;
    startCounting();
    piUnlock(POSICION);
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
    int aux;
    if(cm > 0){
        palante(1);
    } else {
        palante(0);
        cm = -cm;
    }
    
    do{
        piLock(POSICION);
        aux = distot;
        piUnlock(POSICION);
        delay(5);
    }while((aux < cm)&&(cm >= 0));
    stop();
}

//angle < 0 -> gira a la izquierda
//angle > 0 -> gira a la derecha
void gira(int angle){
    int mchl, mchr, nt, aux;
    float auxn;
    
    piLock(POSICION);
    startCounting();
    auxn = PI*angle*INTER*FRANJAS/(360.0*PERI);
    nt = abs(nearbyintf(auxn)); //calculamos cuántas franjas nos hace falta girar
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
    }
    piUnlock(POSICION);
    
    softPwmWrite(PINL,mchl);
    softPwmWrite(PINR,mchr);
    do{
        piLock(POSICION);
        aux = cuentas;
        piUnlock(POSICION);
        delay(5);
    }while((aux < nt)&&(nt > 0));
    stop();
    printf("   Cuentas requeridas: %f -> %d | Cuentas contadas: %d\n",auxn,nt,aux);
    aux = angle*360*PERI*aux/(abs(angle)*PI*INTER*FRANJAS);
    printf("He girado %dº\n",aux);
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

//Convierte nuestros ángulos en -180 a 180
/* float rangulo(float angulo){
	int sgn;
	
	while(fabs(angulo) > 180){
		if(angulo > 0){
			sgn = 1;
		} else if(angulo < 0) {
			sgn = -1;
		}
		
		angulo = angulo - sgn * 360;
		printf("Estoy aquí dentro messing with your shit\n");
	}
	return angulo;
} */

/***************** POR AQUÍ EMPIEZAN LOS HILOS VARIOS ****************/

//CONTADOR DE FRANJAS (for real)
PI_THREAD(izq){
	piHiPri(10);
    while(1){
        //echamos el cerrojo
        piLock(FRI);
        int leci = leeSens(0);
        if(leci != sti){
            sti = leci;
            fri++;
        }
        piUnlock(FRI);
        delay(5);
    }
}

PI_THREAD(dcha){
	piHiPri(10);
    while(1){
        piLock(FRD);
        int lecd = leeSens(1);
        if(lecd != std){
            std = lecd;
            frd++;
        }
        piUnlock(FRD);
        delay(5);
    }   
}

//actualiza la posición
PI_THREAD(posi){
    int auxi, auxd, frm, aux;
    int cuanto=0;
    float di, dd, d, angulo, auxf;
    while(1){
        piLock(FRI);
        piLock(FRD);
        auxi = fri;
        auxd = frd;
        fri = 0;
        frd = 0;
        piUnlock(FRD);
        piUnlock(FRI);
        
        //en función de si estamos girando y tal calculamos una cosa u otra
        piLock(POSICION);
        switch(girando){
                case 0: //si estamos yendo en línea recta
                    //cálculo de las distancias recorridas en cada mini-tramo
                    di = PERI*auxi/FRANJAS;
                    dd = PERI*auxd/FRANJAS;
                    d = (di+dd)/2;
					printf("%.2f cm más, o sea que he recorrido ",d);
					
                    angulo = 180*(dd-di)/(PI*INTER); //medimos el ángulo girado
                    auxf = ori + angulo;
                    mypos[0] += d*cos(PI*auxf/180); //actualizamos posición x
                    mypos[1] += d*sin(PI*auxf/180); //actualizamos posición y
                    ori = auxf;
                
                    //Vamos acumulando la distancia recorrida
                    distot += d;
                    printf("%.2f cm\n",distot);
                    cuanto = 250;
                    break;
                case 1: //si está girando
                case -1:
                    if(auxi > auxd){ //miramos cuál es el máximo
                        frm = auxi;
                    } else {
                        frm = auxd;
                    }
                
                    auxf = girando*360*PERI*frm/(PI*INTER*FRANJAS);
                    ori += auxf;
                    
                    cuentas += frm;
                    cuanto = 50;
                    break;
        }
        piUnlock(POSICION);
        delay(cuanto);
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
    int x = piThreadCreate(izq);
    int y = piThreadCreate(dcha);
    int z = piThreadCreate(posi);
}
