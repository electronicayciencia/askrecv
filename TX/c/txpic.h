#include <12F683.h>

#FUSES PUT                   	//Power Up Timer
#FUSES NOMCLR                  	//Master Clear pin disabled

#use delay(crystal=20MHz)

#define SYMB_TIME_US	1000
//#define BAUDS 1000
//#define BITS     8
//#define PARITY   O
//#define STOP     1


#define PWM_PIN			PIN_A2

