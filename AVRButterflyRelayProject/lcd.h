#ifndef lcd
#define lcd

#include <avr/io.h>
#include "lookUpTable.h"

void LCD_Init(void);
void LCD_disable(void);
void Clear_LCD(void);
void Send_Character(unsigned char position,  uint16_t character);






void LCD_Init()
{
/* Use 32 kHz crystal oscillator */
/* 1/3 Bias and 1/3 duty, SEG21:SEG24 is used as port pins */
LCDCRB = (1<<LCDCS) | (1<<LCDMUX1) | (1<<LCDMUX0)| (1<<LCDPM2) | (1<<LCDPM1) | (1<<LCDPM0);
/* Using 16 as prescaler selection and 7 as LCD Clock Divide */
/* gives a frame rate of 49 Hz */
LCDFRR = (1<<LCDCD2) | (1<<LCDCD1);
/* Set segment drive time to 125 ìs and output voltage to 3.3 V*/
LCDCCR = (1<<LCDDC1) | (1<<LCDCC3) | (1<<LCDCC2) | (1<<LCDCC1);
/* Enable LCD, default waveform and no interrupt enabled */
LCDCRA = (1<<LCDEN);
}


void LCD_disable()
{
/* Wait until a new frame is started. */
while ( !(LCDCRA & (1<<LCDIF)) );
/* Set LCD Blanking and clear interrupt flag */
/* by writing a logical one to the flag. */
LCDCRA = (1<<LCDEN)|(1<<LCDIF)|(1<<LCDBL);
/* Wait until LCD Blanking is effective. */
while ( !(LCDCRA & (1<<LCDIF)) );
/* Disable LCD */
LCDCRA = (0<<LCDEN);
}


void Clear_LCD(void)
{
LCDDR0 = 0;
LCDDR1 = 0;
LCDDR2 = 0;
LCDDR3 = 0;

LCDDR5 = 0;
LCDDR6 = 0;
LCDDR7 = 0;
LCDDR8 = 0;

LCDDR10 = 0;
LCDDR11 = 0;
LCDDR12 = 0;
LCDDR13 = 0;

LCDDR15 = 0;
LCDDR16 = 0;
LCDDR17 = 0;
LCDDR18 = 0;

}


void Send_Character(unsigned char position, uint16_t character)
{

if(position ==0 || position >6) return;


	switch(position)
	{

	case 1:	LCDDR0 |= nib[character] & 0x0F;	
			LCDDR5 |= nib[character]>>4 & 0x0F;
			LCDDR10 |= nib[character]>>8 & 0x0F;
			LCDDR15 |= nib[character]>>12 & 0x0F;
	break;

	case 2:	LCDDR0 |= (nib[character] & 0x0F)<<4;	
			LCDDR5 |= (nib[character]>>4 & 0x0F)<<4;
			LCDDR10 |= (nib[character]>>8 & 0x0F)<<4;
			LCDDR15 |= (nib[character]>>12 & 0x0F)<<4;
	break;

	case 3:	LCDDR1 |= nib[character] & 0x0F;	
			LCDDR6 |= nib[character]>>4 & 0x0F;
			LCDDR11 |= nib[character]>>8 & 0x0F;
			LCDDR16 |= nib[character]>>12 & 0x0F;
	break;

	case 4:	LCDDR1 |= (nib[character] & 0x0F)<<4;	
			LCDDR6 |= (nib[character]>>4 & 0x0F)<<4;
			LCDDR11 |= (nib[character]>>8 & 0x0F)<<4;
			LCDDR16 |= (nib[character]>>12 & 0x0F)<<4;
	break;

	case 5:	LCDDR2 |= nib[character] & 0x0F;	
			LCDDR7 |= nib[character]>>4 & 0x0F;
			LCDDR12 |= nib[character]>>8 & 0x0F;
			LCDDR17 |= nib[character]>>12 & 0x0F;
	break;

	case 6:	LCDDR2 |= (nib[character] & 0x0F)<<4;	
			LCDDR7 |= (nib[character]>>4 & 0x0F)<<4;
			LCDDR12 |= (nib[character]>>8 & 0x0F)<<4;
			LCDDR17 |= (nib[character]>>12 & 0x0F)<<4;
	break;

	}


}


#endif
