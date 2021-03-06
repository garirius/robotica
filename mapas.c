#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "sense.h"
#include "move.h"

#define MAX_ELEMENTS 10000
#define CELL_SIZE 10.0
#define OBST_LENGTH 40
#define OBST_WIDTH 40
//definimos el struct nodo de grafo
typedef enum status {NOVISITADO=0, VISITADO, NUM_MODOS} Estado;

typedef struct nodus {
    Estado state; //estado del nodo: visitado o novisitado
    int pos[2]; //posición del mapa a la que corresponde el nodo
    int num_vecinos; //número de vecinos que tiene el nodo
    int id_vecinos[8]; //array con los ID (aka posiciones en el array) de los nodos vecinos
    float dist_vecinos[8]; //array con las distancias a las que están los vecinos
    int previo; //nos dice el id del nodo previo en la trayectoria óptima
    float disfrom; //nos dice la distancia a la fuente
} Nodo;

extern int** map;
extern tamano1, tamano2;
Nodo graph[MAX_ELEMENTS];
int grtam;
int ruta[MAX_ELEMENTS];
int muest[MAX_ELEMENTS][2];

//Determina si X punto cae dentro del mapa.
int inMap(int x, int y){
    int cond1 = (x<0)||(x>=tamano1);
    int cond2 = (y<0)||(y>=tamano2);
    
    if(cond1||cond2){
        return 0;
    } else {
        return 1;
    }
}
//Determina el tamaño del mapa.
void getSize(char* name){
	FILE * fp = fopen(name,"r");
    int n,m;
    int s1 =0;
    int s2 =0;
    char aux;
    int over;
    
    if(fp == NULL){
        printf("Error abriendo el archivo.\n");
        exit(-1);
    }
    over = 0;
    printf("Leyendo mapa...\n");
    while(!over){ //vamos viendo qué tamaño tiene el mapa
        fscanf(fp, "%c", &aux);
        if(aux!='\n'){//si el char leído no es un salto de línea, aumentamos el tamaño
            s1++;
        } else { //si pillamos un salto de línea, actuamos en consecuencia
            if((s1 == n)||(s2==0)){ //si el mapa es cuadrado, todo bien
                n = s1;
                s1 = 0;
                s2++;
            } else {
                printf("OJOCUIDAO! El mapa no es cuadrado.\n");
                exit(-1);
            }
        }
        over = feof(fp);
    }
    tamano1 = s1-1;
    tamano2 = s2+1;
    fclose(fp);
}

/*** Dice si una posición x,y del mapa[tam1][tam2] vale para crear un nodo del grafo
   * Devuelve 1 si sí, 0 si no.
   * Para ser un nodo válido, la celda debe cumplir 3 condiciones:
   * a) contener un valor numérico entre 0 y 9
   * b) no contener un 1 ni un 2 (léase: obstáculos)
   * c) no ser contigua a un 1 ni un 2 (por temas de maniobrabilidad, básicamente)
   ***/
int isValid(int x, int y){
    int ele;
    int cond;
    int res = 1; 
    //recorremos el vecindario de (x,y) para ver si se cumplen las condiciones b y c
    int n,m;
    if(inMap(x,y)){
        res = 0;
    } else {
        for(n=-1; n<=1; n++){
            for(m=-1; m<=1; m++){
                //primero comprobamos que los índices no se salen del mapa

                if(inMap(x+n,y+n)){
                    ele = map[y+m][x+n];
                    //Y entonces comprobamos que se cumplen las condiciones
                    cond = (ele != 1) && (ele != 2); //condición b
                    if(!cond){
                        res = 0;
                    }

                }
            }
        }
    }
    return res;
}

//Devuelve el índice del nodo correspondiente a la celda (x,y) del mapa
//En caso de no existir en el grafo, devuelve -1
int inGraph(int x, int y){
    int n;
    for(n=0; n<grtam; n++){
        if((graph[n].pos[0]==x)&&(graph[n].pos[1]==y)){
            return n;
        }
    }
    return -1;
}

