#include "CAR_interface.h"
#include "CAR_private.h"
#include "CAR_config.h"

#include "../../HAL/LED/LED_interface.h"
#include "../../MCAL/GPIO/GPIO_interface.h"

static DCMOTOR_t LeftMotor = {
    CAR_LEFT_MOTOR_IN1_PORT, CAR_LEFT_MOTOR_IN1_PIN,
    CAR_LEFT_MOTOR_IN2_PORT, CAR_LEFT_MOTOR_IN2_PIN,
    CAR_LEFT_MOTOR_EN_PORT, CAR_LEFT_MOTOR_EN_PIN,
    DCMOTOR_USE_PWM,
    CAR_LEFT_MOTOR_PWM_CHANNEL};

static DCMOTOR_t RightMotor = {
    CAR_RIGHT_MOTOR_IN1_PORT, CAR_RIGHT_MOTOR_IN1_PIN,
    CAR_RIGHT_MOTOR_IN2_PORT, CAR_RIGHT_MOTOR_IN2_PIN,
    CAR_RIGHT_MOTOR_EN_PORT, CAR_RIGHT_MOTOR_EN_PIN,
    DCMOTOR_USE_PWM,
    CAR_RIGHT_MOTOR_PWM_CHANNEL};

static ULTRASONIC_t FrontSensor = {
    CAR_FRONT_SENSOR_TRIGGER_PORT, CAR_FRONT_SENSOR_TRIGGER_PIN,
    CAR_FRONT_SENSOR_ECHO_PORT, CAR_FRONT_SENSOR_ECHO_PIN};

static ULTRASONIC_t BackSensor = {
    CAR_BACK_SENSOR_TRIGGER_PORT, CAR_BACK_SENSOR_TRIGGER_PIN,
    CAR_BACK_SENSOR_ECHO_PORT, CAR_BACK_SENSOR_ECHO_PIN};

static ULTRASONIC_t RightSensor = {
    CAR_RIGHT_SENSOR_TRIGGER_PORT, CAR_RIGHT_SENSOR_TRIGGER_PIN,
    CAR_RIGHT_SENSOR_ECHO_PORT, CAR_RIGHT_SENSOR_ECHO_PIN};

static ULTRASONIC_t LeftSensor = {
    CAR_LEFT_SENSOR_TRIGGER_PORT, CAR_LEFT_SENSOR_TRIGGER_PIN,
    CAR_LEFT_SENSOR_ECHO_PORT, CAR_LEFT_SENSOR_ECHO_PIN};

static IR_t TrackSensor = {
    CAR_TRACK_SENSOR_PORT, CAR_TRACK_SENSOR_PIN};

static u8 CurrentMotorSpeed = CAR_DEFAULT_MOTOR_SPEED_PERCENT;

static void delay_ms(u16 Ms)
{
    unsigned int i;
    unsigned int j;

    for (i = 0U; i < Ms; i++)
    {
        for (j = 0U; j < 165U; j++)
        {
            __asm("nop");
        }
    }
}

static u8 CAR_ClampSpeed(u16 SpeedPercent)
{
    if (SpeedPercent > 100U)
    {
        return 100U;
    }

    return (u8)SpeedPercent;
}

static void CAR_SendUint(u16 Value)
{
    u8 Buffer[5];
    u8 Index = 0U;

    if (Value == 0U)
    {
        UART_SendByte('0');
        return;
    }

    while (Value > 0U)
    {
        Buffer[Index] = (u8)((Value % 10U) + '0');
        Value /= 10U;
        Index++;
    }

    while (Index > 0U)
    {
        Index--;
        UART_SendByte(Buffer[Index]);
    }
}

static void CAR_SendDistance(u16 DistanceCm)
{
    CAR_SendUint(DistanceCm);
    UART_SendString((u8 *)".00");
}

static void CAR_SendSensorCsv(void)
{
    u16 FrontDistanceCm;
    u16 BackDistanceCm;
    u16 LeftDistanceCm;
    u16 RightDistanceCm;

    FrontDistanceCm = ULTRASONIC_GetDistanceCm(&FrontSensor);
    BackDistanceCm = ULTRASONIC_GetDistanceCm(&BackSensor);
    LeftDistanceCm = ULTRASONIC_GetDistanceCm(&LeftSensor);
    RightDistanceCm = ULTRASONIC_GetDistanceCm(&RightSensor);

    CAR_SendDistance(FrontDistanceCm);
    UART_SendByte(',');
    CAR_SendDistance(BackDistanceCm);
    UART_SendByte(',');
    CAR_SendDistance(LeftDistanceCm);
    UART_SendByte(',');
    CAR_SendDistance(RightDistanceCm);
    UART_SendByte('\n');
}

static void CAR_SendTrackStatus(void)
{
    if (IR_IsTrackDetected(&TrackSensor) != 0U)
    {
        UART_SendString((u8 *)"ON_TRACK\n");
    }
    else
    {
        UART_SendString((u8 *)"OFF_TRACK\n");
    }
}

