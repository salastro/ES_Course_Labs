#include "IR_interface.h"
#include "IR_private.h"
#include "IR_config.h"

#include "../../MCAL/GPIO/GPIO_interface.h"

void IR_Init(IR_t *Sensor)
{
    if (Sensor == NULL_PTR)
    {
        return;
    }

    GPIO_SetPinDirection(Sensor->Port, Sensor->Pin, GPIO_INPUT);
}

u8 IR_GetRawValue(IR_t *Sensor)
{
    if (Sensor == NULL_PTR)
    {
        return GPIO_LOW;
    }

    return GPIO_GetPinValue(Sensor->Port, Sensor->Pin);
}

static u8 IR_IsActiveLevel(IR_t *Sensor)
{
    u8 Local_Value = IR_GetRawValue(Sensor);

    if (IR_ACTIVE_STATE == IR_ACTIVE_LOW)
    {
        return (Local_Value == GPIO_LOW) ? 1U : 0U;
    }

    return (Local_Value == GPIO_HIGH) ? 1U : 0U;
}

u8 IR_IsDarkSurface(IR_t *Sensor)
{
    return IR_IsActiveLevel(Sensor);
}

u8 IR_IsLightSurface(IR_t *Sensor)
{
    return (IR_IsActiveLevel(Sensor) == 0U) ? 1U : 0U;
}

u8 IR_IsTrackDetected(IR_t *Sensor)
{
    if (IR_TRACK_STATE == IR_DARK_SURFACE)
    {
        return IR_IsDarkSurface(Sensor);
    }

    return IR_IsLightSurface(Sensor);
}

u8 IR_IsObstacleDetected(IR_t *Sensor)
{
    return IR_IsTrackDetected(Sensor);
}