//Devuelve la distancia entre dos nodos de un grafo.
float distGraph(int pos1, int pos2){
    int x1 = graph[pos1].pos[0];
    int y1 = graph[pos1].pos[1];
    int x2 = graph[pos2].pos[0];
    int y2 = graph[pos2].pos[1];
    
    return sqrt((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2));
}
//Inicializa el grafo con el mapa que tenemos.
void initGraph(){
    int n,m, id, tot, tot2, aux, cnt;
    tot = 0;
    id = 0;
    //contaremos sólo como nodos válidos aquellos que NO tengan ni sean contiguos a un 1 ó un 2
    for(n=0; n<tamano2;n++){
        for(m=0; m<tamano1; m++){
            if(isValid(n,m)){
                tot++;
                printf("_");
            } else {
                printf("X");
            }
        }
        printf("\n");
    }
    //Reservamos la memoria que haga falta
    grtam = tot;
    //Recorremos nuevamente el mapa, esta vez inicializando ya nodos.
    for(n=0; n<tamano2;n++){
        for(m=0; m<tamano1; m++){
            if(isValid(n,m)){
                graph[id].pos[0]=m;
                graph[id].pos[1]=n;
                
                //Y contamos cuántos vecinos posibles tiene la celda
                graph[id].num_vecinos=0;
                
                for(tot=-1; tot<=1; tot++){
                    for(tot2=-1; tot2<=1; tot2++){
                       aux = isValid(n+tot,m+tot2);
                       if(aux&&(tot!=0 || tot2!=0)){
                           graph[id].num_vecinos++;                        
                       }
                    }
                } 
                id++;
            }
        }
    }
    
    //Finalmente, establecemos las conexiones de los nodos con sus vecinos.
    for(id=0; id<grtam; id++){
        n = graph[id].pos[0];
        m = graph[id].pos[1];
        cnt = 0;
        for(tot=-1; tot<=1; tot++){
            for(tot2=-1; tot2<=1; tot2++){
                aux = inGraph(n+tot,m+tot2);
                if((aux >= 0)&&(tot!=0 || tot2!=0)){ //si la celda contigua corresponde a un nodo del grafo, lo apuntamos.
                    graph[id].id_vecinos[cnt]=aux;
                    graph[id].dist_vecinos[cnt]=distGraph(aux,id);
                    cnt++;
                    if(cnt >= graph[id].num_vecinos){
                        break;
                    }
                }
            }
        }
    }
}

//Lee un archivo y saca un mapa, así como su tamaño
void leeMap(char* name){
    FILE * fp;
    int n,m;
    char aux;
    int over;
    fp = fopen(name,"r");
    printf("%dx%d\n",tamano1,tamano2);
    free(map);
    map = (int**)malloc((tamano2+1)*sizeof(int*));
    //Como para nosotros el origen del mapa está abajo de todo, hay que leer el mapa del revés.
    
    for(n=tamano2-1; n>=0; n=n-1){
		map[n] = (int*)malloc(tamano1*sizeof(int));
		for(m=0; m<tamano1+1; m++){
			fscanf(fp, "%c", &aux);
			if((aux >= '0')&&(aux <= '9')){
				map[n][m]= aux - '0';
			}
		}
	}
    fclose(fp);
    
    printf("Mapa leído!\n");
}

void showMap(){
    int n,m;
    
    for(n=tamano2-1; n>=0; n=n-1){
		for(m=0; m<tamano1; m++){
			printf("%d",map[n][m]);
		}
		printf("\n");
	}
}

//Muestra información sobre un nodo en particular
void showNodo(int whe){
    int n;
    Nodo what = graph[whe];
    printf("\nNodo (%d,%d) tiene %d vecinos\n",what.pos[0],what.pos[1],what.num_vecinos);
    for(n=0; n<what.num_vecinos; n++){
        printf("El (%d,%d), que está a una distancia %.1f\n",graph[what.id_vecinos[n]].pos[0], graph[what.id_vecinos[n]].pos[1], what.dist_vecinos[n]);
    }
    printf("\n");
}

