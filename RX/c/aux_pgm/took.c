/* DeCodifica señales en GPIO como un stream de 0s y 1s 
 *
 * gcc -lm -lwiringPi -o took took.c
 *
 * Electrónica y Ciencia. http://electronicayciencia.blogspot.com.es/
 *
 * Reinoso G.  02/03/2018
 *
 */

#include <stdio.h>
#include <wiringPi.h>
#include <sys/time.h>
#include <math.h>

#define bauds 1000
#define inpin 25

float meanerr = 0;

struct timeval tlast;

static void isr_srv(void) {
	struct timeval tnow;
	gettimeofday(&tnow, NULL);

	unsigned int tdiff = 1e6 * (tnow.tv_sec - tlast.tv_sec)
	                         +  tnow.tv_usec - tlast.tv_usec;

	float symbs = tdiff / (1e6/bauds);
	float err   = tdiff - round(symbs)*(1.0e6/bauds);
	meanerr = 0.8*meanerr + 0.2*sqrt(err*err); // + and - should compensate
	fprintf(stderr, "Took %5uus (%4.1f symbs) -> Err: % 5.0f\n", tdiff, symbs, meanerr);


	// Ignore spurious transition
	if (symbs < 0.5) 
		return;

	tlast = tnow;

	// Max 15 equal bits in a row
//	if (symbs > 0.5 && symbs < 15) {
		int v = !digitalRead(inpin); // value that WAS, not that IS
		int i;
		for (i = round(symbs); i > 0; i--)
			putchar(v + 0x30); //poor man's itoa
//	}
}


int main(void) {
	char gpiocmd[32];

	if (wiringPiSetup () == -1)
		return 1;

	fprintf(stderr, 
			"Receiver pin %d. Speed: %d bauds.\n", 
			inpin, bauds);

	piHiPri(99);
	pinMode(inpin, INPUT);

	setbuf(stdout, NULL);
	gettimeofday(&tlast, NULL);
	wiringPiISR(inpin, INT_EDGE_BOTH, &isr_srv);

	while (1) {
		delay(1000);
	};
}
