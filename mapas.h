
#define MAX_ELEMENTS 10000

extern int** map;

//Determina el tamaño del mapa.
void getSize(char* name, int* tam1, int* tam2);

/*** Dice si una posición x,y del mapa[tam1][tam2] vale para crear un nodo del grafo
   * Devuelve 1 si sí, 0 si no.
   * Para ser un nodo válido, la celda debe cumplir 3 condiciones:
   * a) contener un valor numérico entre 0 y 9
   * b) no contener un 1 ni un 2 (léase: obstáculos)
   * c) no ser contigua a un 1 ni un 2 (por temas de maniobrabilidad, básicamente)
   ***/
int isValid(int x, int y, int tam1, int tam2);

//Devuelve el índice del nodo correspondiente a la celda (x,y) del mapa
//En caso de no existir en el grafo, devuelve -1
int inGraph(int x, int y);

//Devuelve la distancia entre dos nodos de un grafo.
float distGraph(int pos1, int pos2);
//Inicializa el grafo con el mapa que tenemos.
void initGraph(int tam1, int tam2);

//Lee un archivo y saca un mapa, así como su tamaño
void leeMap(char* name, int tam1, int tam2);

void showMap(int tam1, int tam2);

//Muestra información sobre un nodo en particular
void showNodo(int whe);

//Nos dice cuál es el siguiente nodo por visitar más cercano al punto de partida
int nextNode();

//Sigue una trayectoria de nodos
void followGraph( int len);
//Ejecuta el algoritmo de Dijkstra para obtener una trayectoria óptima
//from y to son los puntos de salida y llegada
//v[len] es el array en el que se guardan los ID de los nodos que forman la trayectoria óptima
int dijkstra(int from[2], int to[2]);

/*** Recorre un vector en busca de obstáculos en map.
 * 0 es que no hay obstáculos
 * 1 es que sí ***/
 int checkVector(int v[][2], int len, int tam1, int tam2);
 
 /*** Saca qué puntos tiene que comprobar entre los puntos 1 y 2 
 * punto1 y punto2 son las entradas de la función
 * muest y marge son los arrays de puntos que hay que mirar
 * muest[len] son los puntos por los que pasa la recta ***/
void whichPoints(int punto1[2], int punto2[2], int* len, int size1, int size2);
 
/*** Comprueba si hay obstáculos entre los puntos 1 y 2
 * 0 es que no hay obstáculos
 * 1 es que sí ***/
 int check4Obstacles(int id1, int id2, int size1, int size2);

//Pinta las celdas de map especificadas por vec[len]
void paint(int* v, int len, char mode, int tam1, int tam2);

//Elimina el pos-ésimo elemento del array ruta
int removefromruta(int wh, int len);

int refine(int len, int tam1, int tam2);
