//
//  Author(s)...: ATMEL Norway - Modified by Cruz Cecilia
//
//  Target(s)...: ATmega169
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/sleep.h>
#include <inttypes.h>
#include <avr/pgmspace.h>

/************************************************************************/
// Definitions
/************************************************************************/
#define BOOL    char
#define FALSE   0
#define TRUE    (!FALSE)
#define AUTO    3

#define LCD_INITIAL_CONTRAST    0x0F
#define LCD_TIMER_SEED		    3
#define LCD_FLASH_SEED          10
#define LCD_REGISTER_COUNT      20
#define TEXTBUFFER_SIZE         25


#define SCROLLMODE_ONCE         0x01
#define SCROLLMODE_LOOP         0x02
#define SCROLLMODE_WAVE         0x03

#define pLCDREG_test (*(char *)(0xEC))

/************************************************************************/
// Global variables
/************************************************************************/
extern volatile char gLCD_Update_Required;
extern char LCD_Data[LCD_REGISTER_COUNT];
extern char gTextBuffer[TEXTBUFFER_SIZE];
extern volatile char gScrollMode;
extern char gFlashTimer;
extern char gColon;
extern volatile signed char gScroll;
char SetContrast(char input);
char lcd_str[30]; //Variable to enable MACROS
double v;		  //Variable to enable MACROS

/************************************************************************/
// Global functions
/************************************************************************/
void Initialization(void);
void LCD_puts_f(const char *pFlashStr);
void LCD_UpdateRequired(char update, char scrollmode);
void LCD_Clear(void);
void LCD_Colon(char show);
void LCD_FlashReset(void);
void LCD_Init (void);
void LCD_WriteDigit(char input, char digit);
void LCD_AllSegments(char show);
void LCD_putc(char character);
void LCD_puts(char *pStr);

/************************************************************************/
//MACROS
/************************************************************************/
#define LCD_SET_COLON(active) LCD_Data[8] = active //active = [TRUE;FALSE]
#define pLCDREG ((unsigned char *)(0xEC))// DEVICE SPECIFIC!!! (ATmega169)
#define LCD_CONTRAST_LEVEL(level) LCDCCR=(0x0F & level)// DEVICE SPECIFIC!!! (ATmega169) First LCD segment register

//COMPUTER SCIENCE SS2012
#define ACTIVATE_LCD LCD_Init ();
///It initializes the LCD module and set the cursor at the first position of the display 
#define CLEAR_LCD LCD_Clear();
///It clears the LCD display
#define LCD_CHAR(character) LCD_putc(character);
///It writes a character on the LCD. The parameter must be constant or a variable declared as VAR(character).
#define LCD_TEXT(pStr) LCD_puts(pStr);
///It writes on the LCD a string constant or a string variable declared as as STRING(<string>, <max>). 
#define LCD_NUMBER(v) {itoa(v,lcd_str,10); LCD_puts(lcd_str);};
///It writes a number on the LCD. The number is a constant or a variable declared as VARIABLE(<name>) or VAR(<name>).
#define LCD_D_NUMBER(v,w,p) {dtostrf((v),w,p,lcd_str); LCD_puts(lcd_str);};
///It writes a number formatted fractional.
/**Parameters:
	\param[in] v numerical value to be displayed . The value must be of data type double. A constant like 3.14 or a variable declared as double x.
	\param[in] w field width of the output string (including the . and the possible sign for negative values)
	\param[in] p number of digits after the decimal sign
*/
																			//in the format "[-]d.ddd" on the LCD