static void CAR_SetIdleIndicators(void)
{
    LED_Off(CAR_WHITE_LED_PORT, CAR_WHITE_LED_PIN);
    LED_Off(CAR_YELLOW_LED_LEFT_PORT, CAR_YELLOW_LED_LEFT_PIN);
    LED_Off(CAR_YELLOW_LED_RIGHT_PORT, CAR_YELLOW_LED_RIGHT_PIN);
    LED_Off(CAR_RED_LED_PORT, CAR_RED_LED_PIN);
}

void CAR_Forward(void)
{
    DCMOTOR_Forward(&LeftMotor, CurrentMotorSpeed);
    DCMOTOR_Forward(&RightMotor, CurrentMotorSpeed);
    LED_On(CAR_WHITE_LED_PORT, CAR_WHITE_LED_PIN);
    LED_Off(CAR_YELLOW_LED_LEFT_PORT, CAR_YELLOW_LED_LEFT_PIN);
    LED_Off(CAR_YELLOW_LED_RIGHT_PORT, CAR_YELLOW_LED_RIGHT_PIN);
    LED_Off(CAR_RED_LED_PORT, CAR_RED_LED_PIN);
}

void CAR_Backward(void)
{
    DCMOTOR_Reverse(&LeftMotor, CurrentMotorSpeed);
    DCMOTOR_Reverse(&RightMotor, CurrentMotorSpeed);
    LED_Off(CAR_WHITE_LED_PORT, CAR_WHITE_LED_PIN);
    LED_Off(CAR_YELLOW_LED_LEFT_PORT, CAR_YELLOW_LED_LEFT_PIN);
    LED_Off(CAR_YELLOW_LED_RIGHT_PORT, CAR_YELLOW_LED_RIGHT_PIN);
    LED_On(CAR_RED_LED_PORT, CAR_RED_LED_PIN);
}

void CAR_Left(void)
{
    DCMOTOR_Stop(&LeftMotor);
    DCMOTOR_Forward(&RightMotor, CurrentMotorSpeed);
    LED_On(CAR_YELLOW_LED_LEFT_PORT, CAR_YELLOW_LED_LEFT_PIN);
    LED_Off(CAR_YELLOW_LED_RIGHT_PORT, CAR_YELLOW_LED_RIGHT_PIN);
    LED_Off(CAR_WHITE_LED_PORT, CAR_WHITE_LED_PIN);
    LED_Off(CAR_RED_LED_PORT, CAR_RED_LED_PIN);
}

void CAR_Right(void)
{
    DCMOTOR_Forward(&LeftMotor, CurrentMotorSpeed);
    DCMOTOR_Stop(&RightMotor);
    LED_On(CAR_YELLOW_LED_RIGHT_PORT, CAR_YELLOW_LED_RIGHT_PIN);
    LED_Off(CAR_YELLOW_LED_LEFT_PORT, CAR_YELLOW_LED_LEFT_PIN);
    LED_Off(CAR_WHITE_LED_PORT, CAR_WHITE_LED_PIN);
    LED_Off(CAR_RED_LED_PORT, CAR_RED_LED_PIN);
}

void CAR_Stop(void)
{
    DCMOTOR_Stop(&LeftMotor);
    DCMOTOR_Stop(&RightMotor);
    CAR_SetIdleIndicators();
}

void CAR_Horn(void)
{
    LED_On(CAR_RED_LED_PORT, CAR_RED_LED_PIN);
    delay_ms(CAR_HORN_PULSE_MS);
    LED_Off(CAR_RED_LED_PORT, CAR_RED_LED_PIN);
}

void CAR_Init(void)
{
    GPIO_Init();
    UART_Init(UART_BAUD_9600, UART_DATA_8BITS, UART_STOP_1BIT);

    LED_Init(CAR_RED_LED_PORT, CAR_RED_LED_PIN);
    LED_Init(CAR_YELLOW_LED_RIGHT_PORT, CAR_YELLOW_LED_RIGHT_PIN);
    LED_Init(CAR_YELLOW_LED_LEFT_PORT, CAR_YELLOW_LED_LEFT_PIN);
    LED_Init(CAR_WHITE_LED_PORT, CAR_WHITE_LED_PIN);

    CAR_SetIdleIndicators();

    DCMOTOR_Init(&LeftMotor);
    DCMOTOR_Init(&RightMotor);
    IR_Init(&TrackSensor);
    ULTRASONIC_Init(&FrontSensor);
    ULTRASONIC_Init(&BackSensor);
    ULTRASONIC_Init(&RightSensor);
    ULTRASONIC_Init(&LeftSensor);

    CAR_Stop();
}

void CAR_HandleCommand(u8 Command)
{
    if (Command == 'Q')
    {
        CAR_SendSensorCsv();
    }
    else if (Command == 'T')
    {
        CAR_SendTrackStatus();
    }
    else if ((Command >= '0') && (Command <= '9'))
    {
        CurrentMotorSpeed = CAR_ClampSpeed((u16)(Command - '0') * 10U);
    }
    else
    {
        switch (Command)
        {
        case 'F':
            CAR_Forward();
            break;

        case 'B':
            CAR_Backward();
            break;

        case 'L':
            CAR_Left();
            break;

        case 'R':
            CAR_Right();
            break;

        case 'H':
            CAR_Horn();
            break;

        case 'S':
        default:
            CAR_Stop();
            break;
        }
    }
}
