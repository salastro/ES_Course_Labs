#ifndef I2C_INTERFACE_H
#define I2C_INTERFACE_H

#include "../../SERVICES/STD_TYPES.h"

/* I2C Modes */
#define I2C_MASTER 0
#define I2C_SLAVE 1

/* I2C Clock Speeds */
#define I2C_SPEED_100kHz 100000
#define I2C_SPEED_400kHz 400000
#define I2C_SPEED_1MHz 1000000

/* I2C Status */
#define I2C_IDLE 0
#define I2C_BUSY 1
#define I2C_ACK_RECEIVED 0
#define I2C_NACK_RECEIVED 1

/* Function Pointers */
typedef void (*I2C_CallbackFuncPtr)(u8 Data, u8 Status);

/* Function Prototypes */
void I2C_Init(u8 Mode, u32 ClockSpeed);
void I2C_StartCondition(void);
void I2C_StopCondition(void);
void I2C_SendByte(u8 Data);
u8 I2C_ReceiveByte(void);
u8 I2C_ReadAck(void);
void I2C_SendAck(void);
void I2C_SendNack(void);
void I2C_SetCallback(I2C_CallbackFuncPtr Copy_Ptr);
u8 I2C_GetStatus(void);
void I2C_InterruptHandler(void); /* Called from INT_Manager */
#endif
