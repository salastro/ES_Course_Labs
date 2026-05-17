// Configuration bits for 16MHz HS oscillator
#pragma config FOSC = HS   // Oscillator Selection bits - HS oscillator (4-16 MHz)
#pragma config WDTE = OFF  // Watchdog Timer Enable bit - WDT disabled
#pragma config PWRTE = OFF // Power-up Timer Enable bit - PWRT disabled
#pragma config BOREN = OFF // Brown-out Reset Enable bit - BOR disabled
#pragma config LVP = OFF   // Low-Voltage Programming Enable bit - RB3/PGM pin for I/O
#pragma config CPD = OFF   // Data EEPROM Memory Code Protection bit - Code protection off
#pragma config WRT = OFF   // Flash Program Memory Write Enable bits - Write protection off
#pragma config CP = OFF    // Flash Program Memory Code Protection bit - Code protection off

#define _XTAL_FREQ 16000000

#include "../MCAL/GPIO/GPIO_interface.h"
#include "../HAL/DC_MOTOR/DC_MOTOR_interface.h"
#include "../HAL/IR/IR_interface.h"
#include "../HAL/ULTRASONIC/ULTRASONIC_interface.h"
#include "../MCAL/PWM/PWM_interface.h"
#include "../MCAL/UART/UART_interface.h"
#include "../SERVICES/STD_TYPES.h"
// #include <xc.h>

void delay_ms(u16 ms)
{
    unsigned int i, j;
    for (i = 0; i < ms; i++)
    {
        for (j = 0; j < 165; j++)
        {
            __asm("nop");
        }
    }
}

#define DEFAULT_MOTOR_SPEED_PERCENT 60U

static DCMOTOR_t LeftMotor = {
    GPIO_PORTD, GPIO_PIN0, // In1
    GPIO_PORTD, GPIO_PIN1, // In2
    GPIO_PORTC, GPIO_PIN2,
    DCMOTOR_USE_PWM,
    PWM_CHANNEL1};

static DCMOTOR_t RightMotor = {
    GPIO_PORTD, GPIO_PIN2, // In3
    GPIO_PORTD, GPIO_PIN3, // In4
    GPIO_PORTC, GPIO_PIN1,
    DCMOTOR_USE_PWM,
    PWM_CHANNEL2};

static ULTRASONIC_t FrontSensor = {
    GPIO_PORTB, GPIO_PIN4,  // Trigger
    GPIO_PORTB, GPIO_PIN5}; // Echo

static ULTRASONIC_t BackSensor = {
    GPIO_PORTB, GPIO_PIN2,  // Trigger
    GPIO_PORTB, GPIO_PIN3}; // Echo

static ULTRASONIC_t RightSensor = {
    GPIO_PORTB, GPIO_PIN6,  // Trigger
    GPIO_PORTB, GPIO_PIN7}; // Echo

static ULTRASONIC_t LeftSensor = {
    GPIO_PORTB, GPIO_PIN0,  // Trigger
    GPIO_PORTB, GPIO_PIN1}; // Echo

static IR_t TrackSensor = {
    GPIO_PORTA, GPIO_PIN4};

// RED LED (Stopping/slowing down)
#define RED_LED_PORT GPIO_PORTA
#define RED_LED_PIN GPIO_PIN0

// Yellow LED (Turn right)
#define YELLOW_LED_PORT_R GPIO_PORTA
#define YELLOW_LED_PIN_R GPIO_PIN1

// Yellow LED (Turn left)
#define YELLOW_LED_PORT_L GPIO_PORTA
#define YELLOW_LED_PIN_L GPIO_PIN2

// White LED (Front)
#define WHITE_LED_PORT GPIO_PORTA
#define WHITE_LED_PIN GPIO_PIN3

static u8 ClampSpeed(u16 SpeedPercent)
{
    if (SpeedPercent > 100U)
    {
        return 100U;
    }

    return (u8)SpeedPercent;
}

static void UART_SendUint(u16 Value)
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

static void UART_SendDistance(u16 DistanceCm)
{
    UART_SendUint(DistanceCm);
    UART_SendString((u8 *)".00");
}

static void UART_SendSensorCsv(void)
{
    u16 FrontDistanceCm = ULTRASONIC_GetDistanceCm(&FrontSensor);
    u16 BackDistanceCm = ULTRASONIC_GetDistanceCm(&BackSensor);
    u16 LeftDistanceCm = ULTRASONIC_GetDistanceCm(&LeftSensor);
    u16 RightDistanceCm = ULTRASONIC_GetDistanceCm(&RightSensor);

    UART_SendDistance(FrontDistanceCm);
    UART_SendByte(',');
    UART_SendDistance(BackDistanceCm);
    UART_SendByte(',');
    UART_SendDistance(LeftDistanceCm);
    UART_SendByte(',');
    UART_SendDistance(RightDistanceCm);
    UART_SendByte('\n');
}

static void UART_SendTrackStatus(void)
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

