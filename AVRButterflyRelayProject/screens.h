#ifndef screens
#define screens

#include "lcd.h"

void screen(uint8_t screen_number);





void screen(uint8_t screen_number)
{

	switch(screen_number)
	{
	
	case:0 	
	
	Send_Character(1,hr_tens);
	Send_Character(2,hr_unit);
	Send_Character(3,min_tens);
	Send_Character(4,min_unit);
	Send_Character(5,sec_tens);
	Send_Character(6,sec_unit);

	LCDDR8 = (1<<0);	// colons



	break;



	}



}





#endif
