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
#include "../HAL/ULTRASONIC/ULTRASONIC_interface.h"
#include "../MCAL/PWM/PWM_interface.h"
#include "../MCAL/UART/UART_interface.h"
#include "../SERVICES/STD_TYPES.h"
#include <xc.h>

void delay_ms(u16 ms)
{
    unsigned int i, j;
    for (i = 0; i < ms; i++)
    {
        const unsigned int iterations = 16000 / 4;
        for (j = 0; j < iterations; j++)
        {
            // This loop creates a delay of approximately 1 ms at 16 MHz
        }
    }
}

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

#define OBSTACLE_DISTANCE_CM 10U

#define LED_PORT GPIO_PORTA
#define LED_PIN GPIO_PIN0

static u8 ClampSpeed(u16 SpeedPercent)
{
    if (SpeedPercent > 100U)
    {
        return 100U;
    }

    return (u8)SpeedPercent;
}

static void UART_SendUnsignedDecimal(u16 Value)
{
    u8 Digits[5];
    u8 DigitCount = 0U;

    if (Value == 0U)
    {
        UART_SendByte('0');
        return;
    }

    while ((Value > 0U) && (DigitCount < 5U))
    {
        Digits[DigitCount] = (u8)('0' + (Value % 10U));
        Value /= 10U;
        DigitCount++;
    }

    while (DigitCount > 0U)
    {
        DigitCount--;
        UART_SendByte(Digits[DigitCount]);
    }
}

static void UART_SendDistanceReport(u16 FrontDistanceCm,
                                    u16 BackDistanceCm,
                                    u16 RightDistanceCm,
                                    u16 LeftDistanceCm,
                                    u8 MotorSpeed)
{
    UART_SendString((u8 *)"F:");
    UART_SendUnsignedDecimal(FrontDistanceCm);
    UART_SendString((u8 *)"cm B:");
    UART_SendUnsignedDecimal(BackDistanceCm);
    UART_SendString((u8 *)"cm R:");
    UART_SendUnsignedDecimal(RightDistanceCm);
    UART_SendString((u8 *)"cm L:");
    UART_SendUnsignedDecimal(LeftDistanceCm);
    UART_SendString((u8 *)"cm S:");
    UART_SendUnsignedDecimal(MotorSpeed);
    UART_SendString((u8 *)"%\r\n");
}

static void UART_ProcessSpeedCommand(u8 *MotorSpeed)
{
    static u16 ReceivedValue = 0U;
    static u8 ReceivingValue = 0U;

    while (UART_GetRXStatus() != 0U)
    {
        u8 ReceivedByte = UART_ReceiveByte();

        if ((ReceivedByte >= (u8)'0') && (ReceivedByte <= (u8)'9'))
        {
            if (ReceivingValue == 0U)
            {
                ReceivedValue = 0U;
                ReceivingValue = 1U;
            }

            ReceivedValue = (u16)(ReceivedValue * 10U) + (u16)(ReceivedByte - (u8)'0');
        }
        else if ((ReceivedByte == (u8)'\r') || (ReceivedByte == (u8)'\n'))
        {
            if (ReceivingValue != 0U)
            {
                *MotorSpeed = ClampSpeed(ReceivedValue);
                ReceivedValue = 0U;
                ReceivingValue = 0U;
            }
        }
        else
        {
            ReceivedValue = 0U;
            ReceivingValue = 0U;
        }
    }
}

int main(void)
{
    u8 MotorSpeed = 75U;
    u16 DistanceCm = 0U;
    u16 FrontDistanceCm = 0U;
    u16 BackDistanceCm = 0U;
    u16 RightDistanceCm = 0U;
    u16 LeftDistanceCm = 0U;
    // Initialize GPIO, motors, and ultrasonic sensor
    GPIO_Init();

    UART_Init(UART_BAUD_9600, UART_DATA_8BITS, UART_STOP_1BIT);

    // LED for debugging
    GPIO_SetPinDirection(LED_PORT, LED_PIN, GPIO_OUTPUT);
    GPIO_SetPinValue(LED_PORT, LED_PIN, GPIO_LOW);

    DCMOTOR_Init(&LeftMotor);
    DCMOTOR_Init(&RightMotor);
    ULTRASONIC_Init(&FrontSensor);
    ULTRASONIC_Init(&BackSensor);
    ULTRASONIC_Init(&RightSensor);
    ULTRASONIC_Init(&LeftSensor);

    // UART_SendString((u8 *)"Wheel speed command: send 0-100 and press Enter\r\n");

    while (1)
    {
        UART_ProcessSpeedCommand(&MotorSpeed);

        DistanceCm = FrontDistanceCm = ULTRASONIC_GetDistanceCm(&FrontSensor);
        BackDistanceCm = ULTRASONIC_GetDistanceCm(&BackSensor);
        RightDistanceCm = ULTRASONIC_GetDistanceCm(&RightSensor);
        LeftDistanceCm = ULTRASONIC_GetDistanceCm(&LeftSensor);

        // Find the minimum distance from the three sensors
        if (BackDistanceCm < DistanceCm)
            DistanceCm = BackDistanceCm;
        else if (RightDistanceCm < DistanceCm)
            DistanceCm = RightDistanceCm;
        else if (LeftDistanceCm < DistanceCm)
            DistanceCm = LeftDistanceCm;

        if ((DistanceCm != 0U) && (DistanceCm <= OBSTACLE_DISTANCE_CM))
        {
            DCMOTOR_Stop(&LeftMotor);
            DCMOTOR_Stop(&RightMotor);
            GPIO_SetPinValue(LED_PORT, LED_PIN, GPIO_HIGH);
        }
        else
        {
            DCMOTOR_Forward(&LeftMotor, MotorSpeed);
            DCMOTOR_Forward(&RightMotor, MotorSpeed);
            GPIO_SetPinValue(LED_PORT, LED_PIN, GPIO_LOW); // Turn off LED when no obstacle is detected
        }

        UART_SendDistanceReport(FrontDistanceCm, BackDistanceCm, RightDistanceCm, LeftDistanceCm, MotorSpeed);
        __delay_ms(100U);
    }

    return 0;
}
