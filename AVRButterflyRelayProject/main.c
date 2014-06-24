#include <avr/io.h>
#include <inttypes.h>
#include <avr/interrupt.h>

#define F_CPU 8000000UL		// calibrated internal RC oscillator - DO NOT MODIFY
#include <util/delay.h>


#include "lookUpTable.h"	// header file with segment to character matching
#include "lcd.h"			// headter file with lcd related functions
#include "keypad.h"			// header file with keypad related functions

#define RELAY_1_0N	PORTB &= ~(1<<0)	// definition of relay 1 (device 1) on
#define RELAY_2_0N	PORTB &= ~(1<<1)	// definition of relay 2 (device 2) on

#define RELAY_1_0FF	PORTB |= (1<<0)		// definition of relay 1 (device 1) off
#define RELAY_2_0FF	PORTB |= (1<<1)		// definition of relay 2 (device 2) off

#define CENTRE_BUTTON_PRESSED (!(PINB &(1<<4)))
#define LEFT_BUTTON_PRESSED (!(PINE &(1<<2)))
#define RIGHT_BUTTON_PRESSED (!(PINE &(1<<3)))
#define UP_BUTTON_PRESSED (!(PINB &(1<<6)))
#define DOWN_BUTTON_PRESSED (!(PINB &(1<<7)))

/*

B0 = RELAY 1
B1 = RELAY 2

E4 = KEYPAD COLUMN 1
E5 = KEYPAD COLUMN 2
E6 = KEYPAD COLUMN 3

F4 = KEYPAD ROW 1
F5 = KEYPAD ROW 2
F6 = KEYPAD ROW 3
F7 = KEYPAD ROW 4

B4 = JOYSTICK CENTRE
B6 = JOYSTICK UP
B7 = JOYSTICK DOWN
E2 = JOYSTICK LEFT
E3 = JOYSTICK RIGHT

*/



// ------------------ GLOBAL VARIABLES --------------------

uint8_t sec_unit = 0;					// real time now variables
uint8_t sec_tens = 0;
uint8_t min_unit = 0;
uint8_t min_tens = 0;
uint8_t hr_unit = 0;
uint8_t hr_tens = 0;

uint8_t div1_on_min_unit = 0;			// device 1 on at time
uint8_t div1_on_min_tens = 0;
uint8_t div1_on_hr_unit = 0;
uint8_t div1_on_hr_tens = 0;

uint8_t div1_off_min_unit = 0;			// device 1 off at time
uint8_t div1_off_min_tens = 0;
uint8_t div1_off_hr_unit = 0;
uint8_t div1_off_hr_tens = 0;

uint8_t div1_Cdown_sec_unit = 0;		// device 1 countdown time
uint8_t div1_Cdown_sec_tens = 0;
uint8_t div1_Cdown_min_unit = 0;
uint8_t div1_Cdown_min_tens = 0;
uint8_t div1_Cdown_hr_unit = 0;
uint8_t div1_Cdown_hr_tens = 0;

uint8_t div2_on_min_unit = 0;			// device 2 on at time
uint8_t div2_on_min_tens = 0;
uint8_t div2_on_hr_unit = 0;
uint8_t div2_on_hr_tens = 0;

uint8_t div2_off_min_unit = 0;			// device 2 on at time
uint8_t div2_off_min_tens = 0;
uint8_t div2_off_hr_unit = 0;
uint8_t div2_off_hr_tens = 0;

uint8_t div2_Cdown_sec_unit = 0;		// device 2 countdown time
uint8_t div2_Cdown_sec_tens = 0;
uint8_t div2_Cdown_min_unit = 0;
uint8_t div2_Cdown_min_tens = 0;
uint8_t div2_Cdown_hr_unit = 0;
uint8_t div2_Cdown_hr_tens = 0;

uint8_t min_unit_temp = 0;				// temporary storage of user input
uint8_t min_tens_temp = 0;
uint8_t hr_unit_temp = 0;
uint8_t hr_tens_temp = 0;


uint8_t screenDispNum = 0;
uint8_t button_wait_time = 0;
uint8_t sub_display = 0;
uint8_t i = 0;
uint8_t relay_1_state = 0;	// 0 = nothing state, 1 = permanently off, 2 = permanently on
uint8_t relay_2_state = 0;	// 0 = nothing state, 1 = permanently off, 2 = permanently on
uint8_t flag1;				// flag used in Timer2 vector
uint8_t flag2;				// flag used in Timer2 vector
uint8_t clear_sub_disp;

uint8_t lockout1 = 0;		// flag used in Timer2 vector
uint8_t lockout2 = 0;		// flag used in Timer2 vector


// the below two functions check the real time now and campare it to the on/off time of the relays

void checkTimedRelayONE(uint8_t hr_tens, uint8_t hr_unit, uint8_t min_tens, uint8_t min_unit,
						uint8_t div1_on_hr_tens, uint8_t div1_on_hr_unit, uint8_t div1_on_min_tens, uint8_t div1_on_min_unit, 
						uint8_t div1_off_hr_tens, uint8_t div1_off_hr_unit, uint8_t div1_off_min_tens, uint8_t div1_off_min_unit);

void checkTimedRelayTWO(uint8_t hr_tens, uint8_t hr_unit, uint8_t min_tens, uint8_t min_unit,
						uint8_t div2_on_hr_tens, uint8_t div2_on_hr_unit, uint8_t div2_on_min_tens, uint8_t div2_on_min_unit, 
						uint8_t div2_off_hr_tens, uint8_t div2_off_hr_unit, uint8_t div2_off_min_tens, uint8_t div2_off_min_unit);



int main(void)
{

DDRB &= ~(1<<4) & (1<<6) & (1<<7);
PORTB |= (1<<4) | (1<<6) | (1<<7);
DDRE &= ~(1<<2) & (1<<3);
PORTE |= (1<<2) | (1<<3);			// joystick as input with pullup.


DDRB |= (1<<0) | (1<<1);	// relays set as output
RELAY_1_0FF;
RELAY_2_0FF;


// --------------------- INITIALIZE  TIMERS ----------------------

TCCR0A |= (1<<CS01);				// timer prescaller
TIMSK0 |= (1<<TOIE0);				// enable timer 0

TCCR1B |= (1<<CS11) | (1<<CS10);	// timer prescaller
TIMSK1 |= (1<<TOIE1);				// enable timer 1

ASSR |= (1<<AS2);					// allows timer 2 to use external crystal 32.768 kHz
TCCR2A |=(1<<CS22) | (1<<CS20);		// 128 prescaller
TIMSK2 |= (1<<TOIE2);				// enable timer 2

sei();

// ---------------------------------------------------------------


LCD_Init();
keypadInit();




while(1);

return 0;


}







ISR(TIMER0_OVF_vect)
{

button_pressed = 12;		// 12 is nothing ie no button pressed
keypad_Get_Pressed();

checkTimedRelayONE(hr_tens, hr_unit, min_tens, min_unit,
					div1_on_hr_tens, div1_on_hr_unit, div1_on_min_tens, div1_on_min_unit, 
					div1_off_hr_tens, div1_off_hr_unit, div1_off_min_tens, div1_off_min_unit);

checkTimedRelayTWO(hr_tens, hr_unit, min_tens, min_unit,
					div2_on_hr_tens, div2_on_hr_unit, div2_on_min_tens, div2_on_min_unit, 
					div2_off_hr_tens, div2_off_hr_unit, div2_off_min_tens, div2_off_min_unit);

}











