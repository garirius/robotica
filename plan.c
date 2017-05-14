#include <math.h>
#include "move.h"
#include <stdio.h>

//Nuestras variables globales de posición y orientación
extern int mypos[2], ori;

void ira(int* post,int orit){
    //calculamos las diferencias de cosas que necesitamos
    int deltax = post[0]-mypos[0];
    int deltay = post[1]-mypos[1];
    int sgn = 0;
    double deltatheta = atan2(deltay,deltax);
    printf("\n\n DECLARACIÓN DE INTENCIONES:\n Estoy en (%d,%d)%dº.\n Ahora quiero estar en (%d,%d)%dº\n\n",mypos[0],mypos[1],ori,post[0],post[1],orit);
    //Calculamos el ángulo que hay que girar para ir en línea recta.
    int ang = deltatheta*180/PI - ori;
    
    printf("QUIERO GIRAR %dº\n",ang);
    while(fabs(ang) > 180){
		if(ang > 0){
			sgn = 1;
		} else if(ang < 0) {
			sgn = -1;
		}
		
		ang = ang - sgn * 360;
	}
	printf("YO QUIERO GIRAR %dº\n",ang);
    //Giramos ese ángulo
    gira(ang);
    delay(100);
    printf("HE GIRADO YA :D\nVale, ahora estoy en (%d,%d),%dº\n",mypos[0],mypos[1],ori);
    
    //Calculamos la distancia que necesitamos recorrer en línea recta.
    printf("\nTengo ahora que desplazarme (%d,%d) para llegar a (%d,%d)\n",deltax,deltay,post[0],post[1]);
    int howlong = sqrt(deltax*deltax+deltay*deltay);
    advance(howlong);
    delay(100);
    printf("Vale, ahora estoy en (%d,%d),%dº\n",mypos[0],mypos[1],ori);
    
    //Finalmente, nos quedamos mirando en la orientación que nos piden.
    ang = orit - ori;
    printf("\n Giraré %dº pa llegar a %dº\n",ang,orit);
    if(fabs(ang) > 180){
		if(ang > 0){
			sgn = 1;
		} else if(ang < 0) {
			sgn = -1;
		}
		
		ang = ang -sgn*360;
	}
	printf("Giraré %dº pa llegar a %dº\n",ang,orit);
    //printf("Mi orientación es %d. Quiero terminar en %d y para ello debo girar %d.\n",ori,orit,ang);
    gira(ang);
    printf("Vale, ahora estoy en (%d,%d),%dº\n",mypos[0],mypos[1],ori);
}

