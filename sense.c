#include <wiringPi.h>
#include <stdio.h>

#define SPIOUT 13
#define UMBRAL 620
#define FRANJAS 16
#define LUT_SIZE 17

static unsigned char dist[LUT_SIZE] = {3, 4, 5, 6, 7, 8, 9, 10, 12, 14, 16, 18, 20, 25, 30, 35, 40};
static float volt[LUT_SIZE] = {3.0348, 2.7261, 2.3478, 2.0347, 1.7826, 1.5869, 1.4, 1.2956, 1.0913, 0.9608, 0.8478, 0.7739, 0.6565, 0.5217, 0.4304, 0.3739, 0.313};

float dista[2];
//lee el sens-ésimo sensor del encoder (0 ó 1)
short leeSens(int sens){
    /*int x = analogRead(100+sens);
    if(x>UMBRAL){ //devolvemos blanco (1) o negro (0) en función de la lectura del sensor
        return 1;
    } else {
        return 0;
    }*/
    return analogRead(100+sens);
}

//lee el sens-ésimo sensor del encoder (2 ó 3)
float leeIR(int sens){
    
	short val = analogRead(100+sens);
	float v = 3.3*((float)val)/1024.0;

	// LEEMOS EL SENSOR IR
	float d=-1;
	int i=0,pos=0;
	float vol1, vol2, d1, d2;

    //Buscamos a qué posición del vector volt estamos
    while(v<volt[i] && i<LUT_SIZE){
        pos++;
        i++;
    }
    
    if(!(pos==0 || pos>=LUT_SIZE)){
        vol1 = volt[pos-1];
        vol2 = volt[pos];
        d1 = (float)dist[pos-1];
        d2 = (float)dist[pos];

        d = d1 + (v-vol1)*(vol2-vol1)/(d2-d1);
    }
        
    return d;
}

PI_THREAD(distance){ //va midiendo distancias
    while(1){
        dista[0] = leeIR(2);
        dista[1] = leeIR(3);
        delay(500);
    }
}

void sensoresSetup(){
    dista[0] = leeIR(2);
    dista[1] = leeIR(3);
    int x = piThreadCreate(distance);
}
