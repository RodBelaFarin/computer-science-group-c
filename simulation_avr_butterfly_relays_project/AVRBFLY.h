#ifndef ACUAS_H
#define ACUAS_H
/**
//  File........: AVRBFLY.h
//
//  Author(s)...: Group02: E.Escobar, R.Gutierrez, Spanda, Mitul
//
//  Target(s)...: ATmega169 
//
//  Compiler....: AVR Studio
//
//  Description.: AVR Butterfly Macros header file distributor 
//
//  Revisions...: 1.0
//
//  YYYYMMDD - VER. - COMMENT    - SIGN.
//
//  20120620 - 1.0  - Created    - KS
//
*/


/**
 @defgroup ACUAS_EP ACUAS Easy Programming

 @brief C macros to easily program the ACUAS ATmega16/ATmega32 board.
 @author Thomas Siepmann, Aachen University of Applied Sciences, Germany
 @see http://www.siepmann.fh-aachen.de
 */

#ifndef F_CPU
/// default processor speed (used for WAIT_SEC and UART)
#define F_CPU 200000UL
#endif

#include <avr/io.h>
#include <stdlib.h>

void set_seed(void);
void InitBoard(void);
void delay_ds(unsigned char ds);
void autoADCps(void);
void InitBuzzer(int f, int d);

/// Begin of instructions which are carried out only one time at the beginning of the program
#define PROGRAM_INIT   int main(void){InitBoard();
/// Begin of instructions which are carried out in an endless loop.
#define PROGRAM_START  while(1){
/// End of the program
#define PROGRAM_END	 } return 0;}

/**
 *  @name  Define Variables
 */
/// Declares a 32 bit integer variable with a range from - 2147483648 to + 2147483648
#define VARIABLE(v) signed long int v;
/// Declares a 16 bit unsigned variable with a range from 0 to 65535
#define VAR16(v) uint16_t v;
/// Declares a 8 bit unsigned variable with a range from 0 to 255
#define VAR(v) unsigned char v;
/// Declares a character array s which can contain a string of max characters
#define STRING(s,max) unsigned char s[(max)+1];
/// Declares a CONSTANT character array s which contains the string string
#define STRING_CONST(s,string) static const char s[] PROGMEM = string;

/**
 *  @name  Miscellaneous
 */
/// Empty instruction. Continues immediately with the next instruction. Can be used for empty loops or before else conditions. Consumes no time.
#define DO_NOTHING ;
/// Waits x seconds. Range of x i from 0.1 to 25
#define WAIT_SEC(x) delay_ds(x*10);
/// Sets the bit number b to 1. Ex.: SET_BIT(PORTC, 7)
#define SET_BIT(r,b) (r) |= 1 << (b);
/// Sets the bit number b to 0. Ex.: CLEAR_BIT(PORTC, 7)
#define CLEAR_BIT(r,b) (r) &= ~(1 << (b));
/// Initialize the random generator. Must be invoked in the PROGRAM_INIT part. The seed is derived from the SRAM initial values at startup time.
#define RANDOM_INIT set_seed();
/**
 *  @brief   Generates a random integer number
 *  @param   n Minimal random integer value
 *  @param   m Maximal random integer value
 *  @return  random number between n and m (including)
 */
#define RANDOM(n,m)(n + random()%(m-n+1))

/**
 *  @name  Joystick center
 */
///Execute the following instruction if the joystick center button is pressed
#define ON_JOYSTICK_CENTER     if(!(PINB & (1 << 4)))
///Execute the following instruction if the joystick center button is not pressed
#define OFF_JOYSTICK_CENTER      if(PINB & (1 << 4))
///Wait until the joystick center button is pressed
#define WAIT_FOR_JOYSTICK_CENTER while(PINB & (1 << 4));
///Wait until the joystick center button is released
#define WAIT_FOR_RELEASE_JOYSTICK_CENTER while(!(PINB & (1 << 4)));

/**
 *  @name  Joystick up
 */
///Execute the following instruction if the joystick up button is pressed
#define ON_JOYSTICK_UP     if(!(PINB & (1 << 6)))
///Execute the following instruction if the joystick up button is not pressed
#define OFF_JOYSTICK_UP      if(PINB & (1 << 6))
///Wait until the joystick up button is pressed
#define WAIT_FOR_JOYSTICK_UP while(PINB & (1 << 6));
///Wait until the joystick up button is released
#define WAIT_FOR_RELEASE_JOYSTICK_UP while(!(PINB & (1 << 6)));
/**
 *  @name  Joystick down
 */
///Execute the following instruction if the joystick down button is pressed
#define ON_JOYSTICK_DOWN   if(!(PINB & (1 << 7)))
///Execute the following instruction if the joystick down button is not pressed
#define OFF_JOYSTICK_DOWN    if(PINB & (1 << 7))
///Wait until the joystick down button is pressed
#define WAIT_FOR_JOYSTICK_DOWN while(PINB & (1 << 7));
///Wait until the joystick down button is released
#define WAIT_FOR_RELEASE_JOYSTICK_DOWN while(!(PINB & (1 << 7)));
/**
 *  @name  Joystick right
 */
