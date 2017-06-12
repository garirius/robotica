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

Nodo graph[MAX_ELEMENTS];
int grtam;
int ruta[MAX_ELEMENTS];
int muest[MAX_ELEMENTS][2];

//Determina si X punto cae dentro del mapa.
int inMap(int x, int y);
//Determina el tamaño del mapa.
void getSize(char* name);

/*** Dice si una posición x,y del mapa[tam1][tam2] vale para crear un nodo del grafo
   * Devuelve 1 si sí, 0 si no.
   * Para ser un nodo válido, la celda debe cumplir 3 condiciones:
   * a) contener un valor numérico entre 0 y 9
   * b) no contener un 1 ni un 2 (léase: obstáculos)
   * c) no ser contigua a un 1 ni un 2 (por temas de maniobrabilidad, básicamente)
   ***/
int isValid(int x, int y);

//Devuelve el índice del nodo correspondiente a la celda (x,y) del mapa
//En caso de no existir en el grafo, devuelve -1
int inGraph(int x, int y);

//Devuelve la distancia entre dos nodos de un grafo.
float distGraph(int pos1, int pos2);

//Inicializa el grafo con el mapa que tenemos.
void initGraph();

//Lee un archivo y saca un mapa, así como su tamaño
void leeMap(char* name);

void showMap();

//Muestra información sobre un nodo en particular
void showNodo(int whe);

//Nos dice cuál es el siguiente nodo por visitar más cercano al punto de partida
int nextNode();

//Sigue una trayectoria de nodos
void followGraph(int len);

//Ejecuta el algoritmo de Dijkstra para obtener una trayectoria óptima
//from y to son los puntos de salida y llegada
//v[len] es el array en el que se guardan los ID de los nodos que forman la trayectoria óptima
int dijkstra(int from[2], int to[2]);

/*** Recorre un vector en busca de obstáculos en map.
 * 0 es que no hay obstáculos
 * 1 es que sí ***/
 int checkVector(int v[][2], int len);
 
 /*** Saca qué puntos tiene que comprobar entre los puntos 1 y 2 
 * punto1 y punto2 son las entradas de la función
 * muest y marge son los arrays de puntos que hay que mirar
 * muest[len] son los puntos por los que pasa la recta ***/
void whichPoints(int punto1[2], int punto2[2], int* len);
 
/*** Comprueba si hay obstáculos entre los puntos 1 y 2
 * 0 es que no hay obstáculos
 * 1 es que sí ***/
 int check4Obstacles(int id1, int id2);
 

//Pinta las celdas de map especificadas por vec[len]
void paint(int* v, int len, char mode);

//Elimina el pos-ésimo elemento del array ruta
int removefromruta(int wh, int len);

//Llena de lo que queramos todas las celdas que pasen por la recta entre from y to.
void fillLine(int from[2], int to[2], int what);

void addObstacle(int from[2], int lookin);

//quita los '3' del mapa
//esos '3' los añadimos para mostrar la ruta que seguirá el robot
void resetMap();