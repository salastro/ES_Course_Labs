#ifndef PWM_INTERFACE_H
#define PWM_INTERFACE_H

#include "../../SERVICES/STD_TYPES.h"

/* PWM Channels */
#define PWM_CHANNEL1 1
#define PWM_CHANNEL2 2

/* PWM Frequency Configuration */
#define PWM_FREQ_1kHz 0
#define PWM_FREQ_2kHz 1
#define PWM_FREQ_3kHz 2
#define PWM_FREQ_5kHz 3
#define PWM_FREQ_10kHz 4
#define PWM_FREQ_20kHz 5

/* Function Prototypes */
void PWM_Init(u8 Channel, u8 Frequency);
void PWM_SetDutyCycle(u8 Channel, u8 DutyCycle);
u8 PWM_GetDutyCycle(u8 Channel);
void PWM_Start(u8 Channel);
void PWM_Stop(u8 Channel);

#endif