///Execute the following instruction if the joystick right button is pressed
#define ON_JOYSTICK_RIGHT  if(!(PINE & (1 << 3)))
///Execute the following instruction if the joystick right button is not pressed
#define OFF_JOYSTICK_RIGHT   if(PINE & (1 << 3))
///Wait until the joystick right button is pressed
#define WAIT_FOR_JOYSTICK_RIGHT while(PINE & (1 << 3));
///Wait until the joystick right button is released
#define WAIT_FOR_RELEASE_JOYSTICK_RIGHT while(!(PINE & (1 << 3)));
/**
 *  @name  Joystick left
 */
///Execute the following instruction if the joystick left button is pressed
#define ON_JOYSTICK_LEFT   if(!(PINE & (1 << 2)))
///Execute the following instruction if the joystick left button is not pressed
#define OFF_JOYSTICK_LEFT    if(PINE & (1 << 2))
///Wait until the joystick left button is pressed
#define WAIT_FOR_JOYSTICK_LEFT while(PINE & (1 << 2));
///Wait until the joystick left button is released
#define WAIT_FOR_RELEASE_JOYSTICK_LEFT while(!(PINE & (1 << 2)));

/**
 *  @name  AD converter
 *  Channel numbering is 1 based!
 */
  /// Activate ADC with Prescaler 16 --> 1Mhz/16 = 62.5kHz. ADEN=AD enable, ADPSx automatic preselection. Reference voltage = AVCC
#define ACTIVATE_ADC  {ADCSRA = (1<<ADEN); ADMUX |= (0<<REFS1) | (1<<REFS0); autoADCps();}
  /// Select channel. Channel numbering is 1 based
#define ADC_CHANNEL(ch) {ADMUX &= 0b11110000; ADMUX |= (ch-1); PORTF &= ~(1 << (ch-1));}
  /// Start one 10 bit AD conversion. Wait until conversion completed
#define START_ADC {ADCSRA |= (1<<ADSC); loop_until_bit_is_clear(ADCSRA,ADSC);}
/// Perform a 10 bit AD conversion and store the value in a 8 bit variable
#define ADCONVERTlow(x) {ADCSRA |= (1<<ADSC); loop_until_bit_is_clear(ADCSRA,ADSC);ADCSRA |= (1<<ADSC); loop_until_bit_is_clear(ADCSRA,ADSC);x=ADCW >> 2;}
/// Perform  a 10 bit AD conversion and store the digital value in a 16 bit variable
#define ADCONVERT(x) {ADCSRA |= (1<<ADSC); loop_until_bit_is_clear(ADCSRA,ADSC);ADCSRA |= (1<<ADSC); loop_until_bit_is_clear(ADCSRA,ADSC);x=ADCW;}
/// Perform  a 10 bit AD conversion and store the result in mV in an integer (or double or float) variable
#define ADCONVERT_MV(x) {ADCSRA |= (1<<ADSC); loop_until_bit_is_clear(ADCSRA,ADSC);ADCSRA |= (1<<ADSC); loop_until_bit_is_clear(ADCSRA,ADSC);x=ADCW*5./1.024;}

/**
 *  @name  Sensors
 */
/// Perform a 10 bit AD conversion from the value of the brightnes sensor and store the value in a 8 bit variable
#define BRIGHTNESS(x) {ACTIVATE_ADC ADC_CHANNEL(3) ADCONVERTlow(x)}
/// Perform a 10 bit AD conversion from the value of the V_in pin and store the result in V in an integer (or double or float) variable
#define VOLTAGE(x) {ACTIVATE_ADC ADC_CHANNEL(2) ADCSRA |= (1<<ADSC); loop_until_bit_is_clear(ADCSRA,ADSC);ADCSRA |= (1<<ADSC); loop_until_bit_is_clear(ADCSRA,ADSC);x=ADCW*5./1023.0;}
/// Perform a 10 bit AD conversion from the value of the temperature sensor and store the result in °C in an integer (or double or float) variable
#define TEMPERATURE(x) {ACTIVATE_ADC ADC_CHANNEL(1) ADCSRA |= (1<<ADSC); loop_until_bit_is_clear(ADCSRA,ADSC);ADCSRA |= (1<<ADSC); loop_until_bit_is_clear(ADCSRA,ADSC); int adc=ADCW; float t=adc/(1024-adc); float r=log(t); x=4250/(r+14.2617)-273;}

/**
 *  @name  Buzzer
 */
///	The volume must be between a value of 0 to 100, this macro must be used before the BEEP macro to allow the sound be heard
#define BEEP_VOLUME(v) {OCR1AH = 0; OCR1AL = v;}
///	The tone is a vale between 20 and 20000 in Hz (the "audible" frequencies), duration in seconds in values between 0.1 and 25
#define BEEP(tone,duration) {InitBuzzer(tone,duration);}


/*@}*/
#endif // ACUAS_H
