#include "CAR_config.h"
#include "CAR_interface.h"
#include "../../MCAL/TIMER0/TIMER0_interface.h"

void Car_Accelerate(DCMOTOR_t *LeftMotor, DCMOTOR_t *RightMotor, u8 *CurrentSpeed, const u8 *MotorSpeed)
{
    static u16 LastAccelerationMs = 0U;
    u16 CurrentMs = TIMER0_GetSystemMs();

    /* Only apply acceleration every 5ms */
    if ((CurrentMs - LastAccelerationMs) < CAR_ACCELERATION_DELAY_MS)
    {
        return;
    }

    LastAccelerationMs = CurrentMs;

    u8 NewSpeed = *CurrentSpeed;

    if (*CurrentSpeed < *MotorSpeed)
    {
        if (*CurrentSpeed < *MotorSpeed - CAR_ACCELERATION_STEP)
        {
            NewSpeed = *CurrentSpeed + CAR_ACCELERATION_STEP;
        }
        else
        {
            NewSpeed = *MotorSpeed;
        }
    }
    else if (*CurrentSpeed > *MotorSpeed)
    {
        if (*CurrentSpeed > CAR_ACCELERATION_STEP)
        {
            NewSpeed = *CurrentSpeed - CAR_ACCELERATION_STEP;
        }
        else
        {
            NewSpeed = 0U;
        }
    }

    /* Only write to motors if speed actually changed */
    if (NewSpeed != *CurrentSpeed)
    {
        *CurrentSpeed = NewSpeed;
        DCMOTOR_Forward(LeftMotor, NewSpeed);
        DCMOTOR_Forward(RightMotor, NewSpeed);
    }
}

void ULTRASONIC_Debounce(ULTRASONIC_t *Sensor, u16 *DistanceCm, u8 *DebounceCounter)
{
    u16 NewDistance = ULTRASONIC_GetDistanceCm(Sensor);

    if (NewDistance != 0U && NewDistance <= CAR_OBSTACLE_DISTANCE_CM)
    {
        if (*DebounceCounter < CAR_ULTRASONIC_DEBOUNCE_MAX)
        {
            (*DebounceCounter)++;
        }
    }
    else
    {
        if (*DebounceCounter > 0U)
        {
            (*DebounceCounter)--;
        }
    }

    if (*DebounceCounter >= CAR_ULTRASONIC_DEBOUNCE_THRESHOLD)
    {
        *DistanceCm = NewDistance;
    }
}
