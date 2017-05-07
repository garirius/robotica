#include <math.h>
#include "move.h"

//Nuestras variables globales de posición y orientación
extern int mypos[2], ori;

void goto(int* post[2],int orit){
    //calculamos las diferencias de cosas que necesitamos
    int deltax = post[0]-mypos[0];
    int deltay = post[1]-mypos[1];
    double deltatheta = atan2(deltay,deltax);
    
    //Calculamos el ángulo que hay que girar para ir en línea recta.
    int ang = deltatheta*180/PI - ori;
    //Giramos ese ángulo
    gira(ang);
    delay(10);
    
    //Calculamos la distancia que necesitamos recorrer en línea recta.
    int howlong = sqrt(deltax*deltax+deltay*deltay);
    advance(howlong);
    delay(10);
    
    //Finalmente, nos quedamos mirando en la orientación que nos piden.
    int ang = orit - ori;
    gira(ang);
}

