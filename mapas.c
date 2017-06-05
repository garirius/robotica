#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "plan.h"

#define MARGEN 1.0 //1 celda - 10 cm

//definimos el struct nodo de grafo
typedef enum estado = {NOVISITADO=0, VISITADO, NUM_MODOS};
struct Nodo {
    estado state; //estado del nodo: visitado o novisitado
    int pos[2]; //posición del mapa a la que corresponde el nodo
    int num_vecinos; //número de vecinos que tiene el nodo
    int* id_vecinos; //array con los ID (aka posiciones en el array) de los nodos vecinos
    float* dist_vecinos; //array con las distancias a las que están los vecinos
    int previo; //nos dice el id del nodo previo en la trayectoria óptima
    float disfrom; //nos dice la distancia a la fuente
}

int** map;
Nodo* graph;
int grtam;

//Determina el tamaño del mapa.
void getSize(char* name, int* tam1, int* tam2){
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
    *tam1 = s1-1;
    *tam2 = s2+1;
    fclose(fp);
}

/*** Dice si una posición x,y del mapa[tam1][tam2] vale para crear un nodo del grafo
   * Devuelve 1 si sí, 0 si no.
   * Para ser un nodo válido, la celda debe cumplir 3 condiciones:
   * a) contener un valor numérico entre 0 y 9
   * b) no contener un 1 ni un 2 (léase: obstáculos)
   * c) no ser contigua a un 1 ni un 2 (por temas de maniobrabilidad, básicamente)
   ***/