ISR(TIMER1_OVF_vect)
{


if(relay_1_state == 1) RELAY_1_0FF;
if(relay_1_state == 2)
	{
	RELAY_1_0N; 
	LCDDR0 |= (1<<1); // arrow 1
	}


//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
////////////////////////  HOME  SCREEN ///////////////////////////
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

if(screenDispNum == 0)	// home screen
{
	Clear_LCD();
	Send_Character(1,hr_tens);
	Send_Character(2,hr_unit);
	Send_Character(3,min_tens);
	Send_Character(4,min_unit);
	Send_Character(5,sec_tens);
	Send_Character(6,sec_unit);

	LCDDR8 = (1<<0);		// colons
	LCDDR0 |= (1<<2); 		// small digit 1 symbol
	LCDDR0 |= (1<<6); 		// small digit 2 symbol

	if(CENTRE_BUTTON_PRESSED) button_wait_time++;
	if(RIGHT_BUTTON_PRESSED) button_wait_time++;
	if(LEFT_BUTTON_PRESSED) button_wait_time++;

	if(CENTRE_BUTTON_PRESSED && (button_wait_time == 5) )
	{
	screenDispNum = 1;
	button_wait_time = 0;
	}

	if(RIGHT_BUTTON_PRESSED && (button_wait_time == 3)) 
	{
	screenDispNum = 18;
	button_wait_time = 0;
	}

	if(LEFT_BUTTON_PRESSED && (button_wait_time == 3)) 
	{
	screenDispNum = 20;
	button_wait_time = 0;
	}

}


if(screenDispNum == 18)	// count down for dev 1
{
	Clear_LCD();
	Send_Character(1,div1_Cdown_hr_tens);
	Send_Character(2,div1_Cdown_hr_unit);
	Send_Character(3,div1_Cdown_min_tens);
	Send_Character(4,div1_Cdown_min_unit);
	Send_Character(5,div1_Cdown_sec_tens);
	Send_Character(6,div1_Cdown_sec_unit);
	LCDDR8 = (1<<0);		// colons
	LCDDR0 |= (1<<2); 		// small digit 1 symbol
	

	if(RIGHT_BUTTON_PRESSED) button_wait_time++;
	if(LEFT_BUTTON_PRESSED) button_wait_time++;
	if(DOWN_BUTTON_PRESSED) button_wait_time++;

	if(RIGHT_BUTTON_PRESSED && (button_wait_time == 3)) 
	{
	screenDispNum = 20;
	button_wait_time = 0;
	}

	if(LEFT_BUTTON_PRESSED && (button_wait_time == 3)) 
	{
	screenDispNum = 0;
	button_wait_time = 0;
	}

	if(DOWN_BUTTON_PRESSED && (button_wait_time == 3)) 
	{
	screenDispNum = 19;
	clear_sub_disp = 1;
	button_wait_time = 0;
	}

}


if(screenDispNum == 20)	// count down for dev 2
{
	Clear_LCD();
	Send_Character(1,div2_Cdown_hr_tens);
	Send_Character(2,div2_Cdown_hr_unit);
	Send_Character(3,div2_Cdown_min_tens);
	Send_Character(4,div2_Cdown_min_unit);
	Send_Character(5,div2_Cdown_sec_tens);
	Send_Character(6,div2_Cdown_sec_unit);
	LCDDR8 = (1<<0);		// colons
	LCDDR0 |= (1<<6); 		// small digit 2 symbol	


	if(RIGHT_BUTTON_PRESSED) button_wait_time++;
	if(LEFT_BUTTON_PRESSED) button_wait_time++;
	if(DOWN_BUTTON_PRESSED) button_wait_time++;

	if(RIGHT_BUTTON_PRESSED && (button_wait_time == 3)) 
	{
	screenDispNum = 0;
	clear_sub_disp = 2;
	button_wait_time = 0;
	}

	if(LEFT_BUTTON_PRESSED && (button_wait_time == 3)) 
	{
	screenDispNum = 18;
	button_wait_time = 0;
	}

	if(DOWN_BUTTON_PRESSED && (button_wait_time == 3)) 
	{
	screenDispNum = 19;
	clear_sub_disp = 2;
	button_wait_time = 0;
	}

}


//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
////////////////////////  MAIN MENU //////////////////////////////
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////


if(screenDispNum == 1)	// main menu SET T
{
	Clear_LCD();
	Send_Character(1,S);
	Send_Character(2,E);
	Send_Character(3,T);
	Send_Character(5,T);

	if(CENTRE_BUTTON_PRESSED) button_wait_time++;
	if(RIGHT_BUTTON_PRESSED) button_wait_time++;
	if(LEFT_BUTTON_PRESSED) button_wait_time++;
	if(DOWN_BUTTON_PRESSED) button_wait_time++;


	if(CENTRE_BUTTON_PRESSED && (button_wait_time == 5) )
	{
	screenDispNum = 0;
	button_wait_time = 0;
	}

	if(RIGHT_BUTTON_PRESSED && (button_wait_time == 3)) 
	{
	screenDispNum = 2;
	button_wait_time = 0;
	}

	if(LEFT_BUTTON_PRESSED && (button_wait_time == 3)) 
	{
	screenDispNum = 3;
	button_wait_time = 0;
	}

	if(DOWN_BUTTON_PRESSED && (button_wait_time == 3)) 
	{
	screenDispNum = 4;
	sub_display = 1;
	button_wait_time = 0;
	}

}


if(screenDispNum == 2)	// main menu DEV 1
{
	Clear_LCD();
	Send_Character(1,D);
	Send_Character(2,E);
	Send_Character(3,V);
	Send_Character(5,1);

	if(CENTRE_BUTTON_PRESSED) button_wait_time++;
	if(RIGHT_BUTTON_PRESSED) button_wait_time++;
	if(LEFT_BUTTON_PRESSED) button_wait_time++;
	if(DOWN_BUTTON_PRESSED) button_wait_time++;


	if(CENTRE_BUTTON_PRESSED && (button_wait_time == 5) )
	{
	screenDispNum = 0;
	button_wait_time = 0;
	}

	if(RIGHT_BUTTON_PRESSED && (button_wait_time == 3)) 
	{
	screenDispNum = 3;
	button_wait_time = 0;
	}

	if(LEFT_BUTTON_PRESSED && (button_wait_time == 3)) 
	{
	screenDispNum = 1;
	button_wait_time = 0;
	}

	if(DOWN_BUTTON_PRESSED && (button_wait_time == 3)) 
	{
	screenDispNum = 10;
	button_wait_time = 0;
	}

}

if(screenDispNum == 3)	// main menu DEV 2
{
	Clear_LCD();
	Send_Character(1,D);
	Send_Character(2,E);
	Send_Character(3,V);
	Send_Character(5,2);

	if(CENTRE_BUTTON_PRESSED) button_wait_time++;
	if(RIGHT_BUTTON_PRESSED) button_wait_time++;
	if(LEFT_BUTTON_PRESSED) button_wait_time++;
	if(DOWN_BUTTON_PRESSED) button_wait_time++;


	if(CENTRE_BUTTON_PRESSED && (button_wait_time == 5) )
	{
	screenDispNum = 0;
	button_wait_time = 0;
	}

	if(RIGHT_BUTTON_PRESSED && (button_wait_time == 3)) 
	{
	screenDispNum = 1;
	button_wait_time = 0;
	}

	if(LEFT_BUTTON_PRESSED && (button_wait_time == 3)) 
	{
	screenDispNum = 2;
	button_wait_time = 0;
	}

	if(DOWN_BUTTON_PRESSED && (button_wait_time == 3)) 
	{
	screenDispNum = 21;
	button_wait_time = 0;
	}


}


//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//////////////////  KEYPAD  INPUT  SCREEN ////////////////////////
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

if(screenDispNum == 4)	//  _ _ : _ _ : _ _
{
	Clear_LCD();
	Send_Character(1,UNDERSCORE);
	Send_Character(2,UNDERSCORE);
	Send_Character(3,UNDERSCORE);
	Send_Character(4,UNDERSCORE);
	Send_Character(5,UNDERSCORE);
	Send_Character(6,UNDERSCORE);
	LCDDR8 = (1<<0);			// colons

	if(sub_display == 2) LCDDR0 |= (1<<2); 		// small digit 1 symbol
	if(sub_display == 3) LCDDR0 |= (1<<2); 		// small digit 1 symbol
	if(sub_display == 4) LCDDR0 |= (1<<2); 		// small digit 1 symbol
	if(sub_display == 5) LCDDR0 |= (1<<2); 		// small digit 1 symbol
	if(sub_display == 6) LCDDR0 |= (1<<6); 		// small digit 2 symbol
	if(sub_display == 7) LCDDR0 |= (1<<6); 		// small digit 2 symbol
	if(sub_display == 8) LCDDR0 |= (1<<6); 		// small digit 2 symbol
	if(sub_display == 9) LCDDR0 |= (1<<6); 		// small digit 2 symbol

	if(CENTRE_BUTTON_PRESSED) button_wait_time++;
	if(UP_BUTTON_PRESSED) button_wait_time++;



	if(CENTRE_BUTTON_PRESSED && (button_wait_time == 5) )
	{
	screenDispNum = 0;
	button_wait_time = 0;
	}

	if(UP_BUTTON_PRESSED && (button_wait_time == 5) && (sub_display ==1))
	{
	screenDispNum = 1;
	button_wait_time = 0;
	}

	if(UP_BUTTON_PRESSED && (button_wait_time == 5) && (sub_display ==2))
	{
	screenDispNum = 12;
	button_wait_time = 0;
	}

	if(UP_BUTTON_PRESSED && (button_wait_time == 5) && (sub_display ==3))
	{
	screenDispNum = 13;
	button_wait_time = 0;
	}

	if(UP_BUTTON_PRESSED && (button_wait_time == 5) && (sub_display ==4))
	{
	screenDispNum = 16;
	button_wait_time = 0;
	}

	if(UP_BUTTON_PRESSED && (button_wait_time == 5) && (sub_display ==5))
	{
	screenDispNum = 17;
	button_wait_time = 0;
	}

	if(UP_BUTTON_PRESSED && (button_wait_time == 5) && (sub_display ==6))
	{
	screenDispNum = 23;
	button_wait_time = 0;
	}

	if(UP_BUTTON_PRESSED && (button_wait_time == 5) && (sub_display ==7))
	{
	screenDispNum = 24;
	button_wait_time = 0;
	}

	if(UP_BUTTON_PRESSED && (button_wait_time == 5) && (sub_display ==8))
	{
	screenDispNum = 27;
	button_wait_time = 0;
	}

	if(UP_BUTTON_PRESSED && (button_wait_time == 5) && (sub_display ==9))
	{
	screenDispNum = 28;
	button_wait_time = 0;
	}

	
	if(button_pressed <3)
	{
	hr_tens_temp = button_pressed;
	button_pressed = 12;		// 12 is nothing ie no button pressed
	screenDispNum = 5;
	}

	if(button_pressed == 10)
	{
	button_pressed = 12;		// 12 is nothing ie no button pressed
	screenDispNum = 4;
	}

}

if(screenDispNum == 5)	//  H _ : _ _ : _ _
{

	Clear_LCD();
	Send_Character(1,hr_tens_temp);
	Send_Character(2,UNDERSCORE);
	Send_Character(3,UNDERSCORE);
	Send_Character(4,UNDERSCORE);
	Send_Character(5,0);
	Send_Character(6,0);
	LCDDR8 = (1<<0);							// colons
	if(sub_display == 2) LCDDR0 |= (1<<2); 		// small digit 1 symbol
	if(sub_display == 3) LCDDR0 |= (1<<2); 		// small digit 1 symbol
	if(sub_display == 4) LCDDR0 |= (1<<2); 		// small digit 1 symbol
	if(sub_display == 5) LCDDR0 |= (1<<2); 		// small digit 1 symbol
	if(sub_display == 6) LCDDR0 |= (1<<6); 		// small digit 2 symbol
	if(sub_display == 7) LCDDR0 |= (1<<6); 		// small digit 2 symbol
	if(sub_display == 8) LCDDR0 |= (1<<6); 		// small digit 2 symbol
	if(sub_display == 9) LCDDR0 |= (1<<6); 		// small digit 2 symbol

	if(CENTRE_BUTTON_PRESSED) button_wait_time++;
	if(UP_BUTTON_PRESSED) button_wait_time++;



	if(CENTRE_BUTTON_PRESSED && (button_wait_time == 5) )
	{
	screenDispNum = 0;
	button_wait_time = 0;
	}

	if(UP_BUTTON_PRESSED && (button_wait_time == 5) && (sub_display ==1))
	{
	screenDispNum = 1;
	button_wait_time = 0;
	}

	if(UP_BUTTON_PRESSED && (button_wait_time == 5) && (sub_display ==2))
	{
	screenDispNum = 12;
	button_wait_time = 0;
	}

	if(UP_BUTTON_PRESSED && (button_wait_time == 5) && (sub_display ==3))
	{
	screenDispNum = 13;
	button_wait_time = 0;
	}

	if(UP_BUTTON_PRESSED && (button_wait_time == 5) && (sub_display ==4))
	{
	screenDispNum = 16;
	button_wait_time = 0;
	}

	if(UP_BUTTON_PRESSED && (button_wait_time == 5) && (sub_display ==5))
	{
	screenDispNum = 17;
	button_wait_time = 0;
	}

	if(UP_BUTTON_PRESSED && (button_wait_time == 5) && (sub_display ==6))
	{
	screenDispNum = 23;
	button_wait_time = 0;
	}

	if(UP_BUTTON_PRESSED && (button_wait_time == 5) && (sub_display ==7))
	{
	screenDispNum = 24;
	button_wait_time = 0;
	}

	if(UP_BUTTON_PRESSED && (button_wait_time == 5) && (sub_display ==8))
	{
	screenDispNum = 27;
	button_wait_time = 0;
	}

	if(UP_BUTTON_PRESSED && (button_wait_time == 5) && (sub_display ==9))
	{
	screenDispNum = 28;
	button_wait_time = 0;
	}

	
	if(hr_tens_temp <2)
	{
		if(button_pressed <10)
		{
		hr_unit_temp = button_pressed;
		button_pressed = 12;		// 12 is nothing ie no button pressed
		screenDispNum = 6;
		}
	}

	if(hr_tens_temp == 2)
	{
		if(button_pressed <4)
		{
		hr_unit_temp = button_pressed;
		button_pressed = 12;		// 12 is nothing ie no button pressed
		screenDispNum = 6;
		}
	}



	if(button_pressed == 10)
	{
	button_pressed = 12;		// 12 is nothing ie no button pressed
	screenDispNum = 4;
	}

}

if(screenDispNum == 6)	//  H H : _ _ : _ _
{
	Clear_LCD();
	Send_Character(1,hr_tens_temp);
	Send_Character(2,hr_unit_temp);
	Send_Character(3,UNDERSCORE);
	Send_Character(4,UNDERSCORE);
	Send_Character(5,0);
	Send_Character(6,0);
	LCDDR8 = (1<<0);							// colons
	if(sub_display == 2) LCDDR0 |= (1<<2); 		// small digit 1 symbol
	if(sub_display == 3) LCDDR0 |= (1<<2); 		// small digit 1 symbol
	if(sub_display == 4) LCDDR0 |= (1<<2); 		// small digit 1 symbol
	if(sub_display == 5) LCDDR0 |= (1<<2); 		// small digit 1 symbol
	if(sub_display == 6) LCDDR0 |= (1<<6); 		// small digit 2 symbol
	if(sub_display == 7) LCDDR0 |= (1<<6); 		// small digit 2 symbol
	if(sub_display == 8) LCDDR0 |= (1<<6); 		// small digit 2 symbol
	if(sub_display == 9) LCDDR0 |= (1<<6); 		// small digit 2 symbol

	if(CENTRE_BUTTON_PRESSED) button_wait_time++;
	if(UP_BUTTON_PRESSED) button_wait_time++;



	if(CENTRE_BUTTON_PRESSED && (button_wait_time == 5) )
	{
	screenDispNum = 0;
	button_wait_time = 0;
	}

	if(UP_BUTTON_PRESSED && (button_wait_time == 5) && (sub_display ==1))
	{
	screenDispNum = 1;
	button_wait_time = 0;
	}

	if(UP_BUTTON_PRESSED && (button_wait_time == 5) && (sub_display ==2))
	{
	screenDispNum = 12;
	button_wait_time = 0;
	}

	if(UP_BUTTON_PRESSED && (button_wait_time == 5) && (sub_display ==3))
	{
	screenDispNum = 13;
	button_wait_time = 0;
	}

	if(UP_BUTTON_PRESSED && (button_wait_time == 5) && (sub_display ==4))
	{
	screenDispNum = 16;
	button_wait_time = 0;
	}

	if(UP_BUTTON_PRESSED && (button_wait_time == 5) && (sub_display ==5))
	{
	screenDispNum = 17;
	button_wait_time = 0;
	}
	
	if(UP_BUTTON_PRESSED && (button_wait_time == 5) && (sub_display ==6))
	{
	screenDispNum = 23;
	button_wait_time = 0;
	}

	if(UP_BUTTON_PRESSED && (button_wait_time == 5) && (sub_display ==7))
	{
	screenDispNum = 24;
	button_wait_time = 0;
	}

	if(UP_BUTTON_PRESSED && (button_wait_time == 5) && (sub_display ==8))
	{
	screenDispNum = 27;
	button_wait_time = 0;
	}

	if(UP_BUTTON_PRESSED && (button_wait_time == 5) && (sub_display ==9))
	{
	screenDispNum = 28;
	button_wait_time = 0;
	}


	if(button_pressed <6)
	{
	min_tens_temp = button_pressed;
	button_pressed = 12;		// 12 is nothing ie no button pressed
	screenDispNum = 7;
	}

	if(button_pressed == 10)
	{
	button_pressed = 12;		// 12 is nothing ie no button pressed
	screenDispNum = 4;
	}


}

if(screenDispNum == 7)	//  H H : M _ : _ _
{
	Clear_LCD();
	Send_Character(1,hr_tens_temp);
	Send_Character(2,hr_unit_temp);
	Send_Character(3,min_tens_temp);
	Send_Character(4,UNDERSCORE);
	Send_Character(5,0);
	Send_Character(6,0);
	LCDDR8 = (1<<0);							// colons
	if(sub_display == 2) LCDDR0 |= (1<<2); 		// small digit 1 symbol
	if(sub_display == 3) LCDDR0 |= (1<<2); 		// small digit 1 symbol
	if(sub_display == 4) LCDDR0 |= (1<<2); 		// small digit 1 symbol
	if(sub_display == 5) LCDDR0 |= (1<<2); 		// small digit 1 symbol
	if(sub_display == 6) LCDDR0 |= (1<<6); 		// small digit 2 symbol
	if(sub_display == 7) LCDDR0 |= (1<<6); 		// small digit 2 symbol
	if(sub_display == 8) LCDDR0 |= (1<<6); 		// small digit 2 symbol
	if(sub_display == 9) LCDDR0 |= (1<<6); 		// small digit 2 symbol


	if(CENTRE_BUTTON_PRESSED) button_wait_time++;
	if(UP_BUTTON_PRESSED) button_wait_time++;



	if(CENTRE_BUTTON_PRESSED && (button_wait_time == 5) )
	{
	screenDispNum = 0;
	button_wait_time = 0;
	}

	if(UP_BUTTON_PRESSED && (button_wait_time == 5) && (sub_display ==1))
	{
	screenDispNum = 1;
	button_wait_time = 0;
	}

	if(UP_BUTTON_PRESSED && (button_wait_time == 5) && (sub_display ==2))
	{
	screenDispNum = 12;
	button_wait_time = 0;
	}

	if(UP_BUTTON_PRESSED && (button_wait_time == 5) && (sub_display ==3))
	{
	screenDispNum = 13;
	button_wait_time = 0;
	}

	if(UP_BUTTON_PRESSED && (button_wait_time == 5) && (sub_display ==4))
	{
	screenDispNum = 16;
	button_wait_time = 0;
	}

	if(UP_BUTTON_PRESSED && (button_wait_time == 5) && (sub_display ==5))
	{
	screenDispNum = 17;
	button_wait_time = 0;
	}

	if(UP_BUTTON_PRESSED && (button_wait_time == 5) && (sub_display ==6))
	{
	screenDispNum = 23;
	button_wait_time = 0;
	}

	if(UP_BUTTON_PRESSED && (button_wait_time == 5) && (sub_display ==7))
	{
	screenDispNum = 24;
	button_wait_time = 0;
	}

	if(UP_BUTTON_PRESSED && (button_wait_time == 5) && (sub_display ==8))
	{
	screenDispNum = 27;
	button_wait_time = 0;
	}

	if(UP_BUTTON_PRESSED && (button_wait_time == 5) && (sub_display ==9))
	{
	screenDispNum = 28;
	button_wait_time = 0;
	}

	

	if(button_pressed <10)
	{
	min_unit_temp = button_pressed;
	button_pressed = 12;		// 12 is nothing ie no button pressed
	screenDispNum = 8;
	}

	if(button_pressed == 10)
	{
	button_pressed = 12;		// 12 is nothing ie no button pressed
	screenDispNum = 4;
	}

}

if(screenDispNum == 8)	//  H H : M M : _ _
{
	Clear_LCD();
	Send_Character(1,hr_tens_temp);
	Send_Character(2,hr_unit_temp);
	Send_Character(3,min_tens_temp);
	Send_Character(4,min_unit_temp);
	Send_Character(5,0);
	Send_Character(6,0);
	LCDDR8 = (1<<0);							// colons
	if(sub_display == 2) LCDDR0 |= (1<<2); 		// small digit 1 symbol
	if(sub_display == 3) LCDDR0 |= (1<<2); 		// small digit 1 symbol
	if(sub_display == 4) LCDDR0 |= (1<<2); 		// small digit 1 symbol
	if(sub_display == 5) LCDDR0 |= (1<<2); 		// small digit 1 symbol
	if(sub_display == 6) LCDDR0 |= (1<<6); 		// small digit 2 symbol
	if(sub_display == 7) LCDDR0 |= (1<<6); 		// small digit 2 symbol
	if(sub_display == 8) LCDDR0 |= (1<<6); 		// small digit 2 symbol
	if(sub_display == 9) LCDDR0 |= (1<<6); 		// small digit 2 symbol

	if(CENTRE_BUTTON_PRESSED) button_wait_time++;
	if(UP_BUTTON_PRESSED) button_wait_time++;



	if(CENTRE_BUTTON_PRESSED && (button_wait_time == 5) )
	{
	screenDispNum = 0;
	button_wait_time = 0;
	}

	if(UP_BUTTON_PRESSED && (button_wait_time == 5) && (sub_display ==1))
	{
	screenDispNum = 1;
	button_wait_time = 0;
	}

	if(UP_BUTTON_PRESSED && (button_wait_time == 5) && (sub_display ==2))
	{
	screenDispNum = 12;
	button_wait_time = 0;
	}

	if(UP_BUTTON_PRESSED && (button_wait_time == 5) && (sub_display ==3))
	{
	screenDispNum = 13;
	button_wait_time = 0;
	}

	if(UP_BUTTON_PRESSED && (button_wait_time == 5) && (sub_display ==4))
	{
	screenDispNum = 16;
	button_wait_time = 0;
	}

	if(UP_BUTTON_PRESSED && (button_wait_time == 5) && (sub_display ==5))
	{
	screenDispNum = 17;
	button_wait_time = 0;
	}

	if(UP_BUTTON_PRESSED && (button_wait_time == 5) && (sub_display ==6))
	{
	screenDispNum = 23;
	button_wait_time = 0;
	}

	if(UP_BUTTON_PRESSED && (button_wait_time == 5) && (sub_display ==7))
	{
	screenDispNum = 24;
	button_wait_time = 0;
	}

	if(UP_BUTTON_PRESSED && (button_wait_time == 5) && (sub_display ==8))
	{
	screenDispNum = 27;
	button_wait_time = 0;
	}

	if(UP_BUTTON_PRESSED && (button_wait_time == 5) && (sub_display ==9))
	{
	screenDispNum = 28;
	button_wait_time = 0;
	}


	if(button_pressed == 10)
	{
	button_pressed = 12;		// 12 is nothing ie no button pressed
	screenDispNum = 4;
	}


	if(button_pressed == 11)	// key # confirms and goes to home screen
	{
	button_pressed = 12;		// 12 is nothing ie no button pressed
	screenDispNum = 9;
	}

}


if(screenDispNum == 9)	// T SET
{

	Clear_LCD();
	Send_Character(1,T);
	Send_Character(3,S);
	Send_Character(4,E);
	Send_Character(5,T);

	if(sub_display == 2) LCDDR0 |= (1<<2); 		// small digit 1 symbol
	if(sub_display == 3) LCDDR0 |= (1<<2); 		// small digit 1 symbol
	if(sub_display == 4) LCDDR0 |= (1<<2); 		// small digit 1 symbol
	if(sub_display == 4) LCDDR0 |= (1<<2); 		// small digit 1 symbol
	if(sub_display == 5) LCDDR0 |= (1<<2); 		// small digit 1 symbol
	if(sub_display == 6) LCDDR0 |= (1<<6); 		// small digit 2 symbol
	if(sub_display == 7) LCDDR0 |= (1<<6); 		// small digit 2 symbol
	if(sub_display == 8) LCDDR0 |= (1<<6); 		// small digit 2 symbol
	if(sub_display == 9) LCDDR0 |= (1<<6); 		// small digit 2 symbol

	i++;

	if(i>3)
	{
		if(sub_display == 1)
		{
		sec_unit = 0;
		sec_tens = 0;
		min_unit = min_unit_temp;
		min_tens = min_tens_temp;
		hr_unit = hr_unit_temp;
		hr_tens = hr_tens_temp;
		screenDispNum = 0;		// home screen
		}
	
		if(sub_display == 2)
		{
		div1_on_min_unit = min_unit_temp;
		div1_on_min_tens = min_tens_temp;
		div1_on_hr_unit = hr_unit_temp;
		div1_on_hr_tens = hr_tens_temp;
		screenDispNum = 13;	// DIV 1 ON AT
		}
	
		if(sub_display == 3)
		{
		div1_off_min_unit = min_unit_temp;
		div1_off_min_tens = min_tens_temp;
		div1_off_hr_unit = hr_unit_temp;
		div1_off_hr_tens = hr_tens_temp;
		screenDispNum = 14;	// DIV 1 OFF AT
		}

		if(sub_display == 4)
		{
		div1_Cdown_min_unit = min_unit_temp;
		div1_Cdown_min_tens = min_tens_temp;
		div1_Cdown_hr_unit = hr_unit_temp;
		div1_Cdown_hr_tens = hr_tens_temp;
		relay_1_state = 1;	// permanently OFF !!
		screenDispNum = 18;	// DEV 1 count down display
		}

		if(sub_display == 5)
		{
		div1_Cdown_min_unit = min_unit_temp;
		div1_Cdown_min_tens = min_tens_temp;
		div1_Cdown_hr_unit = hr_unit_temp;
		div1_Cdown_hr_tens = hr_tens_temp;
		relay_1_state = 2;	// permanently ON !!
		screenDispNum = 18;	// DEV 1 count down display
		}



		if(sub_display == 6)
		{
		div2_on_min_unit = min_unit_temp;
		div2_on_min_tens = min_tens_temp;
		div2_on_hr_unit = hr_unit_temp;
		div2_on_hr_tens = hr_tens_temp;
		screenDispNum = 24;	// DIV 2 ON AT
		}
	
		if(sub_display == 7)
		{
		div2_off_min_unit = min_unit_temp;
		div2_off_min_tens = min_tens_temp;
		div2_off_hr_unit = hr_unit_temp;
		div2_off_hr_tens = hr_tens_temp;
		screenDispNum = 25;	// DIV 2 OFF AT
		}

		if(sub_display == 8)
		{
		div2_Cdown_min_unit = min_unit_temp;
		div2_Cdown_min_tens = min_tens_temp;
		div2_Cdown_hr_unit = hr_unit_temp;
		div2_Cdown_hr_tens = hr_tens_temp;
		relay_2_state = 1;	// permanently OFF !!
		screenDispNum = 20;	// DEV 2 count down display
		}

		if(sub_display == 9)
		{
		div2_Cdown_min_unit = min_unit_temp;
		div2_Cdown_min_tens = min_tens_temp;
		div2_Cdown_hr_unit = hr_unit_temp;
		div2_Cdown_hr_tens = hr_tens_temp;
		relay_2_state = 2;	// permanently ON !!
		screenDispNum = 20;	// DEV 2 count down display
		}

	}
}

//------------------------------------------------------------
//------------------------ DEV 1 -----------------------------
//------------------------------------------------------------

if(screenDispNum == 10)	// submenu DEV 1  TIMED ON/OFF
{
	Clear_LCD();
	Send_Character(1,T);
	Send_Character(2,I);
	Send_Character(3,M);
	Send_Character(4,E);
	Send_Character(5,D);
	LCDDR0 |= (1<<2); 		// small digit 1 symbol

	if(CENTRE_BUTTON_PRESSED) button_wait_time++;
	if(RIGHT_BUTTON_PRESSED) button_wait_time++;
	if(LEFT_BUTTON_PRESSED) button_wait_time++;
	if(DOWN_BUTTON_PRESSED) button_wait_time++;
	if(UP_BUTTON_PRESSED) button_wait_time++;

	if(CENTRE_BUTTON_PRESSED && (button_wait_time == 5) )
	{
	screenDispNum = 0;
	button_wait_time = 0;
	}

	if(RIGHT_BUTTON_PRESSED && (button_wait_time == 3)) 
	{
	screenDispNum = 11;
	button_wait_time = 0;
	}

	if(LEFT_BUTTON_PRESSED && (button_wait_time == 3)) 
	{
	screenDispNum = 11;
	button_wait_time = 0;
	}

	if(DOWN_BUTTON_PRESSED && (button_wait_time == 3)) 
	{
	screenDispNum = 12;
	button_wait_time = 0;
	}

	if(UP_BUTTON_PRESSED && (button_wait_time == 3)) 
	{
	screenDispNum = 2;
	button_wait_time = 0;
	}

}

if(screenDispNum == 11)	// submenu DEV 1  COUNTDOWNTIMER
{
	Clear_LCD();
	Send_Character(1,C);
	Send_Character(3,D);
	Send_Character(4,O);
	Send_Character(5,W);
	Send_Character(6,N);
	LCDDR0 |= (1<<2); 		// small digit 1 symbol 

	if(CENTRE_BUTTON_PRESSED) button_wait_time++;
	if(RIGHT_BUTTON_PRESSED) button_wait_time++;
	if(LEFT_BUTTON_PRESSED) button_wait_time++;
	if(DOWN_BUTTON_PRESSED) button_wait_time++;
	if(UP_BUTTON_PRESSED) button_wait_time++;

	if(CENTRE_BUTTON_PRESSED && (button_wait_time == 5) )
	{
	screenDispNum = 0;
	button_wait_time = 0;
	}

	if(RIGHT_BUTTON_PRESSED && (button_wait_time == 3)) 
	{
	screenDispNum = 10;
	button_wait_time = 0;
	}

	if(LEFT_BUTTON_PRESSED && (button_wait_time == 3)) 
	{
	screenDispNum = 10;
	button_wait_time = 0;
	}

	if(DOWN_BUTTON_PRESSED && (button_wait_time == 3)) 
	{
	screenDispNum = 16;
	button_wait_time = 0;
	}

	if(UP_BUTTON_PRESSED && (button_wait_time == 3)) 
	{
	screenDispNum = 2;
	button_wait_time = 0;
	}

}



if(screenDispNum == 12)	// submenu DEV 1 TIMED ON AT:
{
	Clear_LCD();
	Send_Character(1,O);
	Send_Character(2,N);
	Send_Character(4,A);
	Send_Character(5,T);

	LCDDR0 |= (1<<2); 		// small digit 1 symbol

	if(CENTRE_BUTTON_PRESSED) button_wait_time++;
	if(RIGHT_BUTTON_PRESSED) button_wait_time++;
	if(LEFT_BUTTON_PRESSED) button_wait_time++;
	if(DOWN_BUTTON_PRESSED) button_wait_time++;
	if(UP_BUTTON_PRESSED) button_wait_time++;

	if(CENTRE_BUTTON_PRESSED && (button_wait_time == 5) )
	{
	screenDispNum = 0;
	button_wait_time = 0;
	}

	if(RIGHT_BUTTON_PRESSED && (button_wait_time == 3)) 
	{
	screenDispNum = 13;
	button_wait_time = 0;
	}

	if(LEFT_BUTTON_PRESSED && (button_wait_time == 3)) 
	{
	screenDispNum = 14;
	button_wait_time = 0;
	}

	if(DOWN_BUTTON_PRESSED && (button_wait_time == 3)) 
	{
	screenDispNum = 4;
	sub_display = 2;
	button_wait_time = 0;
	}

	if(UP_BUTTON_PRESSED && (button_wait_time == 3)) 
	{
	screenDispNum = 10;
	button_wait_time = 0;
	}

}

if(screenDispNum == 13)	// submenu DEV 1 TIMED OFF AT:
{
	Clear_LCD();
	Send_Character(1,O);
	Send_Character(2,F);
	Send_Character(3,F);
	Send_Character(5,A);
	Send_Character(6,T);

	LCDDR0 |= (1<<2); 		// small digit 1 symbol

	if(CENTRE_BUTTON_PRESSED) button_wait_time++;
	if(RIGHT_BUTTON_PRESSED) button_wait_time++;
	if(LEFT_BUTTON_PRESSED) button_wait_time++;
	if(DOWN_BUTTON_PRESSED) button_wait_time++;
	if(UP_BUTTON_PRESSED) button_wait_time++;

	if(CENTRE_BUTTON_PRESSED && (button_wait_time == 5) )
	{
	screenDispNum = 0;
	button_wait_time = 0;
	}

	if(RIGHT_BUTTON_PRESSED && (button_wait_time == 3)) 
	{
	screenDispNum = 14;
	button_wait_time = 0;
	}

	if(LEFT_BUTTON_PRESSED && (button_wait_time == 3)) 
	{
	screenDispNum = 12;
	button_wait_time = 0;
	}

	if(DOWN_BUTTON_PRESSED && (button_wait_time == 3)) 
	{
	screenDispNum = 4;
	sub_display = 3;
	button_wait_time = 0;
	}

	if(UP_BUTTON_PRESSED && (button_wait_time == 3)) 
	{
	screenDispNum = 10;
	button_wait_time = 0;
	}

}

if(screenDispNum == 14)	// submenu DEV 1 CHECK ON/OFF TIME
{
	Clear_LCD();
	Send_Character(1,C);
	Send_Character(2,H);
	Send_Character(3,E);
	Send_Character(4,C);
	Send_Character(5,K);

	LCDDR0 |= (1<<2); 		// small digit 1 symbol

	if(CENTRE_BUTTON_PRESSED) button_wait_time++;
	if(RIGHT_BUTTON_PRESSED) button_wait_time++;
	if(LEFT_BUTTON_PRESSED) button_wait_time++;
	if(DOWN_BUTTON_PRESSED) button_wait_time++;
	if(UP_BUTTON_PRESSED) button_wait_time++;

	if(CENTRE_BUTTON_PRESSED && (button_wait_time == 5) )
	{
	screenDispNum = 0;
	button_wait_time = 0;
	}

	if(RIGHT_BUTTON_PRESSED && (button_wait_time == 3)) 
	{
	screenDispNum = 12;
	button_wait_time = 0;
	}

	if(LEFT_BUTTON_PRESSED && (button_wait_time == 3)) 
	{
	screenDispNum = 13;
	button_wait_time = 0;
	}

	if(DOWN_BUTTON_PRESSED && (button_wait_time == 3)) 
	{
	screenDispNum = 15;
	i = 0;
	button_wait_time = 0;
	}

	if(UP_BUTTON_PRESSED && (button_wait_time == 3)) 
	{
	screenDispNum = 10;
	button_wait_time = 0;
	}

}


if(screenDispNum == 15)	// submenu DEV 1 CHECK - CYCLES THROUGH
{



if(i<5)	
{	
	Clear_LCD();
	Send_Character(1,O);
	Send_Character(2,N);
	Send_Character(4,A);
	Send_Character(5,T);
	LCDDR0 |= (1<<2); 		// small digit 1 symbol
	i++;
}

if(i>=5 && i<10)	
{
	Clear_LCD();
	Send_Character(1,div1_on_hr_tens);
	Send_Character(2,div1_on_hr_unit);
	Send_Character(3,div1_on_min_tens);
	Send_Character(4,div1_on_min_unit);
	Send_Character(5,0);
	Send_Character(6,0);
	LCDDR8 = (1<<0);			// colons
	LCDDR0 |= (1<<2); 		// small digit 1 symbol
	i++;

}

if(i>=10 && i<15)	
{
	Clear_LCD();
	Send_Character(1,O);
	Send_Character(2,F);
	Send_Character(3,F);
	Send_Character(5,A);
	Send_Character(6,T);
	LCDDR0 |= (1<<2); 		// small digit 1 symbol
	i++;
 
}

if(i>=15 && i<20)	
{

	Clear_LCD();
	Send_Character(1,div1_off_hr_tens);
	Send_Character(2,div1_off_hr_unit);
	Send_Character(3,div1_off_min_tens);
	Send_Character(4,div1_off_min_unit);
	Send_Character(5,0);
	Send_Character(6,0);
	LCDDR8 = (1<<0);			// colons
	LCDDR0 |= (1<<2); 		// small digit 1 symbol
	i++;

}

if(i>=20) screenDispNum = 14;


	if(CENTRE_BUTTON_PRESSED) button_wait_time++;

	if(CENTRE_BUTTON_PRESSED && (button_wait_time == 5) )
	{
	screenDispNum = 0;
	button_wait_time = 0;
	}

}



if(screenDispNum == 16)	// submenu DEV 2 C Down ON IN
{
	Clear_LCD();
	Send_Character(1,O);
	Send_Character(2,N);
	Send_Character(4,I);
	Send_Character(5,N);

	LCDDR0 |= (1<<2); 		// small digit 1 symbol

	if(CENTRE_BUTTON_PRESSED) button_wait_time++;
	if(RIGHT_BUTTON_PRESSED) button_wait_time++;
	if(LEFT_BUTTON_PRESSED) button_wait_time++;
	if(DOWN_BUTTON_PRESSED) button_wait_time++;
	if(UP_BUTTON_PRESSED) button_wait_time++;

	if(CENTRE_BUTTON_PRESSED && (button_wait_time == 5) )
	{
	screenDispNum = 0;
	button_wait_time = 0;
	}

	if(RIGHT_BUTTON_PRESSED && (button_wait_time == 3)) 
	{
	screenDispNum = 17;
	button_wait_time = 0;
	}

	if(LEFT_BUTTON_PRESSED && (button_wait_time == 3)) 
	{
	screenDispNum = 17;
	button_wait_time = 0;
	}

	if(DOWN_BUTTON_PRESSED && (button_wait_time == 3)) 
	{
	screenDispNum = 4;
	sub_display = 4;
	button_wait_time = 0;
	}

	if(UP_BUTTON_PRESSED && (button_wait_time == 3)) 
	{
	screenDispNum = 11;
	button_wait_time = 0;
	}

}



if(screenDispNum == 17)	// submenu DEV 1 C Down OFF IN
{
	Clear_LCD();
	Send_Character(1,O);
	Send_Character(2,F);
	Send_Character(3,F);
	Send_Character(5,I);
	Send_Character(6,N);

	LCDDR0 |= (1<<2); 		// small digit 1 symbol

	if(CENTRE_BUTTON_PRESSED) button_wait_time++;
	if(RIGHT_BUTTON_PRESSED) button_wait_time++;
	if(LEFT_BUTTON_PRESSED) button_wait_time++;
	if(DOWN_BUTTON_PRESSED) button_wait_time++;
	if(UP_BUTTON_PRESSED) button_wait_time++;

	if(CENTRE_BUTTON_PRESSED && (button_wait_time == 5) )
	{
	screenDispNum = 0;
	button_wait_time = 0;
	}

	if(RIGHT_BUTTON_PRESSED && (button_wait_time == 3)) 
	{
	screenDispNum = 16;
	button_wait_time = 0;
	}

	if(LEFT_BUTTON_PRESSED && (button_wait_time == 3)) 
	{
	screenDispNum = 16;
	button_wait_time = 0;
	}

	if(DOWN_BUTTON_PRESSED && (button_wait_time == 3)) 
	{
	screenDispNum = 4;
	sub_display = 5;
	button_wait_time = 0;
	}

	if(UP_BUTTON_PRESSED && (button_wait_time == 3)) 
	{
	screenDispNum = 11;
	button_wait_time = 0;
	}

}






if(screenDispNum == 19)	// CLEAR THE COUNTDOWN TIMER AND RESET TO AUTOMATIC ON/OFF MODE
{

	Clear_LCD();
	Send_Character(1,C);
	Send_Character(2,L);
	Send_Character(3,E);
	Send_Character(4,A);
	Send_Character(5,R);

 if (clear_sub_disp == 1) LCDDR0 |= (1<<2); 		// small digit 1 symbol
 if (clear_sub_disp == 2) LCDDR0 |= (1<<6); 		// small digit 2 symbol
	

	i++;

	if(i>3) 
	{
	
			if (clear_sub_disp == 1)
			{
			relay_1_state = 0;	// puts relay 1 back into timed state
			div1_Cdown_sec_unit = 0;
			div1_Cdown_sec_tens = 0;
			div1_Cdown_min_unit = 0;
			div1_Cdown_min_tens = 0;
			div1_Cdown_hr_unit = 0;
			div1_Cdown_hr_tens = 0;
			lockout1 = 0;
			screenDispNum = 0;
			}


			if (clear_sub_disp == 2)
			{
			relay_2_state = 0;	// puts relay 2 back into timed state
			div2_Cdown_sec_unit = 0;
			div2_Cdown_sec_tens = 0;
			div2_Cdown_min_unit = 0;
			div2_Cdown_min_tens = 0;
			div2_Cdown_hr_unit = 0;
			div2_Cdown_hr_tens = 0;
			lockout2 = 0;
			screenDispNum = 0;
			}

	}
}




// ===========================================================================



//------------------------------------------------------------
//------------------------ DEV 2 -----------------------------
//------------------------------------------------------------

if(screenDispNum == 21)	// submenu DIV 2
{
	Clear_LCD();
	Send_Character(1,T);
	Send_Character(2,I);
	Send_Character(3,M);
	Send_Character(4,E);
	Send_Character(5,D);
	LCDDR0 |= (1<<6); 		// small digit 2 symbol

	if(CENTRE_BUTTON_PRESSED) button_wait_time++;
	if(RIGHT_BUTTON_PRESSED) button_wait_time++;
	if(LEFT_BUTTON_PRESSED) button_wait_time++;
	if(DOWN_BUTTON_PRESSED) button_wait_time++;
	if(UP_BUTTON_PRESSED) button_wait_time++;

	if(CENTRE_BUTTON_PRESSED && (button_wait_time == 5) )
	{
	screenDispNum = 0;
	button_wait_time = 0;
	}

	if(RIGHT_BUTTON_PRESSED && (button_wait_time == 3)) 
	{
	screenDispNum = 22;
	button_wait_time = 0;
	}

	if(LEFT_BUTTON_PRESSED && (button_wait_time == 3)) 
	{
	screenDispNum = 22;
	button_wait_time = 0;
	}

	if(DOWN_BUTTON_PRESSED && (button_wait_time == 3)) 
	{
	screenDispNum = 23;
	button_wait_time = 0;
	}

	if(UP_BUTTON_PRESSED && (button_wait_time == 3)) 
	{
	screenDispNum = 3;
	button_wait_time = 0;
	}

}

if(screenDispNum == 22)	// submenu DIV 2
{
	Clear_LCD();
	Send_Character(1,C);
	Send_Character(3,D);
	Send_Character(4,O);
	Send_Character(5,W);
	Send_Character(6,N);
	LCDDR0 |= (1<<6); 		// small digit 2 symbol

	if(CENTRE_BUTTON_PRESSED) button_wait_time++;
	if(RIGHT_BUTTON_PRESSED) button_wait_time++;
	if(LEFT_BUTTON_PRESSED) button_wait_time++;
	if(DOWN_BUTTON_PRESSED) button_wait_time++;
	if(UP_BUTTON_PRESSED) button_wait_time++;

	if(CENTRE_BUTTON_PRESSED && (button_wait_time == 5) )
	{
	screenDispNum = 0;
	button_wait_time = 0;
	}

	if(RIGHT_BUTTON_PRESSED && (button_wait_time == 3)) 
	{
	screenDispNum = 21;
	button_wait_time = 0;
	}

	if(LEFT_BUTTON_PRESSED && (button_wait_time == 3)) 
	{
	screenDispNum = 21;
	button_wait_time = 0;
	}

	if(DOWN_BUTTON_PRESSED && (button_wait_time == 3)) 
	{
	screenDispNum = 27;
	button_wait_time = 0;
	}

	if(UP_BUTTON_PRESSED && (button_wait_time == 3)) 
	{
	screenDispNum = 3;
	button_wait_time = 0;
	}

}



if(screenDispNum == 23)	// submenu DIV 2 ON AT:
{
	Clear_LCD();
	Send_Character(1,O);
	Send_Character(2,N);
	Send_Character(4,A);
	Send_Character(5,T);

	LCDDR0 |= (1<<6); 		// small digit 2 symbol

	if(CENTRE_BUTTON_PRESSED) button_wait_time++;
	if(RIGHT_BUTTON_PRESSED) button_wait_time++;
	if(LEFT_BUTTON_PRESSED) button_wait_time++;
	if(DOWN_BUTTON_PRESSED) button_wait_time++;
	if(UP_BUTTON_PRESSED) button_wait_time++;

	if(CENTRE_BUTTON_PRESSED && (button_wait_time == 5) )
	{
	screenDispNum = 0;
	button_wait_time = 0;
	}

	if(RIGHT_BUTTON_PRESSED && (button_wait_time == 3)) 
	{
	screenDispNum = 24;
	button_wait_time = 0;
	}

	if(LEFT_BUTTON_PRESSED && (button_wait_time == 3)) 
	{
	screenDispNum = 25;
	button_wait_time = 0;
	}

	if(DOWN_BUTTON_PRESSED && (button_wait_time == 3)) 
	{
	screenDispNum = 4;
	sub_display = 6;
	button_wait_time = 0;
	}

	if(UP_BUTTON_PRESSED && (button_wait_time == 3)) 
	{
	screenDispNum = 21;
	button_wait_time = 0;
	}

}

if(screenDispNum == 24)	// submenu DIV 2 OFF AT:
{
	Clear_LCD();
	Send_Character(1,O);
	Send_Character(2,F);
	Send_Character(3,F);
	Send_Character(5,A);
	Send_Character(6,T);

	LCDDR0 |= (1<<6); 		// small digit 2 symbol

	if(CENTRE_BUTTON_PRESSED) button_wait_time++;
	if(RIGHT_BUTTON_PRESSED) button_wait_time++;
	if(LEFT_BUTTON_PRESSED) button_wait_time++;
	if(DOWN_BUTTON_PRESSED) button_wait_time++;
	if(UP_BUTTON_PRESSED) button_wait_time++;

	if(CENTRE_BUTTON_PRESSED && (button_wait_time == 5) )
	{
	screenDispNum = 0;
	button_wait_time = 0;
	}

	if(RIGHT_BUTTON_PRESSED && (button_wait_time == 3)) 
	{
	screenDispNum = 25;
	button_wait_time = 0;
	}

	if(LEFT_BUTTON_PRESSED && (button_wait_time == 3)) 
	{
	screenDispNum = 23;
	button_wait_time = 0;
	}

	if(DOWN_BUTTON_PRESSED && (button_wait_time == 3)) 
	{
	screenDispNum = 4;
	sub_display = 7;
	button_wait_time = 0;
	}

	if(UP_BUTTON_PRESSED && (button_wait_time == 3)) 
	{
	screenDispNum = 21;
	button_wait_time = 0;
	}

}

if(screenDispNum == 25)	// submenu DIV 2 CHECK
{
	Clear_LCD();
	Send_Character(1,C);
	Send_Character(2,H);
	Send_Character(3,E);
	Send_Character(4,C);
	Send_Character(5,K);

	LCDDR0 |= (1<<6); 		// small digit 2 symbol

	if(CENTRE_BUTTON_PRESSED) button_wait_time++;
	if(RIGHT_BUTTON_PRESSED) button_wait_time++;
	if(LEFT_BUTTON_PRESSED) button_wait_time++;
	if(DOWN_BUTTON_PRESSED) button_wait_time++;
	if(UP_BUTTON_PRESSED) button_wait_time++;

	if(CENTRE_BUTTON_PRESSED && (button_wait_time == 5) )
	{
	screenDispNum = 0;
	button_wait_time = 0;
	}

	if(RIGHT_BUTTON_PRESSED && (button_wait_time == 3)) 
	{
	screenDispNum = 23;
	button_wait_time = 0;
	}

	if(LEFT_BUTTON_PRESSED && (button_wait_time == 3)) 
	{
	screenDispNum = 24;
	button_wait_time = 0;
	}

	if(DOWN_BUTTON_PRESSED && (button_wait_time == 3)) 
	{
	screenDispNum = 26;
	i = 0;
	button_wait_time = 0;
	}

	if(UP_BUTTON_PRESSED && (button_wait_time == 3)) 
	{
	screenDispNum = 21;
	button_wait_time = 0;
	}

}


if(screenDispNum == 26)	// submenu DIV 2 CHECK
{



if(i<5)	
{	
	Clear_LCD();
	Send_Character(1,O);
	Send_Character(2,N);
	Send_Character(4,A);
	Send_Character(5,T);
	LCDDR0 |= (1<<6); 		// small digit 2 symbol
	i++;
}

if(i>=5 && i<10)	
{
	Clear_LCD();
	Send_Character(1,div2_on_hr_tens);
	Send_Character(2,div2_on_hr_unit);
	Send_Character(3,div2_on_min_tens);
	Send_Character(4,div2_on_min_unit);
	Send_Character(5,0);
	Send_Character(6,0);
	LCDDR8 = (1<<0);			// colons
	LCDDR0 |= (1<<6); 		// small digit 2 symbol
	i++;

}

if(i>=10 && i<15)	
{
	Clear_LCD();
	Send_Character(1,O);
	Send_Character(2,F);
	Send_Character(3,F);
	Send_Character(5,A);
	Send_Character(6,T);
	LCDDR0 |= (1<<6); 		// small digit 2 symbol
	i++;
 
}

if(i>=15 && i<20)	
{

	Clear_LCD();
	Send_Character(1,div2_off_hr_tens);
	Send_Character(2,div2_off_hr_unit);
	Send_Character(3,div2_off_min_tens);
	Send_Character(4,div2_off_min_unit);
	Send_Character(5,0);
	Send_Character(6,0);
	LCDDR8 = (1<<0);			// colons
	LCDDR0 |= (1<<6); 		// small digit 2 symbol
	i++;

}

if(i>=20) screenDispNum = 25;


	if(CENTRE_BUTTON_PRESSED) button_wait_time++;

	if(CENTRE_BUTTON_PRESSED && (button_wait_time == 5) )
	{
	screenDispNum = 0;
	button_wait_time = 0;
	}

}



if(screenDispNum == 27)	// submenu DEV 2 C Down
{
	Clear_LCD();
	Send_Character(1,O);
	Send_Character(2,N);
	Send_Character(4,I);
	Send_Character(5,N);

	LCDDR0 |= (1<<6); 		// small digit 2 symbol

	if(CENTRE_BUTTON_PRESSED) button_wait_time++;
	if(RIGHT_BUTTON_PRESSED) button_wait_time++;
	if(LEFT_BUTTON_PRESSED) button_wait_time++;
	if(DOWN_BUTTON_PRESSED) button_wait_time++;
	if(UP_BUTTON_PRESSED) button_wait_time++;

	if(CENTRE_BUTTON_PRESSED && (button_wait_time == 5) )
	{
	screenDispNum = 0;
	button_wait_time = 0;
	}

	if(RIGHT_BUTTON_PRESSED && (button_wait_time == 3)) 
	{
	screenDispNum = 28;
	button_wait_time = 0;
	}

	if(LEFT_BUTTON_PRESSED && (button_wait_time == 3)) 
	{
	screenDispNum = 28;
	button_wait_time = 0;
	}

	if(DOWN_BUTTON_PRESSED && (button_wait_time == 3)) 
	{
	screenDispNum = 4;
	sub_display = 8;
	button_wait_time = 0;
	}

	if(UP_BUTTON_PRESSED && (button_wait_time == 3)) 
	{
	screenDispNum = 22;
	button_wait_time = 0;
	}

}



if(screenDispNum == 28)	// submenu DEV 2 C Down
{
	Clear_LCD();
	Send_Character(1,O);
	Send_Character(2,F);
	Send_Character(3,F);
	Send_Character(5,I);
	Send_Character(6,N);

	LCDDR0 |= (1<<6); 		// small digit 2 symbol

	if(CENTRE_BUTTON_PRESSED) button_wait_time++;
	if(RIGHT_BUTTON_PRESSED) button_wait_time++;
	if(LEFT_BUTTON_PRESSED) button_wait_time++;
	if(DOWN_BUTTON_PRESSED) button_wait_time++;
	if(UP_BUTTON_PRESSED) button_wait_time++;

	if(CENTRE_BUTTON_PRESSED && (button_wait_time == 5) )
	{
	screenDispNum = 0;
	button_wait_time = 0;
	}

	if(RIGHT_BUTTON_PRESSED && (button_wait_time == 3)) 
	{
	screenDispNum = 27;
	button_wait_time = 0;
	}

	if(LEFT_BUTTON_PRESSED && (button_wait_time == 3)) 
	{
	screenDispNum = 27;
	button_wait_time = 0;
	}

	if(DOWN_BUTTON_PRESSED && (button_wait_time == 3)) 
	{
	screenDispNum = 4;
	sub_display = 9;
	button_wait_time = 0;
	}

	if(UP_BUTTON_PRESSED && (button_wait_time == 3)) 
	{
	screenDispNum = 22;
	button_wait_time = 0;
	}

}






// ==========================================================================






if(relay_1_state == 1) 
{
RELAY_1_0FF;
}


if(relay_1_state == 2)
	{
	RELAY_1_0N; 
	LCDDR0 |= (1<<1); // arrow 1
	}




if(relay_2_state == 1) 
{
RELAY_2_0FF;
}


if(relay_2_state == 2)
	{
	RELAY_2_0N; 
	LCDDR0 |= (1<<5); // arrow 2
	}


} // end ISP TIMER 1












