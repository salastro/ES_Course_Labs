#include "ULTRASONIC_interface.h"
#include "ULTRASONIC_private.h"
#include "ULTRASONIC_config.h"

#include "../../MCAL/GPIO/GPIO_interface.h"
#include "../../MCAL/TIMER0/TIMER0_interface.h"
#include "../../MCAL/TIMER0/TIMER0_private.h"
#include "../../SERVICES/BIT_MATH.h"

/* Measurement state machine */
typedef enum
{
    ULTRASONIC_STATE_IDLE = 0,
    ULTRASONIC_STATE_TRIGGER_SENT = 1,
    ULTRASONIC_STATE_WAITING_ECHO = 2,
    ULTRASONIC_STATE_MEASURING = 3,
    ULTRASONIC_STATE_COMPLETE = 4
} ULTRASONIC_MeasurementState_t;

/* Measurement context for async measurement */
typedef struct
{
    ULTRASONIC_t *Sensor;
    ULTRASONIC_MeasurementState_t State;
    u16 EchoTicks;
    u16 Timeout;
    u16 DistanceCm;
    u8 IsValid;
} ULTRASONIC_MeasurementContext_t;

/* Sensor result storage */
static u16 ULTRASONIC_FrontDistanceCm = 0U;
static u16 ULTRASONIC_BackDistanceCm = 0U;
static u16 ULTRASONIC_LeftDistanceCm = 0U;
static u16 ULTRASONIC_RightDistanceCm = 0U;
static u16 *ULTRASONIC_DistanceArray[ULTRASONIC_SENSOR_COUNT] = {
    &ULTRASONIC_FrontDistanceCm,
    &ULTRASONIC_BackDistanceCm,
    &ULTRASONIC_LeftDistanceCm,
    &ULTRASONIC_RightDistanceCm};

/* Current measurement context */
static ULTRASONIC_MeasurementContext_t ULTRASONIC_CurrentMeasurement;

/* Sensor index for cycling through sensors (0-3) */
static u8 ULTRASONIC_SensorIndex = 0U;

/* Storage for sensor pointers for cycling */
static ULTRASONIC_t *ULTRASONIC_SensorArray[ULTRASONIC_SENSOR_COUNT];

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

/* Non-blocking measurement handler called periodically by TIMER0 interrupt */
static void ULTRASONIC_MeasurementProcess(void)
{
    ULTRASONIC_MeasurementContext_t *Ctx = &ULTRASONIC_CurrentMeasurement;

    if (Ctx->Sensor == NULL_PTR)
        return;

    switch (Ctx->State)
    {
    case ULTRASONIC_STATE_IDLE:
        /* Send trigger pulse */
        ULTRASONIC_SendTrigger(Ctx->Sensor);
        Ctx->State = ULTRASONIC_STATE_TRIGGER_SENT;
        Ctx->Timeout = 0U;
        Ctx->EchoTicks = 0U;
        break;

    case ULTRASONIC_STATE_TRIGGER_SENT:
        /* Wait for echo line to go high */
        if (GPIO_GetPinValue(Ctx->Sensor->EchoPort, Ctx->Sensor->EchoPin) == GPIO_HIGH)
        {
            Ctx->State = ULTRASONIC_STATE_MEASURING;
            TMR0 = 0U;
            CLR_BIT(INTCON, T0IF);
        }
        else if (Ctx->Timeout++ > ULTRASONIC_TIMEOUT_TICKS)
        {
            /* Timeout waiting for echo start */
            Ctx->DistanceCm = 0U;
            Ctx->IsValid = 0U;
            Ctx->State = ULTRASONIC_STATE_COMPLETE;
        }
        break;

    case ULTRASONIC_STATE_MEASURING:
        /* Measure echo pulse width */
        if (GPIO_GetPinValue(Ctx->Sensor->EchoPort, Ctx->Sensor->EchoPin) == GPIO_LOW)
        {
            /* Echo ended, calculate distance */
            if (GET_BIT(INTCON, T0IF))
            {
                Ctx->EchoTicks = 256U;
                CLR_BIT(INTCON, T0IF);
            }
            Ctx->EchoTicks += TMR0;

            if (Ctx->EchoTicks > 0U)
            {
                Ctx->DistanceCm = (u16)(Ctx->EchoTicks / 29U);
                Ctx->IsValid = 1U;
            }
            else
            {
                Ctx->DistanceCm = 0U;
                Ctx->IsValid = 0U;
            }
            Ctx->State = ULTRASONIC_STATE_COMPLETE;
        }
        else if (GET_BIT(INTCON, T0IF))
        {
            /* Timer overflow, accumulate ticks */
            Ctx->EchoTicks += 256U;
            CLR_BIT(INTCON, T0IF);

            /* Timeout if measurement takes too long */
            if (Ctx->EchoTicks > ULTRASONIC_TIMEOUT_TICKS)
            {
                Ctx->DistanceCm = 0U;
                Ctx->IsValid = 0U;
                Ctx->State = ULTRASONIC_STATE_COMPLETE;
            }
        }
        break;

    case ULTRASONIC_STATE_COMPLETE:
        /* Measurement complete, wait for next cycle */
        break;

    default:
        Ctx->State = ULTRASONIC_STATE_IDLE;
        break;
    }
}