int isValid(int x, int y, int tam1, int tam2){
    int ele = map[x][y];
    int cond1,cond2;
    int res = 0; 
    //recorremos el vecindario de (x,y) para ver si se cumplen las condiciones b y c
    int n,m;
    
    for(n=-1; n<=1; n++){
        for(m=-1; m<=1; m++){
            //primero comprobamos que los índices no se salen del mapa
            if(((x+n>tam1)&&(x+n<0))&&((y+m>tam2)&&(y+m<0))){
                ele = map[x+n][y+m];
                //Y entonces comprobamos que se cumplen las condiciones
                cond1 = (ele >= 0) && (ele <= 9); //condición a
                cond2 = (ele != 1) && (ele != 2); //condición b
                if(cond1 && cond2){
                    res = 1;
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
void initGraph(int tam1, int tam2){
    int n,m, id, tot, tot2, aux, cnt;
    id = 0;
    //contaremos sólo como nodos válidos aquellos que NO tengan ni sean contiguos a un 1 ó un 2
    for(n=0; n<tam1;n++){
        for(m=0; m<tam2; m++){
            if(isValid(n,m,tam1,tam2)){
                tot++;
            }
        }
    }
    
    //Reservamos la memoria que haga falta
    graph = (Nodo*)malloc(tot*sizeof(Nodo));
    grtam = tot;
    
    //Recorremos nuevamente el mapa, esta vez inicializando ya nodos.
    for(n=0; n<tam1;n++){
        for(m=0; m<tam2; m++){
            if(isValid(n,m,tam1,tam2)){
                graph[id].pos[0]=n;
                graph[id].pos[0]=m;
                //Y contamos cuántos vecinos posibles tiene la celda
                graph[id].num_vecinos=0;
                for(tot=-1; tot<=1; tot++){
                    for(tot2=-1; tot2<=1; tot2++){
                        if(isValid(n+tot,m+tot2,tam1,tam2)){
                            graph[id].num_vecinos++;
                        }
                    }
                }
                //Tras saberlo, reservamos la memoria necesaria.
                graph[id].id_vecinos = (int*)malloc(num_vecinos*sizeof(int));
                graph[id].dist_vecinos = (float*)malloc(num_vecinos*sizeof(int));
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
                if(aux >= 0){ //si la celda contigua corresponde a un nodo del grafo, lo apuntamos.
                    graph[id].id_vecinos[cnt]=aux;
                    graph[id].dist_vecinos[cnt]=distGraph(aux,id);
                    cnt++;
                }
            }
        }
    }
}

//Lee un archivo y saca un mapa, así como su tamaño
void leeMap(char* name, int tam1, int tam2){
    FILE * fp;
    int n,m;
    char aux;
    int over;
    fp = fopen(name,"r");
    
    map = (int**)malloc((tam2+1)*sizeof(int*));
    //Como para nosotros el origen del mapa está abajo de todo, hay que leer el mapa del revés.
    for(n=tam2-1; n>=0; n=n-1){
		map[n] = (int*)malloc(tam1*sizeof(int));
		for(m=0; m<tam1+2; m++){
			fscanf(fp, "%c", &aux);
			if((aux >= '0')&&(aux <= '9')){
				map[n][m]= aux - '0';
			}
		}
	}
    fclose(fp);
    
    for(n=tam2-1; n>=0; n=n-1){
		for(m=0; m<tam1; m++){
			printf("%d",map[n][m]);
		}
		printf("\n");
	}
    printf("Mapa leído!\n");
}

//Escribe un archivo con el mapa
void escribeMap(char* name, int tam1, int tam2){
    FILE * fp = fopen(name,"w");
    int n,m;
    printf("estoi escrebiendo\n");
    for(n=tam2-1; n>=0; n=n-1){
		for(m=0; m<tam1; m++){
			fprintf(fp,"%d",map[n][m]);
		}
		fprintf(fp,"\n");
	}
    fclose(fp);
    printf("pa que me interrompes atontao que no repetas\n");
    printf("wtf ebil floutin poin bit level jaquin\n");
    printf("jejeje es broma de la enformateca\n");
}

void showMap(int tam1, int tam2){
    int n,m;
    
    for(n=tam2-1; n>=0; n=n-1){
		for(m=0; m<tam1; m++){
			printf("%d",map[n][m]);
		}
		printf("\n");
	}
}

//Muestra información sobre un nodo en particular
void showNodo(Nodo what){
    int n,pos;
    pos = inGraph(what.pos[0],what.pos[1]);
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
        if(graph[k].state = NOVISITADO){ //sólo consideramos los que están por visitar
            if(graph[k].disfrom < dismin){
                n = k;
            }
        }
    }
    
    return n;
}

//Sigue una trayectoria de nodos
void followGraph(int* v, int len){
    int** path = malloc(len*sizeof(int*));
    int k;
    
    for(k=0; k<len; k++){
        path[k] = malloc(2*sizeof(int));
        path[k][0] = v[k].pos[0];
        path[k][1] = v[k].pos[1];
    }
    
    follow(int** path, int len);
}
//Ejecuta el algoritmo de Dijkstra para obtener una trayectoria óptima
//from y to son los puntos de salida y llegada
//v[len] es el array en el que se guardan los ID de los nodos que forman la trayectoria óptima
void dijkstra(int from[2], int to[2], int* v, int* len){
    int n, remaining, k;
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
        *len = 1;
        n = id2;
        while((n!=id1)&&(n>-1)){ //mientras no hayamos llegado al destino...
            n = graph[n].previo;
            *len++;
        }
        
        if(n>-1){ //si hemos podido hallar una forma de llegar a id2...
            v = (int*)malloc((*len)*sizeof(int)); //reservamos memoria
            n = id2;
            k = *len -1;
            while((n!=id1)&&(k >= 0)){ //volvemos a recorrer el vector del revés
                v[k] = n; //guardamos el id del nodo en la última posición disponible
                n = graph[n].previo;
                k--;
            }
        } else {
            printf("¡No se puede realizar la trayectoria!");
        }
    } else {
        printf("¡No se puede realizar la trayectoria!");
    }
    
}

//Pinta las celdas de map especificadas por vec[len]
void paint(char which, int len, char mode){
    int n,x,y;
    int** vec;
    switch(which){
		case '0':
			vec = muest;
			break;
		case '1':
			vec = marge;
			break;
	}
    for(n=0; n<len; n++){
        x = vec[n][0];
        y = vec[n][1];
        
        switch(mode){
            case '0': //
                map[x][y]=3;
                break;
            case '1':
                map[x][y] += 3;
                break;
        }
    }
}


 int main(){
     int tam1,tam2,n;
     int po[2]={2,2};
     int pf[2]={8,8};
     int len, len2;
     getSize("mapa.txt",&tam1,&tam2);
     map = (int**)malloc((tam2+1)*sizeof(int*));
     for(n=tam2-1; n>=0; n=n-1){
		map[n] = (int*)malloc(tam1*sizeof(int));
	 }
     leeMap("mapa.txt",tam1,tam2);
     //Una vez leído el mapa, vamos a ver qué tal funciona el tema de detectar obstáculos
     showMap(tam1,tam2);
     whichPoints(po,pf,&len,&len2);
     printf("Terminé de medir mis mierdas");
     paint('0',len,'0');
     paint('1',len2,'0');
     printf("\n");
     showMap(tam1,tam2);
     escribeMap("testmapetix.txt",tam1,tam2);
     return 0;
 }
