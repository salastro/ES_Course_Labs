#ifndef CAR_INTERFACE_H
#define CAR_INTERFACE_H

#include "../../SERVICES/STD_TYPES.h"

void CAR_Init(void);
void CAR_HandleCommand(u8 Command);
void CAR_Forward(void);
void CAR_Backward(void);
void CAR_Left(void);
void CAR_Right(void);
void CAR_Stop(void);
void CAR_Horn(void);

#endif
