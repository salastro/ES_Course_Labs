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
// #include <xc.h>

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
        return 100U;
    return (u8)SpeedPercent;
}

u8 atoi(u8 *str)
{
    u8 result = 0;
    while (*str >= '0' && *str <= '9')
    {
        result = result * 10 + (*str - '0');
        str++;
    }
    return result;
}

void UART_CAR_SpeedCallback(u8 data)
{
    // Convert received string to integer
    u8 ReceivedSpeed = atoi(&data);
    u8 ClampedSpeed = ClampSpeed(ReceivedSpeed);

    // Set motor speed using PWM
    DCMOTOR_SetSpeed(&LeftMotor, ClampedSpeed);
    DCMOTOR_SetSpeed(&RightMotor, ClampedSpeed);
}

int main(void)
{
    u8 MotorSpeed = 0U;
    u16 FrontDistanceCm = 0U;
    u16 BackDistanceCm = 0U;
    u16 RightDistanceCm = 0U;
    u16 LeftDistanceCm = 0U;

    // Initialize GPIO, motors, and ultrasonic sensor
    GPIO_Init();

    // Initialize UART for PI
    UART_Init(UART_BAUD_9600, UART_DATA_8BITS, UART_STOP_1BIT);
    UART_SetRXCallback(UART_CAR_SpeedCallback);

    // LED for debugging
    GPIO_SetPinDirection(RED_LED_PORT, RED_LED_PIN, GPIO_OUTPUT);
    GPIO_SetPinValue(RED_LED_PORT, RED_LED_PIN, GPIO_LOW);

    DCMOTOR_Init(&LeftMotor);
    DCMOTOR_Init(&RightMotor);
    ULTRASONIC_Init(&FrontSensor);
    ULTRASONIC_Init(&BackSensor);
    ULTRASONIC_Init(&RightSensor);
    ULTRASONIC_Init(&LeftSensor);

    while (1)
    {
        // Read distances from ultrasonic sensors
        FrontDistanceCm = ULTRASONIC_GetDistanceCm(&FrontSensor);
        BackDistanceCm = ULTRASONIC_GetDistanceCm(&BackSensor);
        RightDistanceCm = ULTRASONIC_GetDistanceCm(&RightSensor);
        LeftDistanceCm = ULTRASONIC_GetDistanceCm(&LeftSensor);

        // Send distance data to PI via UART
        UART_SendString((u8 *)"F: ");
        UART_SendByte((u8)(FrontDistanceCm / 10) + '0'); // Send tens
        UART_SendByte((u8)(FrontDistanceCm % 10) + '0'); // Send units
        UART_SendString((u8 *)"\n");

        UART_SendString((u8 *)"B: ");
        UART_SendByte((u8)(BackDistanceCm / 10) + '0'); // Send tens
        UART_SendByte(((u8)BackDistanceCm % 10) + '0'); // Send units
        UART_SendString((u8 *)"\n");

        UART_SendString((u8 *)"R: ");
        UART_SendByte((u8)(RightDistanceCm / 10) + '0'); // Send tens
        UART_SendByte(((u8)RightDistanceCm % 10) + '0'); // Send units
        UART_SendString((u8 *)"\n");

        UART_SendString((u8 *)"L: ");
        UART_SendByte((u8)(LeftDistanceCm / 10) + '0'); // Send tens
        UART_SendByte(((u8)LeftDistanceCm % 10) + '0'); // Send units
        UART_SendString((u8 *)"\n");

        // Send speed
        UART_SendString((u8 *)"S: ");
        UART_SendByte((u8)(MotorSpeed / 10) + '0'); // Send tens
        UART_SendByte(((u8)MotorSpeed % 10) + '0'); // Send units
        UART_SendString((u8 *)"\n");

        // // Simple obstacle avoidance logic
        // if (FrontDistanceCm < OBSTACLE_DISTANCE_CM)
        // {
        //     DCMOTOR_Stop(&LeftMotor);
        //     DCMOTOR_Stop(&RightMotor);
        //     MotorSpeed = 0U;
        // }
    }

    return 0;
}
