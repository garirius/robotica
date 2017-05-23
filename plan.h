#define TOOFAR 10
//Nuestras variables globales de posición y orientación
extern int mypos[2], ori;

//Comprueba si hemos llegado a nuestro destino.
int thereYet(int* post);

//Va al punto que le decimos
void ira(int* post);

//Sigue un camino definido por path
void follow(int** path, int len);
