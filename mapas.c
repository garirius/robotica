#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#define MARGEN 1.0 //1 celda - 10 cm
int** map;
int** muest;
int** marge;

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

/*** Saca qué puntos tiene que comprobar entre los puntos 1 y 2 
 * punto1 y punto2 son las entradas de la función
 * muest y marge son los arrays de puntos que hay que mirar
 * muest[len] son los puntos por los que pasa la recta
 * marge[len2] son los puntos que damos de pseudomargen para completar ***/
void whichPoints(int punto1[2], int punto2[2], int* len, int* len2){
	int deltax = punto2[0]-punto1[0];
	int deltay = punto2[1]-punto1[1];
	float m = deltay/(deltax+0.0);
	float n = punto1[1]-m*punto1[0];
	float res;
	int tam,i,sgn,aux,tam2;
	free(muest);
	free(marge);
	printf("Pasito \n");
	//miramos si muestrear en x ó en y
	if(abs(deltax) > abs(deltay)){
		//hay más variación en x
		tam = abs(deltax)+1;
		muest = (int**)malloc(tam*sizeof(int*));
		printf("a pasito\n");
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
		marge = (int**)malloc(tam2*sizeof(int*));
		aux = 0;
		for(i=1; i<tam2; i++){
			if(muest[i][1] != muest[i-1][1]){
				marge[aux] = (int*)malloc(2*sizeof(int));
				marge[aux][0] = muest[i][0];
				marge[aux][1] = muest[i-1][1];
				aux++;
				marge[aux] = (int*)malloc(2*sizeof(int));
				marge[aux][0] = muest[i-1][0];
				marge[aux][1] = muest[i][1];
				aux++;
			}
		}
		
		//Una vez tenemos marge y muest, le decimos al programa quién manda
		*len=tam;
		*len2 = 2*tam2;
	} else {
		//hay más variación en y
		m = 1/m;
		n = -n*m;
		tam = abs(deltay)+1;
		muest = (int**)malloc((tam+1)*sizeof(int*));
		printf("a pasito, alojando %d arrays\n",tam);
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
		printf("%d\n",tam);
		//Calculamos los márgenes, just in case
		sgn = 2*tam2*sizeof(int*);
		marge = malloc(sgn);
		printf("Deberían caberme %d pollas\n",sgn);
		aux = 0;
		for(i=1; i<tam; i++){
			
			if(muest[i][0] != muest[i-1][0]){
				printf("NEWS FLASH: Estamos en i=%d\n",i);
				marge[aux] = (int*)malloc(2*sizeof(int));
				
				marge[aux][0] = muest[i][0];
				marge[aux][1] = muest[i-1][1];
				sgn = sgn - sizeof(marge[aux]);
				printf("Ahora mismo caben %d pollas más\n",sgn);
				aux++;
				
				
				marge[aux] = (int*)malloc(2*sizeof(int));
				marge[aux][0] = muest[i-1][0];
				marge[aux][1] = muest[i][1];
				sgn = sgn - sizeof(marge[aux]);
				printf("Ahora mismo caben %d pollas más\n",sgn);
				aux++;
			}
			
		}
		
		//Una vez tenemos marge y muest, le decimos al programa quién manda
		*len=tam;
		*len2 = 2*tam2;
	}
}

