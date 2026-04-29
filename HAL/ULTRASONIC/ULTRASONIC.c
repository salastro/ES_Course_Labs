#include "ULTRASONIC_interface.h"
#include "ULTRASONIC_private.h"
#include "ULTRASONIC_config.h"

#include "../../MCAL/GPIO/GPIO_interface.h"
#include "../../MCAL/TIMER0/TIMER0_interface.h"
#include "../../MCAL/TIMER0/TIMER0_private.h"
#include "../../SERVICES/BIT_MATH.h"

static void ULTRASONIC_DelayLoop(u16 Iterations)
{
    volatile u16 Local_I = 0;

    for (Local_I = 0; Local_I < Iterations; Local_I++)
    {
        ;
    }
}

static void ULTRASONIC_SendTrigger(ULTRASONIC_t *Sensor)
{
    GPIO_SetPinValue(Sensor->TriggerPort, Sensor->TriggerPin, GPIO_LOW);
    ULTRASONIC_DelayLoop(10U);
    GPIO_SetPinValue(Sensor->TriggerPort, Sensor->TriggerPin, GPIO_HIGH);
    ULTRASONIC_DelayLoop(60U);
    GPIO_SetPinValue(Sensor->TriggerPort, Sensor->TriggerPin, GPIO_LOW);
}

static u16 ULTRASONIC_MeasureEchoTicks(ULTRASONIC_t *Sensor)
{
    u16 Local_Timeout = 0;
    u16 Local_Ticks = 0;

    while (GPIO_GetPinValue(Sensor->EchoPort, Sensor->EchoPin) == GPIO_LOW)
    {
        if (Local_Timeout++ > ULTRASONIC_TIMEOUT_TICKS)
        {
            return 0U;
        }
    }

    TMR0 = 0;
    CLR_BIT(INTCON, T0IF);

    while (GPIO_GetPinValue(Sensor->EchoPort, Sensor->EchoPin) == GPIO_HIGH)
    {
        if (GET_BIT(INTCON, T0IF) != 0U)
        {
            Local_Ticks += 256U;
            CLR_BIT(INTCON, T0IF);
        }
    }

    Local_Ticks += TMR0;

    return Local_Ticks;
}

void ULTRASONIC_Init(ULTRASONIC_t *Sensor)
{
    if (Sensor == NULL_PTR)
    {
        return;
    }

    GPIO_SetPinDirection(Sensor->TriggerPort, Sensor->TriggerPin, GPIO_OUTPUT);
    GPIO_SetPinDirection(Sensor->EchoPort, Sensor->EchoPin, GPIO_INPUT);
    GPIO_SetPinValue(Sensor->TriggerPort, Sensor->TriggerPin, GPIO_LOW);

    TIMER0_Init(TIMER0_TIMER_MODE, TIMER0_PRESCALER_8, TIMER0_INT_DISABLE);
    CLR_BIT(INTCON, T0IE);
    CLR_BIT(INTCON, T0IF);
}

u16 ULTRASONIC_GetDistanceCm(ULTRASONIC_t *Sensor)
{
    u16 Local_Ticks = 0;

    if (Sensor == NULL_PTR)
    {
        return 0U;
    }

    ULTRASONIC_SendTrigger(Sensor);
    Local_Ticks = ULTRASONIC_MeasureEchoTicks(Sensor);

    if (Local_Ticks == 0U)
    {
        return 0U;
    }

    return (u16)(Local_Ticks / 29U);
}

u16 ULTRASONIC_GetDistanceMm(ULTRASONIC_t *Sensor)
{
    u16 Local_DistanceCm = ULTRASONIC_GetDistanceCm(Sensor);

    return (u16)(Local_DistanceCm * 10U);
}