/* DeCodifica señales en GPIO como si fuera una UART 
 *
 * gcc -lm -lwiringPi -o iobits iobits.c
 *
 * Electrónica y Ciencia. http://electronicayciencia.blogspot.com.es/
 *
 * Reinoso G.  02/03/2018
 *
 */

#include <stdio.h>
#include <wiringPi.h>
#include <sys/time.h>
#include <stdlib.h>

#define bauds 1000
#define inpin 25

/* Wait for the specified condition in input pin
 * If timeout is reached, returns -1 
 * This is not the correct way to do it. Due to multitasking 
 * SO delays, the timeout is barely approximate. */
int wait_for(int v, unsigned int timeout_us) {
	int lap;
	int max_laps = timeout_us / 100;

	for (lap = 0; lap < max_laps; lap++) {
		if (digitalRead(inpin) == v) 
			return 0;

		delayMicroseconds(100);
	}

	return -1;
}

/* Read a 8 bit byte using 8O1.
 * Return '?' if parity error
 * Return 0xFF if break condition */
char read_byte() {
	printf("\nWaiting for start bit... ");
	while (wait_for(0,1e6) == -1);
	printf("Got It!\n");

	delayMicroseconds(1e6 / bauds / 4);
	printf("Reading 8 bits LSF: ");

	char c = 0;
	int  p = 0; // parity odd: p = 1 at the end
	int  i;
	for (i = 0; i < 8; i++) {
		delayMicroseconds(1e6 / bauds);
		int v = digitalRead(inpin);
		printf("%d", v);
		c >>= 1;
		c |= (v<<7);
		p = p ^ v;
	}

	printf("\nReading parity bit... ");
	delayMicroseconds(1e6 / bauds);
	int v = digitalRead(inpin);
	p = p ^ v;
	printf("%d:  %s\n", v, p ? "Right!" : "Wrong!");

	printf("Waiting for stop bit... ");
	if (wait_for(1, 2*1e6/bauds) == -1) {
		printf("Break condition!\n");
		return 0xff;
	}
	printf("Got it!\n");

	if (p) {
		printf("Read: 0x%02x\n", c);
		return c;
	}
	else {
		printf("Parity error\n", c);
		return '?';
	}
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

	while (1) {
		printf("Waiting for line up... ");
		if (wait_for(1, 15e6) == -1) {
			printf("NO CARRIER\n");
			exit(0);
		}
		printf("Line!\n");
		
		char c;
		while ( (c = read_byte()) != 0xff)
			fprintf(stderr, "%c", c);
	}
}

