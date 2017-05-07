/* Definimos todas las especificaciones del robot */
#define PINR 0 // Pin para la rueda derecha
#define PINL 1 // Pin para la rueda izquierda
#define PERIIZQ 0.22 //Perímetro (en m) de las ruedas.

//Qué valores pasar al PWM y demás cosis.
#define FWL 20
#define FWR 5
#define BKL 10
#define BKR 25

//Vueltas/segundo de cada rueda
#define FWLV 0.860585
#define FWRV 0.8365
#define BKLV 0.9208
#define BKRV 0.8421

//Vueltas/segundo del giro global
#define SPIN 0.326 //rev/s
//Velocidad (grosso modo) de avance del robot.
#define SPEED 0.174 //m/s
//s/vuelta pivotando
#define PIVL 7.52
#define PIVR 7.2

void startCounting();
/* A continuación se escribe la función principal */
//Va hacia delante.
void gofw(int cm);
//Para.
void stop();
//Se pone a girar en una dirección (IZQ ó DCH)
void turn(int tow);
//Pivota un cierto ángulo
void pivot(float ang);
PI_THREAD(stable);
void motoresSetup();