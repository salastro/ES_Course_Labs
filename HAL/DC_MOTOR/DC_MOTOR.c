#include "DC_MOTOR_interface.h"
#include "DC_MOTOR_private.h"
#include "DC_MOTOR_config.h"

#include "../../MCAL/GPIO/GPIO_interface.h"
#include "../../MCAL/PWM/PWM_interface.h"

static u8 DCMOTOR_ClampSpeed(u8 SpeedPercent)
{
    if (SpeedPercent > 100U)
    {
        return 100U;
    }

    return SpeedPercent;
}

void DCMOTOR_Init(DCMOTOR_t *Motor)
{
    if (Motor == NULL_PTR)
    {
        return;
    }

    GPIO_SetPinDirection(Motor->In1Port, Motor->In1Pin, GPIO_OUTPUT);
    GPIO_SetPinDirection(Motor->In2Port, Motor->In2Pin, GPIO_OUTPUT);
    GPIO_SetPinDirection(Motor->EnPort, Motor->EnPin, GPIO_OUTPUT);

    GPIO_SetPinValue(Motor->In1Port, Motor->In1Pin, GPIO_LOW);
    GPIO_SetPinValue(Motor->In2Port, Motor->In2Pin, GPIO_LOW);
    GPIO_SetPinValue(Motor->EnPort, Motor->EnPin, GPIO_LOW);

    if (Motor->UsePwm == DCMOTOR_USE_PWM)
    {
        PWM_Init(Motor->PwmChannel, DCMOTOR_PWM_DEFAULT_FREQUENCY);
        PWM_SetDutyCycle(Motor->PwmChannel, 0U);
        PWM_Start(Motor->PwmChannel);
    }
}

void DCMOTOR_SetDirection(DCMOTOR_t *Motor, u8 Direction)
{
    if (Motor == NULL_PTR)
    {
        return;
    }

    switch (Direction)
    {
    case DCMOTOR_DIR_FORWARD:
        GPIO_SetPinValue(Motor->In1Port, Motor->In1Pin, GPIO_HIGH);
        GPIO_SetPinValue(Motor->In2Port, Motor->In2Pin, GPIO_LOW);
        break;

    case DCMOTOR_DIR_REVERSE:
        GPIO_SetPinValue(Motor->In1Port, Motor->In1Pin, GPIO_LOW);
        GPIO_SetPinValue(Motor->In2Port, Motor->In2Pin, GPIO_HIGH);
        break;

    case DCMOTOR_DIR_BRAKE:
        GPIO_SetPinValue(Motor->In1Port, Motor->In1Pin, GPIO_HIGH);
        GPIO_SetPinValue(Motor->In2Port, Motor->In2Pin, GPIO_HIGH);
        break;

    case DCMOTOR_DIR_STOP:
    default:
        GPIO_SetPinValue(Motor->In1Port, Motor->In1Pin, GPIO_LOW);
        GPIO_SetPinValue(Motor->In2Port, Motor->In2Pin, GPIO_LOW);
        break;
    }
}

void DCMOTOR_SetSpeed(DCMOTOR_t *Motor, u8 SpeedPercent)
{
    u8 Local_Speed = 0;

    if (Motor == NULL_PTR)
    {
        return;
    }

    Local_Speed = DCMOTOR_ClampSpeed(SpeedPercent);

    if (Motor->UsePwm == DCMOTOR_USE_PWM)
    {
        PWM_SetDutyCycle(Motor->PwmChannel, Local_Speed);
    }
    else
    {
        if (Local_Speed == 0U)
        {
            GPIO_SetPinValue(Motor->EnPort, Motor->EnPin, GPIO_LOW);
        }
        else
        {
            GPIO_SetPinValue(Motor->EnPort, Motor->EnPin, GPIO_HIGH);
        }
    }
}

void DCMOTOR_Forward(DCMOTOR_t *Motor, u8 SpeedPercent)
{
    DCMOTOR_SetDirection(Motor, DCMOTOR_DIR_FORWARD);
    DCMOTOR_SetSpeed(Motor, SpeedPercent);
}

void DCMOTOR_Reverse(DCMOTOR_t *Motor, u8 SpeedPercent)
{
    DCMOTOR_SetDirection(Motor, DCMOTOR_DIR_REVERSE);
    DCMOTOR_SetSpeed(Motor, SpeedPercent);
}

void DCMOTOR_Stop(DCMOTOR_t *Motor)
{
    DCMOTOR_SetSpeed(Motor, 0U);
    DCMOTOR_SetDirection(Motor, DCMOTOR_DIR_STOP);
}

void DCMOTOR_Brake(DCMOTOR_t *Motor)
{
    DCMOTOR_SetDirection(Motor, DCMOTOR_DIR_BRAKE);

    if (Motor == NULL_PTR)
    {
        return;
    }

    if (Motor->UsePwm == DCMOTOR_USE_PWM)
    {
        PWM_SetDutyCycle(Motor->PwmChannel, 100U);
    }
    else
    {
        GPIO_SetPinValue(Motor->EnPort, Motor->EnPin, GPIO_HIGH);
    }
}
