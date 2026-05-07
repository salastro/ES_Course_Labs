#include "CAR_config.h"
#include "CAR_interface.h"
#include "../../MCAL/TIMER0/TIMER0_interface.h"

/* State variables for autonomous obstacle avoidance */
static DCMOTOR_t *CAR_LeftMotor = NULL_PTR;
static DCMOTOR_t *CAR_RightMotor = NULL_PTR;
static u8 *CAR_CurrentSpeed = NULL_PTR;
static const u8 *CAR_MaxSpeed = NULL_PTR;
static const u8 *CAR_MinSpeed = NULL_PTR;
static u8 *CAR_ObstacleState = NULL_PTR;
static ULTRASONIC_t *CAR_FrontSensor = NULL_PTR;
static ULTRASONIC_t *CAR_BackSensor = NULL_PTR;
static ULTRASONIC_t *CAR_LeftSensor = NULL_PTR;
static ULTRASONIC_t *CAR_RightSensor = NULL_PTR;
static void (*CAR_LEDCallback)(u8) = NULL_PTR;

/* Periodic obstacle detection task (runs via TIMER0 interrupt) */
static void CAR_ObstacleDetectionTask(void)
{
    u16 distances[4] = {
        ULTRASONIC_GetDistanceCm(CAR_FrontSensor),
        ULTRASONIC_GetDistanceCm(CAR_BackSensor),
        ULTRASONIC_GetDistanceCm(CAR_LeftSensor),
        ULTRASONIC_GetDistanceCm(CAR_RightSensor)};

    /* Find minimum non-zero distance */
    u16 ClosestDistanceCm = 0U;
    for (u8 i = 0; i < 4; i++)
    {
        if (distances[i] != 0U)
        {
            if (ClosestDistanceCm == 0U || distances[i] < ClosestDistanceCm)
            {
                ClosestDistanceCm = distances[i];
            }
        }
    }

    /* Obstacle detection and response */
    if ((ClosestDistanceCm != 0U) && (ClosestDistanceCm <= CAR_OBSTACLE_DISTANCE_CM))
    {
        /* Obstacle detected */
        if (*CAR_ObstacleState != CAR_OBSTACLE_STATE_DETECTED)
        {
            *CAR_ObstacleState = CAR_OBSTACLE_STATE_DETECTED;
            if (CAR_LEDCallback != NULL_PTR)
                CAR_LEDCallback(1U);
        }
        Car_Accelerate(CAR_LeftMotor, CAR_RightMotor, CAR_CurrentSpeed, CAR_MinSpeed);
    }
    else
    {
        /* Path clear */
        if (*CAR_ObstacleState != CAR_OBSTACLE_STATE_CLEAR)
        {
            *CAR_ObstacleState = CAR_OBSTACLE_STATE_CLEAR;
            if (CAR_LEDCallback != NULL_PTR)
                CAR_LEDCallback(0U);
        }
        Car_Accelerate(CAR_LeftMotor, CAR_RightMotor, CAR_CurrentSpeed, CAR_MaxSpeed);
    }
}

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

void Car_InitAutonomousMode(DCMOTOR_t *LeftMotor, DCMOTOR_t *RightMotor,
                            u8 *CurrentSpeed, const u8 *MaxSpeed, const u8 *MinSpeed,
                            u8 *ObstacleState,
                            ULTRASONIC_t *FrontSensor, ULTRASONIC_t *BackSensor,
                            ULTRASONIC_t *LeftSensor, ULTRASONIC_t *RightSensor,
                            void (*LEDCallback)(u8))
{
    /* Store pointers for interrupt-driven operation */
    CAR_LeftMotor = LeftMotor;
    CAR_RightMotor = RightMotor;
    CAR_CurrentSpeed = CurrentSpeed;
    CAR_MaxSpeed = MaxSpeed;
    CAR_MinSpeed = MinSpeed;
    CAR_ObstacleState = ObstacleState;
    CAR_FrontSensor = FrontSensor;
    CAR_BackSensor = BackSensor;
    CAR_LeftSensor = LeftSensor;
    CAR_RightSensor = RightSensor;
    CAR_LEDCallback = LEDCallback;

    /* Register obstacle detection as periodic task (runs every 100ms via TIMER0) */
    TIMER0_SetPeriodicTask(CAR_ObstacleDetectionTask, 100U);
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