//Nos dice cuál es el siguiente nodo por visitar más cercano al punto de partida
int nextNode(){
    int n, k;
    float dismin = INFINITY;
    for(k=0; k<grtam; k++){ //recorremos el grafo en busca del siguiente nodo
        if(graph[k].state == NOVISITADO){ //sólo consideramos los que están por visitar
            if(graph[k].disfrom < dismin){
                n = k;
                dismin = graph[k].disfrom;
            }
        }
    }
    
    return n;
}

//Sigue una trayectoria de nodos
void followGraph(int len){
    int** path = malloc(len*sizeof(int*));
    int k;
    
    for(k=0; k<len; k++){
        path[k] = malloc(2*sizeof(int));
        path[k][0] = 10*graph[ruta[k]].pos[0];
        path[k][1] = 10*graph[ruta[k]].pos[1];
    }
    
    follow(path,len);
}

//Ejecuta el algoritmo de Dijkstra para obtener una trayectoria óptima
//from y to son los puntos de salida y llegada
//v[len] es el array en el que se guardan los ID de los nodos que forman la trayectoria óptima
int dijkstra(int from[2], int to[2]){
    int n, remaining, k, len=-1;
    int id1, id2; //IDs de los nodos a los que corresponden los puntos del mapa
    float aux;
    Nodo ele;
    id1 = inGraph(from[0],from[1]);
    id2 = inGraph(to[0],to[1]);
    if((id1>-1)&&(id2>-1)){ //si ambas posiciones existen en el grafo, avanti
        //Partimos inicializando el conjunto Q de nodos por visitar
        for(n=0; n<grtam; n++){
            graph[n].disfrom = INFINITY;
            graph[n].previo = -1;
            graph[n].state = NOVISITADO;
            
        }
        graph[id1].disfrom = 0;
        n = id1; //partimos, cómo no, del punto de partida
        remaining = grtam;
        
        while((remaining>0)&&(n!=id2)){ //mientras queden nodos por visitar y no hayamos llegado...
            graph[n].state = VISITADO;
            ele = graph[n];
            for(k=0; k<ele.num_vecinos; k++){ 
                // recorremos los vecinos del n-ésimo nodo
                /* evidentemente, la distancia del origen al k-ésimo vecino
                   del n-ésimo nodo será la distancia del origen a éste más
                   la distancia del n-ésimo nodo a su vecino */
                aux = ele.disfrom + ele.dist_vecinos[k];
                if(aux < graph[ele.id_vecinos[k]].disfrom){ 
                    //si hemos encontrado una ruta más corta hasta v,
                    //actuamos en consecuencia
                    graph[ele.id_vecinos[k]].disfrom = aux;
                    graph[ele.id_vecinos[k]].previo = n;
                }
            }
            n = nextNode();
            remaining--; //¡un nodo menos!
        }
        
        //Una vez contamos con una cadena de nodos previos, funcionamos al revés
        //partimos del final y vamos contando hacia atrás para ver lo largo que será v
        len = 1;
        n = id2;
        while((n!=id1)&&(n>-1)){ //mientras no hayamos llegado al destino...
            n = graph[n].previo;
            len++;
        }
        if(n>-1){ //si hemos podido hallar una forma de llegar a id2...
            n = id2;
            k = len -1;
            while((n!=id1)&&(k >= 0)){ //volvemos a recorrer el vector del revés
                ruta[k] = n; //guardamos el id del nodo en la última posición disponible
                n = graph[n].previo;
                k--;
                
    
            }
            ruta[0] = id1;
        } else {
            printf("¡No se puede realizar la trayectoria!\n");
        }
    } else {
        printf("¡El punto que me dices tiene un obstáculo!\n");
    }
    return len;
}

