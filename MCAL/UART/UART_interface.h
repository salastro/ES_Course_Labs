#ifndef UART_INTERFACE_H
#define UART_INTERFACE_H

#include "../../SERVICES/STD_TYPES.h"

/* UART Baud Rates */
#define UART_BAUD_1200 1200
#define UART_BAUD_2400 2400
#define UART_BAUD_4800 4800
#define UART_BAUD_9600 9600
#define UART_BAUD_14400 14400
#define UART_BAUD_19200 19200
#define UART_BAUD_28800 28800
#define UART_BAUD_38400 38400
#define UART_BAUD_57600 57600

/* UART Data Bits */
#define UART_DATA_8BITS 0
#define UART_DATA_9BITS 1

/* UART Stop Bits */
#define UART_STOP_1BIT 0
#define UART_STOP_2BITS 1

/* UART Parity */
#define UART_NO_PARITY 0
#define UART_EVEN_PARITY 1
#define UART_ODD_PARITY 2

/* Function Pointers */
typedef void (*UART_RX_CallbackFuncPtr)(u8 Data);
typedef void (*UART_TX_CallbackFuncPtr)(void);

/* Function Prototypes */
void UART_Init(u16 BaudRate, u8 DataBits, u8 StopBits);
void UART_SendByte(u8 Data);
u8 UART_ReceiveByte(void);
void UART_SendString(u8 *String);
void UART_SetRXCallback(UART_RX_CallbackFuncPtr Copy_Ptr);
void UART_SetTXCallback(UART_TX_CallbackFuncPtr Copy_Ptr);
u8 UART_GetRXStatus(void);
u8 UART_GetTXStatus(void);
void UART_InterruptHandler(void); /* Called from INT_Manager */
#endif
