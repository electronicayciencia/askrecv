/* Support file for ultrasonic ASK receiver.
 *
 * Reinoso G.  04/03/2018
 *
 * http://electronicayciencia.blogspot.com
 */

#include <txpic.h>

void set_line(int1 v) {
	if (v)
		output_high(PWM_PIN);
	else
		input(PWM_PIN);
}

void tx_bit(int1 v) {
	set_line(v);
	delay_us(SYMB_TIME_US);	
}

/* Transmit a byte:
 - Set line down, to tx start bit
 - Wait 1 symbol time
 - Repeat for BITS
   - Set line properly to tx next values
   - Wait 1 symbol time between them
 - Set line to paritiy value
 - Wait
 - Set line high to tx stop byte
 - Wait 1 symbol time  */ 
void tx_byte(char b) {
	#bit lsb = b.0;
	int1 p = 1; // odd parity
	
	tx_bit(0);

	char i = 0; // counter	
	while (i < 8) {
		tx_bit(lsb);
		p = p ^ lsb;
		b = b >> 1;
		
		i++;
	}
	
	tx_bit(p);
	tx_bit(1);
}

void main()
{	
	const char message[] = 
	"Prueba de comunicacion serie por ultrasonidos.\n";
	
	//Configure PWM output
	// 40kHz 50% PWM config @20MHz
	setup_timer_2(T2_DIV_BY_1,124,1);
	set_pwm1_duty((int16)248);
	
	// Line UP
	setup_ccp1(CCP_PWM);
	delay_ms(500);

	while(TRUE)	{
		int8 i = 0;

		while (message[i]) {
			tx_byte(message[i]);
			i++;
		}
		
		delay_ms(100);
	}
}