/*** Recorre un vector en busca de obstáculos en map.
 * 0 es que no hay obstáculos
 * 1 es que sí ***/
 int checkVector(int v[][2], int len){
	 int i=0, equis, ygriega, esta, res=0;
	 for(i=0; i<len; i++){
		 //¿En qué coordenadas hay que mirar? muest nos lo dice
		 equis = v[i][0];
		 ygriega = v[i][1];
		 if(inMap(equis,ygriega)){
			 esta = map[ygriega][equis];
			 if((esta==1)||(esta==2)){
				 return 1;
			 }
		 }
	 }
	 
	 return 0;
 }
 
 /*** Saca qué puntos tiene que comprobar entre los puntos 1 y 2 
 * punto1 y punto2 son las entradas de la función
 * muest y marge son los arrays de puntos que hay que mirar
 * muest[len] son los puntos por los que pasa la recta ***/
void whichPoints(int punto1[2], int punto2[2], int* len){
	int deltax = punto2[0]-punto1[0];
	int deltay = punto2[1]-punto1[1];
	float m, n;
	float res;
	int tam,i,sgn,aux,tam2,k;
	int s1, s2;
	k = 0; 
	
	//miramos si muestrear en x ó en y
	if(abs(deltax) > abs(deltay)){
		//hay más variación en x
		m = deltay/(deltax+0.0);
		n = punto1[1]-m*punto1[0];
		tam = abs(deltax)+1;
		if(deltax > 0){
			sgn = 1;
		} else {
			sgn = -1;
		}
	} else {
		//hay más variación en y
		m = deltax/(deltay+0.0);
		n = punto1[0]-m*punto1[1];
		tam = abs(deltay)+1;
		
		if(deltay > 0){
			sgn = 1;
		} else {
			sgn = -1;
		}
	}
	
	//Mirar por qué puntos pasa la recta.
		for(i=0;i<tam;i++){
			muest[i][0] = punto1[0]+i*sgn;
			res = m*muest[i][0]+n;
			muest[i][1] = roundf(res);
		}
		
		tam2 = tam;
		k = tam-1;
		for(i=0; i<tam; i++){
			//para cada celda en la recta que comprobamos
			for(s1=-2; s1<=2; s1++){
				for(s2=-2; s2<=2; s2++){
					//añadimos los vecinos a la lista de cosas a comprobar
					aux = s1==0 && s2==0;
					//si se sale del mapa no lo tenemos en cuenta
					aux = aux || !inMap(muest[i][0]+s1,muest[i][1]+s2);
					if(!aux){
						muest[k][0] = muest[i][0]+s1;
						muest[k][1] = muest[i][1]+s2;
						k++;
						tam2++;
					}
				}
			}
		}
		
		//Una vez tenemos marge y muest, le decimos al programa quién manda
		*len=tam2;
}
 
/*** Comprueba si hay obstáculos entre los puntos 1 y 2
 * 0 es que no hay obstáculos
 * 1 es que sí ***/
 int check4Obstacles(int id1, int id2){
	 int len, len2;
	 int res = 0, i;
	 int equis, ygriega, esta;
	 int r[2], aux1[2], aux2[2];
	 float theta, m, n;
	 int punto1[2], punto2[2];
	 
	 punto1[0]=graph[id1].pos[0];
	 punto1[1]=graph[id1].pos[1];
	 punto2[0]=graph[id2].pos[0];
	 punto2[1]=graph[id2].pos[1];
	 whichPoints(punto1,punto2,&len);
	 //comprobamos el array muest pa ver si hay obstáculos
	 res = checkVector(muest,len);
	 return res;
 }
 

//Pinta las celdas de map especificadas por vec[len]
void paint(int* v, int len, char mode){
    int n,x,y,id;
    for(n=0; n<len; n++){
        id = v[n];
        x = graph[id].pos[0];
        y = graph[id].pos[1];
        if(inMap(x,y)){
            switch(mode){
                case '0': //
                    map[y][x]=3;
                    break;
                case '1':
                    map[y][x] += 3;
                    break;
            }
        }
    }
}

