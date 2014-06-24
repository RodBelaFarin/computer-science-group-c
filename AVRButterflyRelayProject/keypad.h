#ifndef keypad
#define keypad


uint8_t button_pressed = 12;		// 12 is nothing ie no button pressed


void keypadInit(void);
void keypad_Get_Pressed(void);


// -----------------------------------------------------------

void keypadInit(void)
{
DDRE &= ~( (1<<4) | (1<<5) | (1<<6)  );				// keypad column inputs
DDRF &= ~( (1<<4) | (1<<5) | (1<<6) | (1<<7) );		// keypad row inputs
PORTF |= 0xF0;
}


void keypad_Get_Pressed(void)
{


for(int cols = 4; cols<=6; cols++)
	{


	DDRE &= ~( (1<<4) | (1<<5) | (1<<6)  );				// keypad column inputs

	DDRE |= (1<<cols);									//make a column as output
	PORTE &= ~(1<<cols); 								// make it low
	
		if((bit_is_clear(PINF, 4)) && (cols == 4)) button_pressed = 1;
		if((bit_is_clear(PINF, 5)) && (cols == 4)) button_pressed = 4;
		if((bit_is_clear(PINF, 6)) && (cols == 4)) button_pressed = 7;
		if((bit_is_clear(PINF, 7)) && (cols == 4)) button_pressed = 10;		// star  *

		if((bit_is_clear(PINF, 4)) && (cols == 5)) button_pressed = 2;
		if((bit_is_clear(PINF, 5)) && (cols == 5)) button_pressed = 5;
		if((bit_is_clear(PINF, 6)) && (cols == 5)) button_pressed = 8;
		if((bit_is_clear(PINF, 7)) && (cols == 5)) button_pressed = 0;


		if((bit_is_clear(PINF, 4)) && (cols == 6)) button_pressed = 3;
		if((bit_is_clear(PINF, 5)) && (cols == 6)) button_pressed = 6;
		if((bit_is_clear(PINF, 6)) && (cols == 6)) button_pressed = 9;
		if((bit_is_clear(PINF, 7)) && (cols == 6)) button_pressed = 11;		// hash #
		
	}


}


#endif
