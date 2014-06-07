
#ifndef __DATAFLASH_INCLUDED
#define __DATAFLASH_INCLUDED
#endif


//General macro definitions
#define sbi(port,bit)	(port |=  (1<<bit))
#define cbi(port,bit)	(port &= ~(1<<bit))

#define SetBit(x,y)		(x |= (y))
#define ClrBit(x,y)		(x &=~(y))
#define ChkBit(x,y)		(x  & (y))


//Dataflash opcodes
#define FlashPageRead				0x52	// Main memory page read
#define FlashToBuf1Transfer 		0x53	// Main memory page to buffer 1 transfer
#define Buf1Read					0x54	// Buffer 1 read
#define FlashToBuf2Transfer 		0x55	// Main memory page to buffer 2 transfer
#define Buf2Read					0x56	// Buffer 2 read
#define StatusReg					0x57	// Status register
#define AutoPageReWrBuf1			0x58	// Auto page rewrite through buffer 1
#define AutoPageReWrBuf2			0x59	// Auto page rewrite through buffer 2
#define FlashToBuf1Compare    		0x60	// Main memory page to buffer 1 compare
#define FlashToBuf2Compare	    	0x61	// Main memory page to buffer 2 compare
#define ContArrayRead				0x68	// Continuous Array Read (Note : Only A/B-parts supported)
#define FlashProgBuf1				0x82	// Main memory page program through buffer 1
#define Buf1ToFlashWE   			0x83	// Buffer 1 to main memory page program with built-in erase
#define Buf1Write					0x84	// Buffer 1 write
#define FlashProgBuf2				0x85	// Main memory page program through buffer 2
#define Buf2ToFlashWE   			0x86	// Buffer 2 to main memory page program with built-in erase
#define Buf2Write					0x87	// Buffer 2 write
#define Buf1ToFlash     			0x88	// Buffer 1 to main memory page program without built-in erase
#define Buf2ToFlash		         	0x89	// Buffer 2 to main memory page program without built-in erase



//Pin definitions for interface to the Dataflash


//Dataflash macro definitions
#define DF_CS_active	cbi(PORTB,0)
#define DF_CS_inactive	sbi(PORTB,0)


//Function definitions
void DF_SPI_init (void);
unsigned char DF_SPI_RW (unsigned char output);
unsigned char Read_DF_status (void);
void Page_To_Buffer (unsigned int PageAdr, unsigned char BufferNo);
unsigned char Buffer_Read_Byte (unsigned char BufferNo, unsigned int IntPageAdr);
void Buffer_Read_Str (unsigned char BufferNo, unsigned int IntPageAdr, unsigned int No_of_bytes, unsigned char *BufferPtr);
void Buffer_Write_Enable (unsigned char BufferNo, unsigned int IntPageAdr);
void Buffer_Write_Byte (unsigned char BufferNo, unsigned int IntPageAdr, unsigned char Data);
void Buffer_Write_Str (unsigned char BufferNo, unsigned int IntPageAdr, unsigned int No_of_bytes, unsigned char *BufferPtr);
void Buffer_To_Page (unsigned char BufferNo, unsigned int PageAdr);
void Cont_Flash_Read_Enable (unsigned int PageAdr, unsigned int IntPageAdr);

//Macros

#include <string.h>

///MACROS FOR DATAFLASH BUTTERFLY

///DATAFLASH INIT

#define INIT_DF {DF_SPI_init(); Read_DF_status();}  ///Initialize DataFlash module and read the status of the DataFlash


///*************************WRITING MACROS*****************************///
#define WRITE_BYTE_BUFFER(IntPageAdr, Data) Buffer_Write_Byte (1, IntPageAdr, Data); ///Send a byte to a specific location in the buffer internal address
#define WRITE_STR_BUFFER(I, NB, string)  Buffer_Write_Str (1, I, NB, string); ///Send a string to a specific location in the buffer SRAM
/// **I is the address in the buffer, **NB is the number of bytes to be stolen, **string is the array e.g. a[n] ---- a shuld be entered. n=NB
#define BUFFER_TO_DF(PageAdr) Buffer_To_Page (1, PageAdr);   ///Send from the buffer to the dataflash in to a given Pageaddress
/// Here you select the Page Address where you want to save in the dataflash

#define WRITING_CONT(IntPageAdr) Buffer_Write_Enable (1, unsigned int IntPageAdr); ///enable continues wrinting from the location IntPageAdr
///It writes only in the buffer.
//------->|NOTE|<----be sure to terminate the process in order to start new comands with the Dataflash

///**************************READING MACROS**************************///


#define DF_TO_BUFFER(PageAdr) Page_To_Buffer (PageAdr, 1);  ///Send the information from a given PageAddress of the dataflash to the buffer
/// Here you select the Page Address where you want to download information from the dataflash to the buffer
#define READ_STR_BUFFER(I, NB, string) Buffer_Read_Str (1, I, NB, string);   ///Read a string in a specific InternalAddress of the Buffer
/// **I is the address in the buffer, **NB is the number of bytes to be stolen, **string is the array e.g. a[n] ---- a shuld be entered. n=NB
#define READ_BYTE_BUFFER(IntPageAdr) Buffer_Read_Byte (1, IntPageAdr); ///Read a byte to a specific location in buffer SRAM
#define READING_CONT(PageAdr, IntPageAdr) Cont_Flash_Read_Enable (PageAdr, IntPageAdr); ///Enables continues reading from the dataflash
///reads directly from the dataflash page address (PageAdr) and page location (IntPageAdr)

///DATAFLASH END

#define END_DF DF_CS_inactive;	///Terminates the comunication with the dataflash

///It is only used the buffer 1 to facilitate and reduce the macros
//

// S. Reyes 

// *****************************[ End Of DATAFLASH.H ]*****************************

