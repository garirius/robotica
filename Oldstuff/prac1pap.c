#include <wiringPi.h>
#include <stdio.h>
#include <softPwm.h>

#define PAP_CLK 4
#define PAP_SG 5
#define DIENTES 47

int main(){
    int ang=0,howmuch,dif;
    
    wiringPiSetup();
	pinMode(PAP_CLK,OUTPUT);
	pinMode(PAP_SG,OUTPUT);
	softPwmCreate(PAP_CLK, 0, 100);


    printf("¿A cuántos grados quieres ponerte?\n");
    while(){
        scanf("%d",&howmuch);
        dif = howmuch-ang;
        
        softPwmWrite(PAP_CLK,150); //activamos el PWM
        delay(20*dif*DIENTES/360);//esperamos una cantidad de tiempo adecuada (20ms*nº periodos necesarios)
        softPwmWrite(PAP_CLK,0); //apagamos el PWM
        
        ang = howmuch;
        printf("\n Maravilloso. ¿A cuántos grados quieres ponerte ahora?\n");
    }
}