ISR(TIMER2_OVF_vect)
{

sec_unit++;

	if(sec_unit == 10)
	{
		sec_unit = 0;
		sec_tens++;
	}

	
	if(sec_unit == 0 && sec_tens == 6)
	{
		min_unit++;
		sec_unit =0;
		sec_tens = 0;
	}

	
	if(min_unit == 10)
	{
		min_unit = 0;
		min_tens++;
	}

	
	if(min_unit == 0 && min_tens == 6)
	{
		hr_unit++;
		min_unit =0;
		min_tens = 0;
	}

	if(hr_unit == 10)
	{
		hr_unit = 0;
		hr_tens++;
	}

	
	if(hr_unit == 4 && hr_tens == 2)
	{
		hr_unit = 0;
		hr_tens = 0;
	}

// =====================================================

if((relay_1_state > 0) && (lockout1 == 0))
	{

		flag1 = 0;

		if (div1_Cdown_hr_tens > 0 || div1_Cdown_hr_unit > 0 || div1_Cdown_min_tens > 0 || div1_Cdown_min_unit > 0 
				|| div1_Cdown_sec_tens > 0 || div1_Cdown_sec_unit > 0)
				{
				flag1 = 1;
				div1_Cdown_sec_unit--;
				}



		if(div1_Cdown_sec_unit == 255)
		{
		div1_Cdown_sec_tens--;
		div1_Cdown_sec_unit = 9;
		}

		if ( div1_Cdown_sec_tens == 255 )
		{
		div1_Cdown_sec_tens = 5;
		div1_Cdown_min_unit--;
		}


		if(div1_Cdown_min_unit == 255)
		{
		div1_Cdown_min_tens --;
		div1_Cdown_min_unit = 9;
		}

		if ( div1_Cdown_min_tens == 255 )
		{
		div1_Cdown_min_tens = 5;
		div1_Cdown_hr_unit--;
		}

		if(div1_Cdown_hr_unit == 255)
		{
		div1_Cdown_hr_tens --;
		div1_Cdown_hr_unit = 9;
		}

		if ( div1_Cdown_hr_tens == 255 )
		{
		div1_Cdown_hr_tens = 0;
		div1_Cdown_hr_unit--;
		}

		if ((flag1 == 0) && (relay_1_state == 1)) relay_1_state = 2;
		else if ((flag1 == 0) && (relay_1_state == 2)) relay_1_state = 1;

		if (flag1 == 0) lockout1 = 1 ;								// never get back into this function unless restarted to avoid blinking.
	}


// =====================================================

if((relay_2_state > 0) && (lockout2 == 0))
	{

		flag2 = 0;

		if (div2_Cdown_hr_tens > 0 || div2_Cdown_hr_unit > 0 || div2_Cdown_min_tens > 0 || div2_Cdown_min_unit > 0 
				|| div2_Cdown_sec_tens > 0 || div2_Cdown_sec_unit > 0)
				{
				flag2 = 1;
				div2_Cdown_sec_unit--;
				}



		if(div2_Cdown_sec_unit == 255)
		{
		div2_Cdown_sec_tens--;
		div2_Cdown_sec_unit = 9;
		}

		if ( div2_Cdown_sec_tens == 255 )
		{
		div2_Cdown_sec_tens = 5;
		div2_Cdown_min_unit--;
		}


		if(div2_Cdown_min_unit == 255)
		{
		div2_Cdown_min_tens --;
		div2_Cdown_min_unit = 9;
		}

		if ( div2_Cdown_min_tens == 255 )
		{
		div2_Cdown_min_tens = 5;
		div2_Cdown_hr_unit--;
		}

		if(div2_Cdown_hr_unit == 255)
		{
		div2_Cdown_hr_tens --;
		div2_Cdown_hr_unit = 9;
		}

		if ( div2_Cdown_hr_tens == 255 )
		{
		div2_Cdown_hr_tens = 0;
		div2_Cdown_hr_unit--;
		}

		if ((flag2 == 0) && (relay_2_state == 1)) relay_2_state = 2;
		else if ((flag2 == 0) && (relay_2_state == 2)) relay_2_state = 1;

		if (flag2 == 0) lockout2 = 1 ;								// never get back into this function unless restarted to avoid blinking.
	}


} // end timer
