static void CAR_ApplyMotionCommand(u8 Command, u8 MotorSpeed)
{
    switch (Command)
    {
    case 'F':
        DCMOTOR_Forward(&LeftMotor, MotorSpeed);
        DCMOTOR_Forward(&RightMotor, MotorSpeed);
        GPIO_SetPinValue(WHITE_LED_PORT, WHITE_LED_PIN, GPIO_HIGH);
        GPIO_SetPinValue(YELLOW_LED_PORT_L, YELLOW_LED_PIN_L, GPIO_LOW);
        GPIO_SetPinValue(YELLOW_LED_PORT_R, YELLOW_LED_PIN_R, GPIO_LOW);
        GPIO_SetPinValue(RED_LED_PORT, RED_LED_PIN, GPIO_LOW);
        break;

    case 'B':
        DCMOTOR_Reverse(&LeftMotor, MotorSpeed);
        DCMOTOR_Reverse(&RightMotor, MotorSpeed);
        GPIO_SetPinValue(WHITE_LED_PORT, WHITE_LED_PIN, GPIO_LOW);
        GPIO_SetPinValue(YELLOW_LED_PORT_L, YELLOW_LED_PIN_L, GPIO_LOW);
        GPIO_SetPinValue(YELLOW_LED_PORT_R, YELLOW_LED_PIN_R, GPIO_LOW);
        GPIO_SetPinValue(RED_LED_PORT, RED_LED_PIN, GPIO_HIGH);
        break;

    case 'L':
        DCMOTOR_Stop(&LeftMotor);
        DCMOTOR_Forward(&RightMotor, MotorSpeed);
        GPIO_SetPinValue(YELLOW_LED_PORT_L, YELLOW_LED_PIN_L, GPIO_HIGH);
        GPIO_SetPinValue(YELLOW_LED_PORT_R, YELLOW_LED_PIN_R, GPIO_LOW);
        GPIO_SetPinValue(WHITE_LED_PORT, WHITE_LED_PIN, GPIO_LOW);
        GPIO_SetPinValue(RED_LED_PORT, RED_LED_PIN, GPIO_LOW);
        break;

    case 'R':
        DCMOTOR_Forward(&LeftMotor, MotorSpeed);
        DCMOTOR_Stop(&RightMotor);
        GPIO_SetPinValue(YELLOW_LED_PORT_R, YELLOW_LED_PIN_R, GPIO_HIGH);
        GPIO_SetPinValue(YELLOW_LED_PORT_L, YELLOW_LED_PIN_L, GPIO_LOW);
        GPIO_SetPinValue(WHITE_LED_PORT, WHITE_LED_PIN, GPIO_LOW);
        GPIO_SetPinValue(RED_LED_PORT, RED_LED_PIN, GPIO_LOW);
        break;

    case 'H':
        /* No dedicated horn output, use red LED as a visible horn indicator pulse. */
        GPIO_SetPinValue(RED_LED_PORT, RED_LED_PIN, GPIO_HIGH);
        delay_ms(80U);
        GPIO_SetPinValue(RED_LED_PORT, RED_LED_PIN, GPIO_LOW);
        break;

    case 'S':
    default:
        DCMOTOR_Stop(&LeftMotor);
        DCMOTOR_Stop(&RightMotor);
        GPIO_SetPinValue(WHITE_LED_PORT, WHITE_LED_PIN, GPIO_LOW);
        GPIO_SetPinValue(YELLOW_LED_PORT_L, YELLOW_LED_PIN_L, GPIO_LOW);
        GPIO_SetPinValue(YELLOW_LED_PORT_R, YELLOW_LED_PIN_R, GPIO_LOW);
        GPIO_SetPinValue(RED_LED_PORT, RED_LED_PIN, GPIO_LOW);
        break;
    }
}

int main(void)
{
    u8 MotorSpeed = DEFAULT_MOTOR_SPEED_PERCENT;
    u8 UART_Command = 'S';

    // Initialize GPIO, motors, and ultrasonic sensor
    GPIO_Init();

    // Initialize UART for PI
    UART_Init(UART_BAUD_9600, UART_DATA_8BITS, UART_STOP_1BIT);
    // UART_SetRXCallback(UART_CAR_SpeedCallback);

    // LED for debugging
    GPIO_SetPinDirection(RED_LED_PORT, RED_LED_PIN, GPIO_OUTPUT);
    GPIO_SetPinDirection(YELLOW_LED_PORT_R, YELLOW_LED_PIN_R, GPIO_OUTPUT);
    GPIO_SetPinDirection(YELLOW_LED_PORT_L, YELLOW_LED_PIN_L, GPIO_OUTPUT);
    GPIO_SetPinDirection(WHITE_LED_PORT, WHITE_LED_PIN, GPIO_OUTPUT);
    GPIO_SetPinValue(RED_LED_PORT, RED_LED_PIN, GPIO_LOW);
    GPIO_SetPinValue(YELLOW_LED_PORT_R, YELLOW_LED_PIN_R, GPIO_LOW);
    GPIO_SetPinValue(YELLOW_LED_PORT_L, YELLOW_LED_PIN_L, GPIO_LOW);
    GPIO_SetPinValue(WHITE_LED_PORT, WHITE_LED_PIN, GPIO_LOW);

    DCMOTOR_Init(&LeftMotor);
    DCMOTOR_Init(&RightMotor);
    IR_Init(&TrackSensor);
    ULTRASONIC_Init(&FrontSensor);
    ULTRASONIC_Init(&BackSensor);
    ULTRASONIC_Init(&RightSensor);
    ULTRASONIC_Init(&LeftSensor);

    CAR_ApplyMotionCommand('S', MotorSpeed);

    while (1)
    {
        if (UART_GetRXStatus())
        {
            UART_Command = UART_ReceiveByte();

            if (UART_Command == 'Q')
            {
                UART_SendSensorCsv();
            }
            else if (UART_Command == 'T')
            {
                UART_SendTrackStatus();
            }
            else if ((UART_Command >= '0') && (UART_Command <= '9'))
            {
                MotorSpeed = ClampSpeed((u16)(UART_Command - '0') * 10U);
            }
            else
            {
                CAR_ApplyMotionCommand(UART_Command, MotorSpeed);
            }
        }
    }

    return 0;
}
