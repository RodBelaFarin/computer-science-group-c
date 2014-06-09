//*****************************************************************************
//
//  File........: RTC.c
//
//  Author(s)...: ATMEL Norway
//
//  Target(s)...: ATmega169
//
//  Compiler....: AVR-GCC 4.1.1; avr-libc 1.4.5
//
//  Description.: Real Time Clock (RTC)
//
//  Revisions...: 1.0
//
//  YYYYMMDD - VER. - COMMENT                                       - SIGN.
//
//  20021015 - 1.0  - Created                                       - LHM
//  20031009          port to avr-gcc/avr-libc                      - M.Thomas
//  20051107          minior correction (volatiles)                 - mt
//  20070129          SIGNAL->ISR                                   - mt
//*****************************************************************************

//mtA
#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

//mtE
//#include "main.h"
//#include "RTC.h"
//#include "LCD_functions.h"
#include "BCD.h"

#ifndef FALSE
#define FALSE   0
#define TRUE    (!FALSE)
#endif

#define sbiBF(port,bit)  (port |= (1<<bit))   //set bit in port
#define cbiBF(port,bit)  (port &= ~(1<<bit))  //clear bit in port

// mtA
//char gSECOND;
//char gMINUTE;
//char gHOUR;
//char gDAY;
//char gMONTH;
volatile uint8_t  gSECOND;
volatile uint8_t  gMINUTE;
volatile uint8_t  gHOUR;
volatile uint8_t  gDAY;
volatile uint8_t  gMONTH;
volatile uint16_t gYEAR;

//char gPowerSaveTimer = 0;
//char dateformat = 0;
volatile uint8_t gPowerSaveTimer = 0;



/*****************************************************************************
*
*   Function name : Delay
*
*   Returns :       None
*
*   Parameters :    unsigned int millisec
*
*   Purpose :       Delay-loop
*
*****************************************************************************/
void Delay(unsigned int millisec)
{
	// mt, int i did not work in the simulator:  int i; 
	uint8_t i;

	while (millisec--) {
		for (i=0; i<125; i++) {
			asm volatile ("nop"::);
		}
	}
}



/******************************************************************************
*
*   Function name:  RTC_init
*
*   returns:        none
*
*   parameters:     none
*
*   Purpose:        Start Timer/Counter2 in asynchronous operation using a
*                   32.768kHz crystal.
*
*******************************************************************************/


void RTC_init(void)
{
    Delay(1000);            // wait for 1 sec to let the Xtal stabilize after a power-on,

    cli(); // mt __disable_interrupt();  // disabel global interrupt

    cbiBF(TIMSK2, TOIE2);             // disable OCIE2A and TOIE2

    ASSR = (1<<AS2);        // select asynchronous operation of Timer2

    TCNT2 = 0;              // clear TCNT2A
    TCCR2A |= (1<<CS22) | (1<<CS20);             // select precaler: 32.768 kHz / 128 = 1 sec between each overflow

    while((ASSR & 0x01) | (ASSR & 0x04));       // wait for TCN2UB and TCR2UB to be cleared

    TIFR2 = 0xFF;           // clear interrupt-flags
    sbiBF(TIMSK2, TOIE2);     // enable Timer2 overflow interrupt

    sei(); // mt __enable_interrupt();                 // enable global interrupt

    // initial time and date setting
    gSECOND  = 0;
    gMINUTE  = 0;
    gHOUR    = 12;
    // mt release timestamp
    gDAY     = 12;
    gMONTH   = 3;
    gYEAR    = 9;
}


/*****************************************************************************
*
*   Function name : ShowClock
*
*   Returns :       char ST_state (to the state-machine)
*
*   Parameters :    char input (from joystick)
*
*   Purpose :       Shows the clock on the LCD
*
*****************************************************************************/
void ShowClock()
{
    //char HH, HL, MH, ML, SH, SL;
    uint8_t HH, HL, MH, ML, SH, SL;


    HH = CHAR2BCD2(gHOUR);
        
    HL = (HH & 0x0F) + '0';
    HH = (HH >> 4) + '0';

    MH = CHAR2BCD2(gMINUTE);
    ML = (MH & 0x0F) + '0';
    MH = (MH >> 4) + '0';

    SH = CHAR2BCD2(gSECOND);
    SL = (SH & 0x0F) + '0';
    SH = (SH >> 4) + '0';

    LCD_putc(0, HH);
    LCD_putc(1, HL);
    LCD_putc(2, MH);
    LCD_putc(3, ML);
    LCD_putc(4, SH);
    LCD_putc(5, SL);
    LCD_putc(6, '\0');

    LCD_Colon(1);

    LCD_UpdateRequired(TRUE, 0);
 
    return;
}

