/// @defgroup ACUAS AVRBFLY Functions
/// @{

/// @file ACUAS.c

#include "AVRBFLY.h"

/// Set seed for random number generation
/// source: http://www.roboternetz.de/wissen/index.php/Zufallszahlen_mit_avr-gcc
void set_seed(void)
{
#ifndef SIMULATION
	unsigned short seed = 0, *p = (unsigned short*) (RAMEND+1);
	extern unsigned short __heap_start;
	while (p >= &__heap_start + 1)
		seed ^= * (--p);
	srandom((unsigned long) seed);
#else
	srandom((unsigned long) 1);
#endif
}


/// Initialize the ports of the controller
void InitBoard(void)
{

/// Inputs: PB4(center), PB6(up), PB7(down), PE2(left) and PE3(right)
	DDRB	= DDRB	& 0b00101111;
	DDRE	= DDRE	& 0b11110011;
/// switch on pull up resistors
	PORTB	= PORTB	| 0b11010000;
	PORTE	= PORTE	| 0b00001100;
/// no button pressed
	PINB	= PINB	| 0b11010000;
	PINE	= PINE	| 0b00001100;
/// Output: LEDs at port C
	DDRC  = 0b11111111;
/// switch off all LEDs
	PORTC = 0b11111111;
/// initialize the random number generator
    set_seed();
/// Output for buzzer on PORT B
	DDRB	= DDRB	| 0b00100000;
//	PORTB	= PORTB	| 0b00100000;
}

/// Wait ds tenths of a second
void delay_ds(unsigned char ds)
{
#ifndef SIMULATION
	uint16_t i, lim= 10*ds;
	for(i=0; i<lim; i++)
	{
		uint16_t __ticks = F_CPU / 400;
		__asm__ volatile (
			"1: sbiw %0,1" "\n\t"
			"brne 1b"
			: "=w" (__ticks)
			: "0" (__ticks)
		);
	}
#endif
}

void autoADCps(void)
{
/// new 03.05.2012  Prescaler setting depending from F_CPU
   /* 
  F_CPU           PS2
  < 0.4 MHz ->    2 ->    ...200 kHz
  >0.4 MHz -  0.8 MHz ->    4 -> 100...200 kHz
  >0.8 MHz -  1.6 MHz ->    8 -> 100...200 kHz
  >1,6 MHz -  3.2 MHz ->   16 -> 100...200 kHz
  >3.2 MHz -  6.4 MHz ->   32 -> 100...200 kHz
  >6.4 MHz - 12.8 MHz ->   64 -> 100...200 kHz
  >12.8 MHz           ->  128 -> 100...    kHz
    
  Simulationsergebnisse in AVR Studio:
1688 Zyklen  @ 16Mhz = 1688/16Mio = 0,000106 sec 
 848 Zyklen  @  8Mhz =  848/ 8Mio = 0,000106 sec
 128 Zyklen  @  1MHz =  128/ 1Mio = 0,000128 sec

Auf ACUAS Board gemessen:
1MHz, 4MHz, 16MHz
bei gleicher Spannung an PINA7: unveränderter Digitalwert

  */
  uint8_t ps012;
  /// Assures the ADC clock to be between 100 and 200 kHz
    if      (F_CPU <=   400000UL) ps012 = 0b00000001; //001 = PS   2
    else if (F_CPU <=   800000UL) ps012 = 0b00000010; //010 = PS   4
    else if (F_CPU <=  1600000UL) ps012 = 0b00000011; //011 = PS   8
    else if (F_CPU <=  3200000UL) ps012 = 0b00000100; //100 = PS  16
    else if (F_CPU <=  6400000UL) ps012 = 0b00000101; //101 = PS  32
    else if (F_CPU <= 12800000UL) ps012 = 0b00000110; //110 = PS  64
    else if (F_CPU >  12800000UL) ps012 = 0b00000111; //111 = PS 128

  	ADCSRA = ADCSRA & 0b11111000; // set bits PS0, PS1, PS2 to 0
    ADCSRA = ADCSRA | ps012;      // set prescaler bits
}

void InitBuzzer(int f, int d)
{
//	char Volume=50;
	int icr1;
	icr1 = 1000000/(2*f);
	ICR1 = icr1;				// Top value of the Timer 1
	TCCR1A = (1<<COM1A1);		// Set OC1A when upcounting, clear when downcounting
    TCCR1B = (1<<WGM13);        // Phase/Freq-correct PWM, top value = ICR1
	SET_BIT(TCCR1B, CS10)		// Start Timer1, prescaler(1)    
	WAIT_SEC(d)
	CLEAR_BIT(TCCR1B, CS10)		// Stops Timer 1
}

/// @}
//#endif // ACUAS_C
