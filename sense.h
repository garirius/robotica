#define SPIOUT 13
#define UMBRAL 620
#define FRANJAS 16
#define LUT_SIZE 17

//lee el sens-ésimo sensor del encoder (0 ó 1)
char leeSens(int sens);
//lee el sens-ésimo sensor del encoder (2 ó 3)
float leeIR(int sens);
void sensoresSetup();
