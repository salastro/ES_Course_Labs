#ifndef DC_MOTOR_INTERFACE_H
#define DC_MOTOR_INTERFACE_H

#include "../../SERVICES/STD_TYPES.h"

/* Motor direction states */
#define DCMOTOR_DIR_FORWARD 0
#define DCMOTOR_DIR_REVERSE 1
#define DCMOTOR_DIR_STOP 2
#define DCMOTOR_DIR_BRAKE 3

/* Enable mode */
#define DCMOTOR_NO_PWM 0
#define DCMOTOR_USE_PWM 1

typedef struct
{
    u8 In1Port;
    u8 In1Pin;
    u8 In2Port;
    u8 In2Pin;
    u8 EnPort;
    u8 EnPin;
    u8 UsePwm;
    u8 PwmChannel;
} DCMOTOR_t;

void DCMOTOR_Init(DCMOTOR_t *Motor);
void DCMOTOR_SetDirection(DCMOTOR_t *Motor, u8 Direction);
void DCMOTOR_SetSpeed(DCMOTOR_t *Motor, u8 SpeedPercent);
void DCMOTOR_Forward(DCMOTOR_t *Motor, u8 SpeedPercent);
void DCMOTOR_Reverse(DCMOTOR_t *Motor, u8 SpeedPercent);
void DCMOTOR_Stop(DCMOTOR_t *Motor);
void DCMOTOR_Brake(DCMOTOR_t *Motor);

#endif