void checkTimedRelayONE(uint8_t hr_tens, uint8_t hr_unit, uint8_t min_tens, uint8_t min_unit,
						uint8_t div1_on_hr_tens, uint8_t div1_on_hr_unit, uint8_t div1_on_min_tens, uint8_t div1_on_min_unit, 
						uint8_t div1_off_hr_tens, uint8_t div1_off_hr_unit, uint8_t div1_off_min_tens, uint8_t div1_off_min_unit)
{

uint16_t div_1_on;
uint16_t div_1_off; 
uint16_t time_now;


div_1_on = (60*((10*div1_on_hr_tens) + div1_on_hr_unit)) + ((10*div1_on_min_tens) + div1_on_min_unit);
div_1_off = (60*((10*div1_off_hr_tens) + div1_off_hr_unit)) + ((10*div1_off_min_tens) + div1_off_min_unit);

time_now = (60*((10*hr_tens) + hr_unit)) + ((10*min_tens) + min_unit);


	if(relay_1_state == 0)  // 0 = nothing state, 1 = permanently off, 2 = permanently on
	{
		if(div_1_on < div_1_off)
		{
			if((time_now >= div_1_on) && (time_now < div_1_off) ) 
				{
				RELAY_1_0N; 
				LCDDR0 |= (1<<1); // arrow 1
				}
			else RELAY_1_0FF;
		}


		if(div_1_on > div_1_off)
		{
			if(((time_now >= div_1_on) && (time_now < 1440)) ||  (time_now < div_1_off) )
				{
				RELAY_1_0N; 
				LCDDR0 |= (1<<1); // arrow 1
				}
			else RELAY_1_0FF;
		}


		if(div_1_on == div_1_off) RELAY_1_0FF;
	

	}

}


