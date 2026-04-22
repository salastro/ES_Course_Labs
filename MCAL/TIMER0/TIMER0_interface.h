#ifndef TIMER0_INTERFACE_H
#define TIMER0_INTERFACE_H

#include "../../SERVICES/STD_TYPES.h"

/* Timer0 Modes */
#define TIMER0_TIMER_MODE 0
#define TIMER0_COUNTER_MODE 1

/* Timer0 Prescaler Values */
#define TIMER0_PRESCALER_1 0
#define TIMER0_PRESCALER_2 1
#define TIMER0_PRESCALER_4 2
#define TIMER0_PRESCALER_8 3
#define TIMER0_PRESCALER_16 4
#define TIMER0_PRESCALER_32 5
#define TIMER0_PRESCALER_64 6
#define TIMER0_PRESCALER_256 7

/* Timer0 Interrupt Enable/Disable */
#define TIMER0_INT_ENABLE 1
#define TIMER0_INT_DISABLE 0

/* Function Pointers */
typedef void (*TIMER0_CallbackFuncPtr)(void);

/* Function Prototypes */
void TIMER0_Init(u8 Mode, u8 Prescaler, u8 IntEnable);
void TIMER0_SetValue(u8 TimerValue);
u8 TIMER0_GetValue(void);
void TIMER0_SetCallback(TIMER0_CallbackFuncPtr Copy_Ptr);
void TIMER0_Start(void);
void TIMER0_Stop(void);
void TIMER0_InterruptHandler(void); /* Called from INT_Manager */
#endif
