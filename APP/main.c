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
#include "../SERVICES/STD_TYPES.h"

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
    GPIO_PORTB, GPIO_PIN4, // Trigger
    GPIO_PORTB, GPIO_PIN5}; // Echo

#define OBSTACLE_DISTANCE_CM 10U

#define LED_PORT GPIO_PORTB
#define LED_PIN GPIO_PIN0

int main(void)
{
    u8 MotorSpeed = 75U;
    u16 DistanceCm = 0U;

    // Initialize GPIO, motors, and ultrasonic sensor
    GPIO_Init();

    // LED for debugging
    GPIO_SetPinDirection(LED_PORT, LED_PIN, GPIO_OUTPUT);
    GPIO_SetPinValue(LED_PORT, LED_PIN, GPIO_LOW);

    DCMOTOR_Init(&LeftMotor);
    DCMOTOR_Init(&RightMotor);
    ULTRASONIC_Init(&FrontSensor);

    while (1)
    {
        DistanceCm = ULTRASONIC_GetDistanceCm(&FrontSensor);

        if ((DistanceCm != 0U) && (DistanceCm <= OBSTACLE_DISTANCE_CM))
        {
            DCMOTOR_Stop(&LeftMotor);
            DCMOTOR_Stop(&RightMotor);
            // DCMOTOR_Forward(&LeftMotor, 0);
            // DCMOTOR_Forward(&RightMotor, 0);
            GPIO_SetPinValue(LED_PORT, LED_PIN, GPIO_HIGH); // Turn on LED when obstacle is detected
        }
        else
        {
            DCMOTOR_Forward(&LeftMotor, MotorSpeed);
            DCMOTOR_Forward(&RightMotor, MotorSpeed);
            GPIO_SetPinValue(LED_PORT, LED_PIN, GPIO_LOW); // Turn off LED when no obstacle is detected
        }

    }

    return 0;
}