/*** Recorre un vector en busca de obstáculos en map.
 * 0 es que no hay obstáculos
 * 1 es que sí ***/
 int checkVector(int** v, int len){
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
 int check4Obstacles(int punto1[2], int punto2[2]){
	 int len, len2;
	 int res = 0, i;
	 int equis, ygriega, esta;
	 int r[2], aux1[2], aux2[2];
	 float theta, m, n;
	 
	 whichPoints(punto1,punto2,&len,&len2);
	 //comprobamos el array muest pa ver si hay obstáculos
	 res = checkVector(muest,len);
	 
	 //comprobamos el array marge pa ver si hay obstáculos
	 if(!res){
		 res = checkVector(marge,len2);
	 }
	 
	 if(!res){
		theta = atan2(punto2[1]-punto1[1],punto2[0]-punto1[0]);
		r[0]=roundf(MARGEN*sin(theta));
		r[1]=-roundf(MARGEN*cos(theta));
		
		for(i=0; i<2; i++){
			aux1[i] = punto1[i]+r[i];
			aux2[i] = punto2[i]+r[i];
		}
		
		whichPoints(aux1,aux2,&len,&len2);
		res = checkVector(muest,len);
		
		if(!res){
			for(i=0; i<2; i++){
				aux1[i] = punto1[i]-r[i];
				aux2[i] = punto2[i]-r[i];
			}
		
			whichPoints(aux1,aux2,&len,&len2);
			res = checkVector(muest,len);
		}
	 }
	 
	 return res;
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

float away(int p1[2], int p2[2]){
    int deltax = p2[0]-p1[0];
    int deltay = p2[1]-p2[1];
    return sqrt(deltax*deltax+deltay*deltay);
}

//Inserta un valor val en la posición pos de un vector v[len]
void inserta(int* v, int* len, int val, int pos){
    int lon = *len;
    int* aux = (int*)malloc(lon*sizeof(int));
    int n;
    
    //Copiar el array original
    for(n=0; n<lon; n++){
        aux[n]=v[n];
    }
    //Aumentar el tamaño
    *len = *len + 1;
    lon = *len;
    //Re-copiar el array original con el nuevo valor
    free(v);
    v = (int*)malloc(lon*sizeof(int));
    for(n=0; n<*len; n++){
        if(n<pos){
            v[n]=aux[n];
        } else if(n==pos){
            v[n] = val;
        } else {
            v[n]=aux[n-1];
        }
    }
}

void planning(int punto1[2], int punto2[2], int* xroute, int* yroute, int* len){
    int aux = check4Obstacles(punto1,punto2);
    int p1[2],p2[2]; //punto auxiliar
    int n=0;
    int aux1[2], aux2[2];
    if(!aux){ //si no hay obstáculos, damos luz verde a la trayectoria
        xroute = (int*)malloc(2*sizeof(int));
        yroute = (int*)malloc(2*sizeof(int));
        xroute[0] = punto1[0];
        yroute[0] = punto1[1];
        xroute[1] = punto2[0];
        yroute[1] = punto2[1];
        *len = 2;
    } else { //en caso contrario, sería cuestión de darle al tema.
        //Seguiremos haciendo esto hasta el día en que muramos.
        while(aux){
            //Sacamos los puntos cruzados.
            p1[0] = punto1[0];
            p1[1] = punto2[1];
            p2[0] = punto2[0];
            p2[1] = punto1[1];
            if(!check4Obstacles(p1,punto2)){
                planning(punto1,p1,xroute,yroute,len);
            } else if(!check4Obstacles(p2,punto2)){
                    planning(punto1,p2,xroute,yroute,len);
            } else {
                if(away(punto1,p1)>away(punto2,p1)){
                    aux1[0]=punto1[0];
                    aux1[1]=punto1[1];
                    aux2[0]=p1[0];
                    aux2[1]=p1[1];
                    while(check4Obstacles(aux1,aux2)){
                        if(n<=0){
                            n = -(n-1);
                        } else {
                            n = -(n+1);
                        }
                        
                        aux1[0]+=n;
                        aux2[0]+=n;
                    }
                    
                    planning(punto1,aux2,xroute,yroute,len);
                } else {
                    aux1[0]=punto2[0];
                    aux1[1]=punto2[1];
                    aux2[0]=p1[0];
                    aux2[1]=p1[1];
                    while(check4Obstacles(aux1,aux2)){
                        if(n<=0){
                            n = -(n-1);
                        } else {
                            n = -(n+1);
                        }
                        
                        aux1[1]+=n;
                        aux2[1]+=n;
                    }
                    
                    planning(punto1,aux2,xroute,yroute,len);
                }
            }
        }
        
    inserta(xroute,len,punto2[0],*len);
    inserta(yroute,len,punto2[1],*len);
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
