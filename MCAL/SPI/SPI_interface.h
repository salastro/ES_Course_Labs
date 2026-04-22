#ifndef SPI_INTERFACE_H
#define SPI_INTERFACE_H

#include "../../SERVICES/STD_TYPES.h"

/* SPI Modes */
#define SPI_MASTER 0
#define SPI_SLAVE 1

/* SPI Clock Prescaler */
#define SPI_CLK_FOSC_4 0
#define SPI_CLK_FOSC_16 1
#define SPI_CLK_FOSC_64 2
#define SPI_CLK_OSC_TMR2 3

/* SPI Clock Polarity */
#define SPI_CLK_IDLE_LOW 0
#define SPI_CLK_IDLE_HIGH 1

/* SPI Clock Phase */
#define SPI_SAMPLE_MIDDLE 0
#define SPI_SAMPLE_END 1

/* Function Pointers */
typedef void (*SPI_CallbackFuncPtr)(u8 Data);

/* Function Prototypes */
void SPI_Init(u8 Mode, u8 Prescaler, u8 ClockPolarity, u8 ClockPhase);
void SPI_SendByte(u8 Data);
u8 SPI_ReceiveByte(void);
u8 SPI_TransceiveByte(u8 Data);
void SPI_SetCallback(SPI_CallbackFuncPtr Copy_Ptr);
u8 SPI_GetStatus(void);
void SPI_InterruptHandler(void); /* Called from INT_Manager */
#endif
