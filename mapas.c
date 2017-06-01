#include<math.h>

#define MARGEN 1.0 //1 celda - 10 cm

/*** Saca qué puntos tiene que comprobar entre los puntos 1 y 2 
 * punto1 y punto2 son las entradas de la función
 * muest y marge son los arrays de puntos que hay que mirar
 * muest[len] son los puntos por los que pasa la recta
 * marge[len2] son los puntos que damos de pseudomargen para completar ***/
void whichPoints(int* punto1, int* punto2, int** muest, int* len, int** marge, int* len2){
	int deltax = punto2[0]-punto1[0];
	int deltay = punto2[1]-punto1[1];
	float m = deltay/(deltax+0.0);
	float n = punto1[1]-m*punto1[0];
	float res;
	int tam,i,sgn,aux,tam2;
	
	//miramos si muestrear en x ó en y
	if(abs(deltax) > abs(deltay)){
		//hay más variación en x
		tam = abs(deltax)+1;
		muest = (int**)malloc(tam*sizeof(int*));
		if(deltax > 0){
			sgn = 1;
		} else {
			sgn = -1;
		}
		
		//Mirar por qué puntos pasa la recta.
		for(i=0;i<tam;i++){
			muest[i] = (int*)malloc(2*sizeof(int));
			muest[i][0] = punto1[0]+i*sgn;
			res = m*muest[i][0]+n;
			muest[i][1] = roundf(res);
		}
		
		tam2 = 0;
		//miramos ahora de darle unos margencillos bien
		//para ello, comprobamos en qué momentos se cambia de y
		for(i=1; i<tam; i++){
			if(muest[i][1] != muest[i-1][1]){
				tam2++;
			}
		}
		
		//Calculamos los márgenes, just in case
		marge = (int**)malloc(2*tam2*sizeof(int*));
		aux = 0;
		for(i=1; i<tam; i++){
			if(muest[i][1] != muest[i-1][1]){
				marge[aux] = (int*)malloc(2*sizeof(int));
				marge[aux][0] = muest[i][0];
				marge[aux][1] = muest[i-1][1];
				aux++;
				marge[aux] = (int*)malloc(2*sizeof(int));
				marge[aux][0] = muest[i-1][0];
				marge[aux][1] = muest[i][1];
			}
		}
		
		//Una vez tenemos marge y muest, le decimos al programa quién manda
		*len=tam;
		*len2 = tam2;
	} else {
		//hay más variación en y
		m = 1/m;
		n = -n*m;
		tam = abs(deltay)+1;
		muest = (int**)malloc(tam*sizeof(int*));
		if(deltay > 0){
			sgn = 1;
		} else {
			sgn = -1;
		}
		
		//Mirar por qué puntos pasa la recta.
		for(i=0;i<tam;i++){
			muest[i] = (int*)malloc(2*sizeof(int));
			muest[i][1] = punto1[1]+i*sgn;
			res = m*muest[i][1]+n;
			muest[i][0] = roundf(res);
		}
		
		tam2 = 0;
		//miramos ahora de darle unos margencillos bien
		//para ello, comprobamos en qué momentos se cambia de y
		for(i=1; i<tam; i++){
			if(muest[i][0] != muest[i-1][0]){
				tam2++;
			}
		}
		
		//Calculamos los márgenes, just in case
		marge = (int**)malloc(2*tam2*sizeof(int*));
		aux = 0;
		for(i=1; i<tam; i++){
			if(muest[i][0] != muest[i-1][0]){
				marge[aux] = (int*)malloc(2*sizeof(int));
				marge[aux][0] = muest[i][0];
				marge[aux][1] = muest[i-1][1];
				aux++;
				marge[aux] = (int*)malloc(2*sizeof(int));
				marge[aux][0] = muest[i-1][0];
				marge[aux][1] = muest[i][1];
			}
		}
		
		//Una vez tenemos marge y muest, le decimos al programa quién manda
		*len=tam;
		*len2 = tam2;
	}
}

/*** Recorre un vector en busca de obstáculos en map.
 * 0 es que no hay obstáculos
 * 1 es que sí ***/
 int checkVector(int** map, int** v, int len){
	 int i=0, equis, ygriega, esta, res=0;
	 for(i=0; i<len; i++){
		 //¿En qué coordenadas hay que mirar? muest nos lo dice
		 equis = v[i][0];
		 ygriega = v[i][1];
		 esta = map[equis][ygriega];
		 
		 if((esta==1)||(esta==2)){
			 res = 1;
		 }
		 i++;
	 }
	 
	 return res;
 }

/*** Comprueba si hay obstáculos entre los puntos 1 y 2
 * 0 es que no hay obstáculos
 * 1 es que sí ***/
 int check4Obstacles(int** map, int* punto1, int* punto2){
	 int** muest;
	 int** marge;
	 int len, len2;
	 int res = 0, i;
	 int equis, ygriega, esta;
	 int r[2], aux1[2], aux2[2];
	 float theta, m, n;
	 
	 whichPoints(punto1,punto2,muest,&len,marge,&len2);
	 //comprobamos el array muest pa ver si hay obstáculos
	 res = checkVector(map,muest,len);
	 
	 //comprobamos el array marge pa ver si hay obstáculos
	 if(!res){
		 res = checkVector(map,marge,len2);
	 }
	 
	 if(!res){
		theta = atan2(punto2[1]-punto1[1],punto2[0]-punto1[0]);
		r[0]=roundf(MARGEN*sin(theta));
		r[1]=-roundf(MARGEN*cos(theta));
		
		for(i=0; i<2; i++){
			aux1[i] = punto1[i]+r[i];
			aux2[i] = punto2[i]+r[i];
		}
		
		whichPoints(aux1,aux2,muest,&len,marge,&len2);
		res = checkVector(map,muest,len);
		
		if(!res){
			for(i=0; i<2; i++){
				aux1[i] = punto1[i]-r[i];
				aux2[i] = punto2[i]-r[i];
			}
		
			whichPoints(aux1,aux2,muest,&len,marge,&len2);
			res = checkVector(map,muest,len);
		}
	 }
	 
	 return res;
 }
 
 int main(){
	 return 0;
 }
