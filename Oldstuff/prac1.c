#include <wiringPi.h>
#include <stdio.h>
#define DC_EN 1
#define DC_A 2
#define DC_B 3
#define PAP_CLK 4
#define PAP_SG 5

int main(){
	
	int motor=-1;
	int giro=-1;
	
	pinMode(DC_EN,OUTPUT);
	pinMode(DC_A,OUTPUT);
	pinMode(DC_B,OUTPUT);
	pinMode(PAP_CLK,PWM_OUTPUT);
	pinMode(PAP_SG,OUTPUT);
	pwmSetMode(PWM_MODE_MS);
	pwmSetRange(2000);	
	pwmSetClock(192);
	

	printf("Elija el motor a operar: motor DC (0) ó motor PaP (1)\n");
	while(motor<0 || motor>1){
		scanf("%d",&motor);

		if(motor<0 || motor>1){
			printf("Hágame usted el favor de introducir 0 ó 1.\n\n");
		}
	}

	printf("Escoja sentido de giro: izquierda (0) o derecha (1)\n");
	while(giro<0 || giro>1){
		scanf("%d",&giro);

		if(giro<0 || giro>1){
			printf("Hágame usted el favor de introducir 0 ó 1.\n\n");
		}
	}

	switch(motor){
		case 0: //MOTOR DC
			digitalWrite(DC_EN,HIGH);
			pwmWrite(PAP_CLK,0);

			if(giro==0) {
				digitalWrite(DC_A,LOW);
				digitalWrite(DC_B,HIGH);
			} else if(giro==1){
				digitalWrite(DC_A,HIGH);
				digitalWrite(DC_B,LOW);
			}
			break;
		case 1: //MOTOR PAP
			digitalWrite(DC_EN,LOW);
			if(giro==0) { //queda ver a qué sentido corresponde LOW y HIGH
				digitalWrite(PAP_SG,LOW);
			} else if(giro==1){
				digitalWrite(PAP_SG,HIGH);
			}
			pwmWrite(PAP_CLK,150);
			break;
		default:
			printf("Qué haces con tu vida\n");
	}
}
