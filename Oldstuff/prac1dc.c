#include <wiringPi.h>
#include <stdio.h>
#define DC_EN 1
#define DC_A 2
#define DC_B 3

int main(){
	
	int giro=-1;
	
	pinMode(DC_EN,OUTPUT);
	pinMode(DC_A,OUTPUT);
	pinMode(DC_B,OUTPUT);

	printf("Escoja sentido de giro: izquierda (0) o derecha (1)\n");
	while(giro<0 || giro>1){
		scanf("%d",&giro);

		if(giro<0 || giro>1){
			printf("Hágame usted el favor de introducir 0 ó 1.\n\n");
		}
	}
    
    digitalWrite(DC_EN,HIGH);
    if(giro==0) {
				digitalWrite(DC_A,LOW);
				digitalWrite(DC_B,HIGH);
    } else if(giro==1){
				digitalWrite(DC_A,HIGH);
				digitalWrite(DC_B,LOW);
    }
}
