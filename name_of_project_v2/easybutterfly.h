//
//  File........: easybutterfly.h
//
//  Author(s)...: Group02 
//
//  Target(s)...: ATmega169 
//
//  Compiler....: AVR Studio
//
//  Description.: AVR Butterfly Macros header file distributor 
//
//  Revisions...: 1.0
//
//  YYYYMMDD - VER. - COMMENT                                       - SIGN.
//
//  20120620 - 1.0  - Created                                       - KS
//
//

// ------ ACUAS general macros:  ------
#include "AVRBFLY.h"

// ------ LCD specific settings: ------
#include "Macro_lcd.h"

// ------ UART specific settings: ------
// 1.) Set baud rate (default is 1200):
 #define UART_BAUD_RATE 2400
// 2.) ACUAS UART macros:
 #include "uart.h"
