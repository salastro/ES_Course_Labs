# MCU
MCU = 16F877A

# Compiler
CC = xc8-cc

# DFP
DFP = /opt/microchip/PIC16Fxxx_DFP/1.7.162/xc8

# Target
TARGET = app

# Source files
SRCS = \
APP/main.c \
HAL/LED/LED.c \
MCAL/GPIO/GPIO.c \
MCAL/INT/INT.c \
MCAL/INT_Manager/INT_Manager.c \
MCAL/TIMER0/TIMER0.c \
MCAL/PWM/PWM.c \
MCAL/ADC/ADC.c \
MCAL/UART/UART.c \
MCAL/SPI/SPI.c \
MCAL/I2C/I2C.c

# Include directories
INCLUDES = \
-IAPP \
-IHAL/LED \
-IMCAL/GPIO \
-IMCAL/INT \
-IMCAL/INT_Manager \
-IMCAL/TIMER0 \
-IMCAL/PWM \
-IMCAL/ADC \
-IMCAL/UART \
-IMCAL/SPI \
-IMCAL/I2C \
-ISERVICES

# Flags
CFLAGS = -mcpu=$(MCU) -mdfp=$(DFP) $(INCLUDES) -Wall -Wextra -Werror=conversion -Werror=unused-parameter

# Default target
all: $(TARGET).hex

# Build target
$(TARGET).hex: $(SRCS)
	$(CC) $(CFLAGS) -o $(TARGET).hex $(SRCS)

# Flash (PICkit2)
flash: $(TARGET).hex
	pk2cmd -P$(MCU) -F$(TARGET).hex -M

# Clean
clean:
	rm -f *.hex *.elf *.o *.d *.map *.lst *.cmf *.d *.elf *.hxl *.o *.p1 *.rlf *.s *.sdb *.sym

.PHONY: all flash clean
