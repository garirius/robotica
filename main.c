#include <stdio.h>
#include <wiringPi.h>
#include <softPwm.h>
#include "move.h"
#include <mcp3004.h>
#include <stdlib.h>
#include "sense.h"
#include "plan.h"

#define INIVAL 0 // Valor inicial del PWM
#define RANGO 100 // Rango del PWM
extern float dista[2];
float mypos[2], ori;

int main(){
    char who; //0 sensor izq triggered y 1 sensor dcho triggered
	wiringPiSetup(); /* Inicializamos WiringPi */
	mcp3004Setup(100,0); //Inicializamos las cosillas del ADC
        
	/* Ya procedemos a crear los PWM */
	softPwmCreate(PINR, INIVAL, RANGO);
	softPwmCreate(PINL, INIVAL, RANGO);
	int izq=0, dch=0; /* Marchas PWM */
    
	//printf("Pulsa ENTER para ser guay.\n");
	///getchar();
    
    //Avanzar 2m y girar 90 cm
    /*advance(200);
    printf("¡Aparcao!\n");
    delay(250);
    printf("¡Voy a girar...!\n");
    gira(-90);
    printf("¡Ya he girao!\n");*/
    
    //Hacer toda la maniobra de detectar obstáculos
	/*gofw(1);
	while(dista[0] == -1 && dista[1] == -1){}
	stop(); //para al detectar algo
    delay(1000);
    if(dista[0] == -1 && dista[1] != -1){
        who = 1;
    } else {
        who = 0;
    }
    gofw(1);
    while(dista[who] > 20){}
    stop();
	delay(1000);
	gira(90);
    delay(100);
    advance(40);
    delay(100);
    gira(-90);
    delay(100);
    advance(50);*/
    
    int i,len,orit, aux1, aux2, aux3;
    int** path;
    
    printf("Dime la coordenada x y ori inicial:\n");
    scanf("%d %d %d", &aux1, &aux2, &aux3);
    mypos[0] = aux1;
    mypos[1] = aux2;
    ori = aux3;
    printf("Vale, parto en (%.0f, %.0f) con orientación %.0fº.\n",mypos[0],mypos[1],ori);
    
    printf("\n Dime por cuántos puntos quieres que pase:\n");
    scanf("%d", &len);
    
    path = malloc(len*sizeof(int*));
    printf("Ve especificándome uno por uno los puntos por los que quieres que pase.\n");
    for(i=0; i<len; i++){
        path[i] = malloc(2*sizeof(int));
        scanf("%d %d",&path[i][0],&path[i][1]);
    }
    motoresSetup();
    sensoresSetup();
    follow(path,len);
        
    //advance(100);
    //delay(1000);
    return 0;
}
