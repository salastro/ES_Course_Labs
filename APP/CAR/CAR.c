#include "CAR_config.h"
#include "CAR_interface.h"

void Car_Accelerate(DCMOTOR_t *LeftMotor, DCMOTOR_t *RightMotor, u8 *CurrentSpeed, const u8 *MotorSpeed)
{
    if (*CurrentSpeed < *MotorSpeed)
    {
        if (*CurrentSpeed < *MotorSpeed - CAR_ACCELERATION_STEP)
        {
            *CurrentSpeed += CAR_ACCELERATION_STEP;
        }
        else
        {
            *CurrentSpeed = *MotorSpeed;
        }
        DCMOTOR_Forward(LeftMotor, *CurrentSpeed);
        DCMOTOR_Forward(RightMotor, *CurrentSpeed);
    }
    else if (*CurrentSpeed > *MotorSpeed)
    {
        if (*CurrentSpeed > CAR_ACCELERATION_STEP)
        {
            *CurrentSpeed -= CAR_ACCELERATION_STEP;
        }
        else
        {
            *CurrentSpeed = 0U;
        }
        DCMOTOR_Forward(LeftMotor, *CurrentSpeed);
        DCMOTOR_Forward(RightMotor, *CurrentSpeed);
    }
    else
    {
        // At cruise speed — only write if speed changed
        DCMOTOR_Forward(LeftMotor, *CurrentSpeed);
        DCMOTOR_Forward(RightMotor, *CurrentSpeed);
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