/* Timer0 interrupt callback for cyclic sensor measurement */
static void ULTRASONIC_TimerCallback(void)
{
    /* Process current measurement */
    ULTRASONIC_MeasurementProcess();

    /* Check if measurement is complete */
    if (ULTRASONIC_CurrentMeasurement.State == ULTRASONIC_STATE_COMPLETE)
    {
        /* Store result via array indexing (faster than switch) */
        *ULTRASONIC_DistanceArray[ULTRASONIC_SensorIndex] = ULTRASONIC_CurrentMeasurement.DistanceCm;

        /* Move to next sensor */
        ULTRASONIC_SensorIndex++;
        if (ULTRASONIC_SensorIndex >= ULTRASONIC_SENSOR_COUNT)
        {
            ULTRASONIC_SensorIndex = 0U;
        }

        /* Start measurement on next sensor */
        ULTRASONIC_CurrentMeasurement.Sensor = ULTRASONIC_SensorArray[ULTRASONIC_SensorIndex];
        ULTRASONIC_CurrentMeasurement.State = ULTRASONIC_STATE_IDLE;
        ULTRASONIC_MeasurementProcess();
    }
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
}

void ULTRASONIC_InitMeasurementSystem(ULTRASONIC_t *FrontSensor, ULTRASONIC_t *BackSensor,
                                      ULTRASONIC_t *LeftSensor, ULTRASONIC_t *RightSensor)
{
    /* Store sensor pointers */
    ULTRASONIC_SensorArray[0] = FrontSensor;
    ULTRASONIC_SensorArray[1] = BackSensor;
    ULTRASONIC_SensorArray[2] = LeftSensor;
    ULTRASONIC_SensorArray[3] = RightSensor;

    /* Initialize measurement context */
    ULTRASONIC_CurrentMeasurement.Sensor = FrontSensor;
    ULTRASONIC_CurrentMeasurement.State = ULTRASONIC_STATE_IDLE;
    ULTRASONIC_CurrentMeasurement.EchoTicks = 0U;
    ULTRASONIC_CurrentMeasurement.Timeout = 0U;
    ULTRASONIC_CurrentMeasurement.DistanceCm = 0U;
    ULTRASONIC_CurrentMeasurement.IsValid = 0U;

    /* Configure TIMER0 for ~50ms interrupt (800 overflows with prescaler 8 at 16MHz) */
    /* This means each sensor is measured every ~200ms (50ms * 4 sensors) */
    TIMER0_Init(TIMER0_TIMER_MODE, TIMER0_PRESCALER_8, TIMER0_INT_ENABLE);
    TIMER0_SetValue(200U); /* Start from 200, so it overflows after 56 ticks = ~50ms */
    TIMER0_SetCallback(ULTRASONIC_TimerCallback);
}

/* Non-blocking function - returns latest measured distance */
u16 ULTRASONIC_GetDistanceCm(ULTRASONIC_t *Sensor)
{
    if (Sensor == NULL_PTR)
    {
        return 0U;
    }

    /* Determine sensor index and return cached value via array */
    if (Sensor == ULTRASONIC_SensorArray[0])
        return ULTRASONIC_FrontDistanceCm;
    if (Sensor == ULTRASONIC_SensorArray[1])
        return ULTRASONIC_BackDistanceCm;
    if (Sensor == ULTRASONIC_SensorArray[2])
        return ULTRASONIC_LeftDistanceCm;
    if (Sensor == ULTRASONIC_SensorArray[3])
        return ULTRASONIC_RightDistanceCm;

    return 0U;
}

u16 ULTRASONIC_GetDistanceMm(ULTRASONIC_t *Sensor)
{
    u16 Local_DistanceCm = ULTRASONIC_GetDistanceCm(Sensor);

    return (u16)(Local_DistanceCm * 10U);
}