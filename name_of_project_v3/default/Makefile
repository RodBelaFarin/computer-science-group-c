###############################################################################
# Makefile for the project Butterfly2014
###############################################################################

## General Flags
PROJECT = Butterfly2014
MCU = atmega169
TARGET = Butterfly2014.elf
CC = avr-gcc

## Options common to compile, link and assembly rules
COMMON = -mmcu=$(MCU)

## Compile options common for all C compilation units.
CFLAGS = $(COMMON)
CFLAGS += -Wall -gdwarf-2 -std=gnu99         -DF_CPU=2000000UL -O0 -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums
CFLAGS += -MD -MP -MT $(*F).o -MF dep/$(@F).d 

## Assembly specific flags
ASMFLAGS = $(COMMON)
ASMFLAGS += $(CFLAGS)
ASMFLAGS += -x assembler-with-cpp -Wa,-gdwarf2

## Linker flags
LDFLAGS = $(COMMON)
LDFLAGS +=  -Wl,-Map=Butterfly2014.map


## Intel Hex file production flags
HEX_FLASH_FLAGS = -R .eeprom -R .fuse -R .lock -R .signature

HEX_EEPROM_FLAGS = -j .eeprom
HEX_EEPROM_FLAGS += --set-section-flags=.eeprom="alloc,load"
HEX_EEPROM_FLAGS += --change-section-lma .eeprom=0 --no-change-warnings


## Include Directories
INCLUDES = -I"D:\Dokumente\Studium\Master\Computer Science\Project\google_code\computer-science-group-c\name_of_project_v3\default" 

## Objects that must be built in order to link
OBJECTS = AVRBFLY.o dataflash.o Macro_lcd.o uart.o name_of_project_v2.o RTC.o BCD.o 

## Objects explicitly added by the user
LINKONLYOBJECTS = 

## Build
all: $(TARGET) Butterfly2014.hex Butterfly2014.eep Butterfly2014.lss size

## Compile
AVRBFLY.o: ../AVRBFLY.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

dataflash.o: ../dataflash.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

Macro_lcd.o: ../Macro_lcd.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

uart.o: ../uart.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

name_of_project_v2.o: ../name_of_project_v2.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

RTC.o: ../RTC.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

BCD.o: ../BCD.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

##Link
$(TARGET): $(OBJECTS)
	 $(CC) $(LDFLAGS) $(OBJECTS) $(LINKONLYOBJECTS) $(LIBDIRS) $(LIBS) -o $(TARGET)

%.hex: $(TARGET)
	avr-objcopy -O ihex $(HEX_FLASH_FLAGS)  $< $@

%.eep: $(TARGET)
	-avr-objcopy $(HEX_EEPROM_FLAGS) -O ihex $< $@ || exit 0

%.lss: $(TARGET)
	avr-objdump -h -S $< > $@

size: ${TARGET}
	@echo
	@avr-size -C --mcu=${MCU} ${TARGET}

## Clean target
.PHONY: clean
clean:
	-rm -rf $(OBJECTS) Butterfly2014.elf dep/* Butterfly2014.hex Butterfly2014.eep Butterfly2014.lss Butterfly2014.map


## Other dependencies
-include $(shell mkdir dep 2>/dev/null) $(wildcard dep/*)