//Elimina el pos-ésimo elemento del array ruta
int removefromruta(int wh, int len){
	int n;
	
	for(n=wh; n<len-1; n++){
		ruta[n] = ruta[n+1];
	}
	
	return len-1;
}

int refine(int len){
	int i=0, is, j;
	
	//Vamos a recorrer el array ruta
	while(i < len-2){
		j = i+2;
		is = check4Obstacles(ruta[i],ruta[j]);
		if(!is){
			//si no hay obstáculos entre el i-ésimo y el j-ésimo nodo,
			//eliminamos el j-1-ésimo nodo, que es redundante
			len = removefromruta(j-1,len);
		} else {
			i++;
		}
	}
	
	
	return len;
}

//Llena de lo que queramos todas las celdas que pasen por la recta entre from y to.
void fillLine(int from[2], int to[2], int what){
    int deltax = to[0]-from[0];
	int deltay = to[1]-from[1];
	float m, n;
	float res;
	int tam,i,sgn;
	int s1, s2;
	int k = 0; 
	
	//miramos si muestrear en x ó en y
	if(abs(deltax) > abs(deltay)){
		//hay más variación en x
		m = deltay/(deltax+0.0);
		n = from[1]-m*from[0];
		tam = abs(deltax)+1;
		if(deltax > 0){
			sgn = 1;
		} else {
			sgn = -1;
		}
	} else {
		//hay más variación en y
		m = deltax/(deltay+0.0);
		n = from[0]-m*from[1];
		tam = abs(deltay)+1;
		
		if(deltay > 0){
			sgn = 1;
		} else {
			sgn = -1;
		}
	}
	
	//Mirar por qué puntos pasa la recta y pintar eso
		for(i=0;i<tam;i++){
			s1 = from[0]+i*sgn;
			res = m*muest[i][0]+n;
			s2 = roundf(res);
            if(inMap(s1,s2)){
                map[s2][s1]=what;
            }
		}
}

void addObstacle(int from[2], int lookin){
    int corners[4][2];
    float x = from[0]/CELL_SIZE;
    float y = from[1]/CELL_SIZE;
    float ang = lookin*PI/180;
    int i, j;
    //empezamos buscando las esquinas
    x = x + LONGING*cos(ang) + OBST_LENGTH*cos(ang+PI/2)/2;
    y = y + LONGING*sin(ang) + OBST_LENGTH*sin(ang+PI/2)/2;
    
    for(i=0; i<4; i++){
        switch(i){
            case 0:
                break;
            case 1:
                x += OBST_WIDTH*cos(ang);
                y += OBST_WIDTH*sin(ang);
                break;
            case 2:
                x -= OBST_LENGTH*cos(ang+PI/2);
                y -= OBST_LENGTH*sin(ang+PI/2);
                break;
            case 3:
                x -= OBST_WIDTH*cos(ang);
                y -= OBST_WIDTH*sin(ang);
                break;
        }
        
        corners[i][0] = roundf(x);
        corners[i][1] = roundf(y);
    }
    
    //Seguidamente, llenamos los lados y diagonales de '1'
    for(i=0; i<4; i++){
        for(j=0; j<4; j++){
            if(i!=j){
                fillLine(corners[i],corners[j],1);
            }
        }
    }
    
    //Para rematar, ponemos que las esquinas sean '2'
    for(i=0; i<4; i++){
        map[corners[i][1]][corners[i][0]] = 2;
    }
}

//quita los '3' del mapa
//esos '3' los añadimos para mostrar la ruta que seguirá el robot
void resetMap(){
    int n,m;
    for(n=0; n<tamano2; n++){
        for(m=0; m<tamano1; m++){
            if(map[n][m]==3){
                map[n][m] = 0;
            }
        }
    }
}
