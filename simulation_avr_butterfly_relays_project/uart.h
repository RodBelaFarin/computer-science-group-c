#ifndef UART_H
#define UART_H
/************************************************************************
Title:    Interrupt UART library with receive/transmit circular buffers
Author:   Peter Fleury <pfleury@gmx.ch> 
			tested on ATmega169-AVRButerfly by C.Cruz and R.Gutierrez
File:     uart.h version 1.0
Hardware: any AVR with built-in UART
************************************************************************/
/**
 *  @brief Interrupt UART library using the built-in UART with transmit and receive circular buffers.
 *  This library can be used to transmit and receive data through the built in UART.
 *  An interrupt is generated when the UART has finished transmitting or
 *  receiving a byte. The interrupt handling routines use circular buffers
 *  for buffering received and transmitted data.
 *
 *  The UART_RX_BUFFER_SIZE and UART_TX_BUFFER_SIZE constants define
 *  the size of the circular buffers in bytes. Note that these constants must be a power of 2.
 *  You may need to adapt this constants to your target and your application by adding
 *  CDEFS += -DUART_RX_BUFFER_SIZE=nn -DUART_RX_BUFFER_SIZE=nn to your Makefile.
 *
 *  @author Peter Fleury pfleury@gmx.ch tested by C.Cruz and R.Gutierrez
 */

/**@{*/

#if (__GNUC__ * 100 + __GNUC_MINOR__) < 304
#error "This library requires AVR-GCC 3.4 or later, update to newer AVR-GCC compiler !"
#endif

/*
** constants and macros
*/

/** @brief  UART Baudrate Expression
 *  @param  xtalCpu  system clock in Mhz, e.g. 4000000L for 4Mhz
 *  @param  baudRate baudrate in bps, e.g. 1200, 2400, 9600
 */
#define UART_BAUD_SELECT(baudRate,xtalCpu) ((xtalCpu)/((baudRate)*16l)-1)

/** @brief  UART Baudrate Expression for ATmega double speed mode
 *  @param  xtalCpu  system clock in Mhz, e.g. 4000000L for 4Mhz
 *  @param  baudRate baudrate in bps, e.g. 1200, 2400, 9600
 */
#define UART_BAUD_SELECT_DOUBLE_SPEED(baudRate,xtalCpu) (((xtalCpu)/((baudRate)*8l)-1)|0x8000)


/** Size of the circular receive buffer, must be power of 2 */
#ifndef UART_RX_BUFFER_SIZE
#define UART_RX_BUFFER_SIZE 128
#endif
/** Size of the circular transmit buffer, must be power of 2 */
#ifndef UART_TX_BUFFER_SIZE
#define UART_TX_BUFFER_SIZE 32
#endif

/* test if the size of the circular buffers fits into SRAM */
#if ( (UART_RX_BUFFER_SIZE+UART_TX_BUFFER_SIZE) >= (RAMEND-0x60 ) )
#error "size of UART_RX_BUFFER_SIZE + UART_TX_BUFFER_SIZE larger than size of SRAM"
#endif

/*
** high byte error return code of uart_getc()
*/
#define UART_FRAME_ERROR      0x0800              /* Framing Error by UART       */
#define UART_OVERRUN_ERROR    0x0400              /* Overrun condition by UART   */
#define UART_BUFFER_OVERFLOW  0x0200              /* receive ringbuffer overflow */
#define UART_NO_DATA          0x0100              /* no receive data available   */


/*
** function prototypes
*/

/**
   @brief   Initialize UART and set baudrate
   @param   baudrate Specify baudrate using macro UART_BAUD_SELECT()
   @return  none
*/
extern void uart_init(unsigned int baudrate);


/**
 *  @brief   Get received byte from ringbuffer
 *
 * Returns in the lower byte the received character and in the
 * higher byte the last receive error.
 * UART_NO_DATA is returned when no data is available.
 *
 *  @return  lower byte:  received byte from ringbuffer
 *  @return  higher byte: last receive status
 *           - \b 0 successfully received data from UART
 *           - \b UART_NO_DATA
 *             <br>no receive data available
 *           - \b UART_BUFFER_OVERFLOW
 *             <br>Receive ringbuffer overflow.
 *             We are not reading the receive buffer fast enough,
 *             one or more received character have been dropped
 *           - \b UART_OVERRUN_ERROR
 *             <br>Overrun condition by UART.
 *             A character already present in the UART UDR register was
 *             not read by the interrupt handler before the next character arrived,
 *             one or more received characters have been dropped.
 *           - \b UART_FRAME_ERROR
 *             <br>Framing Error by UART
 */
