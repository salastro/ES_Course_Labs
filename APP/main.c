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
#include "CAR/CAR_config.h"
#include "CAR/CAR_interface.h"

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

static ULTRASONIC_t BackSensor = {
    GPIO_PORTB, GPIO_PIN6, // Trigger
    GPIO_PORTB, GPIO_PIN7}; // Echo

static ULTRASONIC_t LeftSensor = {
    GPIO_PORTA, GPIO_PIN0, // Trigger
    GPIO_PORTA, GPIO_PIN1}; // Echo

static ULTRASONIC_t RightSensor = {
    GPIO_PORTA, GPIO_PIN2, // Trigger
    GPIO_PORTA, GPIO_PIN3}; // Echo


#define LED_PORT GPIO_PORTB
#define LED_PIN GPIO_PIN0

// TODO: Debounce ultrasonic

int main(void)
{
    // Initial motor speed and state variables
    const u8 MotorSpeed = 100U;
    const u8 StopSpeed = 0U;
    u8 CurrentSpeed = MotorSpeed;
    u16 FrontDistanceCm = 0U;
    u16 BackDistanceCm = 0U;
    u16 LeftDistanceCm = 0U;
    u16 RightDistanceCm = 0U;
    u16 ClosestDistanceCm = 0U;
    u8 ObstacleState = CAR_OBSTACLE_STATE_CLEAR;

    // Initialize GPIO, motors, and ultrasonic sensors
    GPIO_Init();

    // LED for debugging
    GPIO_SetPinDirection(LED_PORT, LED_PIN, GPIO_OUTPUT);
    GPIO_SetPinValue(LED_PORT, LED_PIN, GPIO_LOW);

    DCMOTOR_Init(&LeftMotor);
    DCMOTOR_Init(&RightMotor);
    ULTRASONIC_Init(&FrontSensor);
    ULTRASONIC_Init(&BackSensor);
    ULTRASONIC_Init(&LeftSensor);
    ULTRASONIC_Init(&RightSensor);

    DCMOTOR_Forward(&LeftMotor, CurrentSpeed);
    DCMOTOR_Forward(&RightMotor, CurrentSpeed);

    while (1)
    {
        FrontDistanceCm = ULTRASONIC_GetDistanceCm(&FrontSensor);
        BackDistanceCm = ULTRASONIC_GetDistanceCm(&BackSensor);
        LeftDistanceCm = ULTRASONIC_GetDistanceCm(&LeftSensor);
        RightDistanceCm = ULTRASONIC_GetDistanceCm(&RightSensor);

        ClosestDistanceCm = FrontDistanceCm;

        if ((BackDistanceCm != 0U) && ((ClosestDistanceCm == 0U) || (BackDistanceCm < ClosestDistanceCm)))
        {
            ClosestDistanceCm = BackDistanceCm;
        }

        if ((LeftDistanceCm != 0U) && ((ClosestDistanceCm == 0U) || (LeftDistanceCm < ClosestDistanceCm)))
        {
            ClosestDistanceCm = LeftDistanceCm;
        }

        if ((RightDistanceCm != 0U) && ((ClosestDistanceCm == 0U) || (RightDistanceCm < ClosestDistanceCm)))
        {
            ClosestDistanceCm = RightDistanceCm;
        }

        if ((ClosestDistanceCm != 0U) && (ClosestDistanceCm <= CAR_OBSTACLE_DISTANCE_CM))
        {
            // Considered an obstacle present
            if (ObstacleState != CAR_OBSTACLE_STATE_DETECTED)
            {
                ObstacleState = CAR_OBSTACLE_STATE_DETECTED;
                GPIO_SetPinValue(LED_PORT, LED_PIN, GPIO_HIGH); // Turn on LED when obstacle is detected
            }

            // Gradually decelerate to stop
            Car_Accelerate(&LeftMotor, &RightMotor, &CurrentSpeed, &StopSpeed);
            delay_ms(CAR_ACCELERATION_DELAY_MS);
        }
        else
        {
            // Considered clear
            if (ObstacleState != CAR_OBSTACLE_STATE_CLEAR)
            {
                ObstacleState = CAR_OBSTACLE_STATE_CLEAR;
                GPIO_SetPinValue(LED_PORT, LED_PIN, GPIO_LOW); // Turn off LED when no obstacle is detected
            }

            // Gradually accelerate back to original speed
            Car_Accelerate(&LeftMotor, &RightMotor, &CurrentSpeed, &MotorSpeed);
            delay_ms(CAR_ACCELERATION_DELAY_MS);
        }
    }

    return 0;
}
