#include <math.h>
#include "move.h"
#include "sense.h"
#include <stdio.h>

#define TOOFAR 10
#define TORCIDO 10
//Nuestras variables globales de posición y orientación
extern int mypos[2], ori;

//Comprueba si hemos llegado a nuestro destino.
int thereYetAng(int* post,int orit){
    int deltax = post[0]-mypos[0], deltay = post[1]-mypos[1];
    float sepa = sqrt(deltax*deltax+deltay*deltay);
    int deltatheta = (orit-ori)%360;
    
    if((sepa > TOOFAR)||(deltatheta > TORCIDO)){
        return 0;
    } else {
        return 1;
    }
}

void iraAng(int* post,int orit){
    //calculamos las diferencias de cosas que necesitamos
    int deltax = post[0]-mypos[0];
    int deltay = post[1]-mypos[1];
    int sgn = 0;
    double deltatheta = atan2(deltay,deltax);
    printf("\n    DECLARACIÓN DE INTENCIONES:\n    Estoy en (%d,%d)%dº.\n    Ahora quiero estar en (%d,%d)%dº\n\n",mypos[0],mypos[1],ori,post[0],post[1],orit);
    //Calculamos el ángulo que hay que girar para ir en línea recta.
    int ang = deltatheta*180/PI - ori;
    
    while(fabs(ang) > 180){
		if(ang > 0){
			sgn = 1;
		} else if(ang < 0) {
			sgn = -1;
		}
		
		ang = ang - sgn * 360;
	}
    //Giramos ese ángulo
    printf("Girando %dº...\n",ang);
    gira(ang);
    delay(100);
    
    //Calculamos la distancia que necesitamos recorrer en línea recta.
    int howlong = sqrt(deltax*deltax+deltay*deltay);
    printf("Avanzando %d cm...\n",howlong);
    advance(howlong);
    delay(100);
    
    //Finalmente, nos quedamos mirando en la orientación que nos piden.
    ang = orit - ori;
    if(fabs(ang) > 180){
		if(ang > 0){
			sgn = 1;
		} else if(ang < 0) {
			sgn = -1;
		}
		
		ang = ang - sgn * 360;
	}
    printf("Girando %dº...\n",ang);
    gira(ang);
    delay(100);
    printf("Vale, ahora estoy en (%d,%d),%dº\n",mypos[0],mypos[1],ori);
}

//Comprueba si hemos llegado a nuestro destino.
int thereYet(int* post){
    int deltax = post[0]-mypos[0], deltay = post[1]-mypos[1];
    float sepa = sqrt(deltax*deltax+deltay*deltay);
    
    if(sepa > TOOFAR){
        return 0;
    } else {
        return 1;
    }
}

void ira(int* post){
    //calculamos las diferencias de cosas que necesitamos
    int deltax = post[0]-mypos[0];
    int deltay = post[1]-mypos[1];
    int sgn = 0;
    double deltatheta = atan2(deltay,deltax);
    printf("\n    DECLARACIÓN DE INTENCIONES:\n    Estoy en (%d,%d)%dº.\n    Ahora quiero estar en (%d,%d)\n\n",mypos[0],mypos[1],ori,post[0],post[1]);
    //Calculamos el ángulo que hay que girar para ir en línea recta.
    int ang = deltatheta*180/PI - ori;
    
    while(fabs(ang) > 180){
		if(ang > 0){
			sgn = 1;
		} else if(ang < 0) {
			sgn = -1;
		}
		
		ang = ang - sgn * 360;
	}
    //Giramos ese ángulo
    printf("Girando %dº...\n",ang);
    gira(ang);
    delay(500);
    
    //Calculamos la distancia que necesitamos recorrer en línea recta.
    int howlong = sqrt(deltax*deltax+deltay*deltay);
    printf("Avanzando %d cm...\n",howlong);
    advance(howlong);
    delay(500);
}

//Sigue un camino definido por path
void follow(int** path, int len){
    int i=0, corigo;
    for(i=0; i<len; i++){
        corigo = 0;
        
        while(!corigo){
            if(!esquivando){
                ira(path[i]);
                corigo = thereYet(path[i]);
            }
        }
    }
}