extern unsigned int uart_getc(void);


/**
 *  @brief   Put byte to ringbuffer for transmitting via UART
 *  @param   data byte to be transmitted
 *  @return  none
 */
extern void uart_putc(unsigned char data);


/**
 *  @brief   Put string to ringbuffer for transmitting via UART
 *
 *  The string is buffered by the uart library in a circular buffer
 *  and one character at a time is transmitted to the UART using interrupts.
 *  Blocks if it can not write the whole string into the circular buffer.
 *
 *  @param   s string to be transmitted
 *  @return  none
 */
extern void uart_puts(const char *s );


/**
 * @brief    Put string from program memory to ringbuffer for transmitting via UART.
 *
 * The string is buffered by the uart library in a circular buffer
 * and one character at a time is transmitted to the UART using interrupts.
 * Blocks if it can not write the whole string into the circular buffer.
 *
 * @param    s program memory string to be transmitted
 * @return   none
 * @see      uart_puts_P
 */
extern void uart_puts_p(const char *s );

/**
 * @brief    Macro to automatically put a string constant into program memory
 */
#define uart_puts_P(__s)       uart_puts_p(PSTR(__s))



/** @brief  Initialize USART1 (only available on selected ATmegas) @see uart_init */
extern void uart1_init(unsigned int baudrate);
/** @brief  Get received byte of USART1 from ringbuffer. (only available on selected ATmega) @see uart_getc */
extern unsigned int uart1_getc(void);
/** @brief  Put byte to ringbuffer for transmitting via USART1 (only available on selected ATmega) @see uart_putc */
extern void uart1_putc(unsigned char data);
/** @brief  Put string to ringbuffer for transmitting via USART1 (only available on selected ATmega) @see uart_puts */
extern void uart1_puts(const char *s );
/** @brief  Put string from program memory to ringbuffer for transmitting via USART1 (only available on selected ATmega) @see uart_puts_p */
extern void uart1_puts_p(const char *s );
/** @brief  Macro to automatically put a string constant into program memory */
#define uart1_puts_P(__s)       uart1_puts_p(PSTR(__s))

/**@}*/


#endif // UART_H



#ifndef ACUASUART_H
#define ACUASUART_H
/**
 @defgroup ACUAS_EPuart ACUAS Easy Programming UART
 @brief C macros to easily program UART of the ACUAS ATmega16/ATmega32 board.
 @author Thomas Siepmann, Aachen University of Applied Sciences, Germany. Based on the UART library of Peter Fleury http://jump.to/fleury
 @see http://www.siepmann.fh-aachen.de
*/

#ifndef UART_BAUD_RATE
/// default baud rate
#define UART_BAUD_RATE 1200
#endif

#include <avr/interrupt.h>
#include <avr/pgmspace.h>
//#include "UART\uart.h"

/*@{*/

/// @file ACUASUART.h
/// @file ACUASUART.c

/// Global Variable used by UART_NUMBER to store the alphanumeric representation of the number
unsigned char uart_str[21];

void uart_read_line(unsigned char * c_array, uint8_t max, int try_sec);

/// Initialises the UART. Must be invoked in the PROGRAM_INIT part. Sets frame format: asynchronous, 8 data bits, no parity, 1 stop bit
#define INIT_UART {uart_init( UART_BAUD_SELECT(UART_BAUD_RATE,F_CPU) ); sei();}
/// Sends a string which is located in SRAM over UART
#define UART_TEXT(string) uart_puts((char*) string);
/// Sends a string which is located in flash ROM over UART. The string must be declared as: static const char PText[] PROGMEM = "text"
#define UART_TEXT_CONST(string) uart_puts_p(string);
/// Sends a number over UART
#define UART_NUMBER(x) {itoa( x, (char*)uart_str, 10); uart_puts((char*)uart_str);}
/// Sends one character over UART
#define UART_CHAR(c) uart_putc(c);
/// Sends a linefeed over UART
#define UART_CRLF uart_putc(13); uart_putc(10);
/// Reads one character from UART
#define UART_READ_CHAR(c) c = uart_getc();
///
/**
 *  @brief   Reads one line of text delimeted by a carriage return (ASCII 13). The function waits until a carriage return is received or max_char characters have been received
 *  @param   c_array a char-array which can contain at least max_char characters + 1
 *  @param   max_char the maximum amount of characters which are received if no carriage return is received.
 *  @return  none
 */
#define UART_READ_LINE(c_array,max_char) uart_read_line((c_array), (max_char), 3);

/*@}*/
#endif // ACUASUART_H