#define HOUR       0
#define MINUTE     1
#define SECOND     2

int get_second()
{
	return gSECOND;
}

int get_minute()
{
	return gMINUTE;
}

int get_hour()
{
	return gHOUR;
}	


/*****************************************************************************
*
*   Function name : SetClock
*
*   Returns :       char ST_state (to the state-machine)
*
*   Parameters :    char input (from joystick)
*
*   Purpose :       Adjusts the clock
*
*****************************************************************************/
void SetClock()
{
    static char enter_function = 1;
    // mtA
    // static char time[3];    // table holding the temporary clock setting
    // static char mode = HOUR;
    // char HH, HL, MH, ML, SH, SL;
    static uint8_t time[3];
    static uint8_t mode = HOUR;
    uint8_t HH, HL, MH, ML, SH, SL;
    // mtE

    if (enter_function)
    {
        time[HOUR] = gHOUR;
        time[MINUTE] = gMINUTE;
        time[SECOND] = gSECOND;
    }

    HH = CHAR2BCD2(time[HOUR]);
        
    HL = (HH & 0x0F) + '0';
    HH = (HH >> 4) + '0';

    MH = CHAR2BCD2(time[MINUTE]);
    ML = (MH & 0x0F) + '0';
    MH = (MH >> 4) + '0';

    SH = CHAR2BCD2(time[SECOND]);
    SL = (SH & 0x0F) + '0';
    SH = (SH >> 4) + '0';

    LCD_putc(0, HH | ((mode == HOUR) ? 0x80 : 0x00));
    LCD_putc(1, HL | ((mode == HOUR) ? 0x80 : 0x00));
    LCD_putc(2, MH | ((mode == MINUTE) ? 0x80 : 0x00));
    LCD_putc(3, ML | ((mode == MINUTE) ? 0x80 : 0x00));
    LCD_putc(4, SH | ((mode == SECOND) ? 0x80 : 0x00));
    LCD_putc(5, SL | ((mode == SECOND) ? 0x80 : 0x00));
    LCD_putc(6, '\0');

    LCD_Colon(1);

    if ((PINB & (1 << 4)))
        LCD_FlashReset();

    LCD_UpdateRequired(TRUE, 0);
    
    enter_function = 1;

    // Increment/decrement hours, minutes or seconds
    if (!(PINB & (1 << 6)))
        time[mode]++;
    else if (!(PINB & (1 << 7)))
        time[mode]--;
    else if (!(PINE & (1 << 2)))
    {
        if (mode == HOUR)
            mode = SECOND;
        else
            mode--;
    }
    else if (!(PINE & (1 << 3)))
    {
        if (mode == SECOND)
            mode = HOUR;
        else
            mode++;
    }
    else if (!(PINB & (1 << 4)))
    {
        // store the temporary adjusted values to the global variables
        cli(); // mt __disable_interrupt();
        gHOUR = time[HOUR];
        gMINUTE = time[MINUTE];
        gSECOND = time[SECOND];
        sei(); // mt __enable_interrupt();
        mode = HOUR;
        return;
    }

    /* OPTIMIZE: Can be solved by using a modulo operation */
    if (time[HOUR] == 255)
        time[HOUR] = 23;
    if (time[HOUR] > 23)
        time[HOUR] = 0;

    if (time[MINUTE] == 255)
        time[MINUTE] = 59;
    if (time[MINUTE] > 59)
        time[MINUTE] = 0;

    if (time[SECOND] == 255)
        time[SECOND] = 59;
    if (time[SECOND] > 59)
        time[SECOND] = 0;

    enter_function = 0;
    return;
}



/******************************************************************************
*
*   Timer/Counter2 Overflow Interrupt Routine
*
*   Purpose: Increment the real-time clock
*            The interrupt occurs once a second (running from the 32kHz crystal)
*
*******************************************************************************/
// mtA
// #pragma vector = TIMER2_OVF_vect
// __interrupt void TIMER2_OVF_interrupt(void)
// SIGNAL(SIG_OVERFLOW2)
ISR(TIMER2_OVF_vect)
// mtE
{

    gSECOND++;               // increment second

    if (gSECOND == 60)
    {
        gSECOND = 0;
        gMINUTE++;
        
        gPowerSaveTimer++;
        
        if (gMINUTE > 59)
        {
            gMINUTE = 0;
            gHOUR++;
            
            if (gHOUR > 23)
            {
                
                gHOUR = 0;
                gDAY++;
            }
        }
    }
}


