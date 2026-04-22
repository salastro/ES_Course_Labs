#ifndef INT_INTERFACE_H
#define INT_INTERFACE_H

#include "../../SERVICES/STD_TYPES.h"

#define INT0 0

#define INT_RISING_EDGE 0
#define INT_FALLING_EDGE 1

#define INT_ENABLE 1
#define INT_DISABLE 0

typedef void (*INT_CallbackFuncPtr)(void);

void INT_Init(u8 IntEdge);
void INT_Enable(void);
void INT_Disable(void);
void INT_SetCallback(INT_CallbackFuncPtr Copy_Ptr);
void INT0_InterruptHandler(void);

#endif
