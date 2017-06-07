#include <stdio.h>
#include <wiringPi.h>
#include <softPwm.h>
#include "move.h"
#include <mcp3004.h>
#include <stdlib.h>
#include "sense.h"
#include "plan.h"
#include "mapas.h"

#define INIVAL 0 // Valor inicial del PWM
#define RANGO 100 // Rango del PWM
extern float dista[2];
int mypos[2], ori;
int** map;
extern int ruta[MAX_ELEMENTS];

int main(){
    char who; //0 sensor izq triggered y 1 sensor dcho triggered
	wiringPiSetup(); /* Inicializamos WiringPi */
	mcp3004Setup(100,0); //Inicializamos las cosillas del ADC
    motoresSetup();
    sensoresSetup();
    
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
    
    /* int i,len,orit;
    int** path;
    
    printf("Dime la coordenada x y ori inicial:\n");
    scanf("%d %d %d", &mypos[0], &mypos[1], &ori);
    printf("Vale, parto en (%d, %d) con orientación %dº.\n",mypos[0],mypos[1],ori);
    
    printf("\n Dime por cuántos puntos quieres que pase:\n");
    scanf("%d", &len);
    
    path = malloc(len*sizeof(int*));
    printf("Ve especificándome uno por uno los puntos por los que quieres que pase.\n");
    for(i=0; i<len; i++){
        path[i] = malloc(2*sizeof(int));
        scanf("%d %d",&path[i][0],&path[i][1]);
    }
    
    follow(path,len);*/
    
    int tam1,tam2,n;
     int po[2]={0,0};
     int pf[2]={37,6};
     int len;
     int id1, id2;
     getSize("mapa.txt",&tam1,&tam2);
     map = (int**)malloc((tam2+1)*sizeof(int*));
     for(n=tam2-1; n>=0; n=n-1){
		map[n] = (int*)malloc(tam1*sizeof(int));
	 }
     leeMap("mapa.txt",tam1,tam2);
     //Una vez leído el mapa, vamos a ver qué tal funciona el tema de detectar obstáculos
     //showMap(tam1,tam2);
     
     //Inicializamos el grafo
     printf("Inicializando grafo...\n");
     initGraph(tam1,tam2);
     printf("Grafo inicializado.\nDame dos puntikos así wapos:\n");
     scanf("%d %d",&po[0],&po[1]);
     scanf("%d %d",&pf[0],&pf[1]);
     len = dijkstra(po,pf);
     len = refine(len,tam1,tam2);
     
     
     paint(ruta,len,'0',tam1,tam2);
     showMap(tam1,tam2);
     len = removefromruta(0,len);
     followGraph(len);
    
    
    //advance(100);
    //delay(1000);
    return 0;
}