void checkTimedRelayTWO(uint8_t hr_tens, uint8_t hr_unit, uint8_t min_tens, uint8_t min_unit,
						uint8_t div2_on_hr_tens, uint8_t div2_on_hr_unit, uint8_t div2_on_min_tens, uint8_t div2_on_min_unit, 
						uint8_t div2_off_hr_tens, uint8_t div2_off_hr_unit, uint8_t div2_off_min_tens, uint8_t div2_off_min_unit)
{

uint16_t div_2_on;
uint16_t div_2_off; 
uint16_t time_now;


div_2_on = (60*((10*div2_on_hr_tens) + div2_on_hr_unit)) + ((10*div2_on_min_tens) + div2_on_min_unit);
div_2_off = (60*((10*div2_off_hr_tens) + div2_off_hr_unit)) + ((10*div2_off_min_tens) + div2_off_min_unit);

time_now = (60*((10*hr_tens) + hr_unit)) + ((10*min_tens) + min_unit);


	if(relay_2_state == 0)  // 0 = nothing state, 1 = permanently off, 2 = permanently on
	{
		if(div_2_on < div_2_off)
		{
			if((time_now >= div_2_on) && (time_now < div_2_off) ) 
				{
				RELAY_2_0N; 
				LCDDR0 |= (1<<5); // arrow 2
				}
			else RELAY_2_0FF;
		}


		if(div_2_on > div_2_off)
		{
			if(((time_now >= div_2_on) && (time_now < 1440)) ||  (time_now < div_2_off) )
				{
				RELAY_2_0N; 
				LCDDR0 |= (1<<5); // arrow 2
				}
			else RELAY_2_0FF;
		}


		if(div_2_on == div_2_off) RELAY_2_0FF;
	

	}

}





