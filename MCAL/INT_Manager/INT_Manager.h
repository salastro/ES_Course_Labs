#ifndef INT_MANAGER_H
#define INT_MANAGER_H

#include "../../SERVICES/STD_TYPES.h"

void IntManager_Init(void);

void INT0_InterruptHandler(void);
void TIMER0_InterruptHandler(void);
void ADC_InterruptHandler(void);
void UART_InterruptHandler(void);
void SPI_InterruptHandler(void);
void I2C_InterruptHandler(void);

#endif
