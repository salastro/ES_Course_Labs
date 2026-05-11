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
#include "../MCAL/INT_Manager/INT_Manager.h"
#include "../SERVICES/STD_TYPES.h"
#include "CAR/CAR_config.h"
#include "CAR/CAR_interface.h"

/* Motor configuration */
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

/* Ultrasonic sensor configuration */
static ULTRASONIC_t FrontSensor = {
    GPIO_PORTB, GPIO_PIN4,  // Trigger
    GPIO_PORTB, GPIO_PIN5}; // Echo

static ULTRASONIC_t BackSensor = {
    GPIO_PORTB, GPIO_PIN6,  // Trigger
    GPIO_PORTB, GPIO_PIN7}; // Echo

static ULTRASONIC_t LeftSensor = {
    GPIO_PORTA, GPIO_PIN0,  // Trigger
    GPIO_PORTA, GPIO_PIN1}; // Echo

static ULTRASONIC_t RightSensor = {
    GPIO_PORTA, GPIO_PIN2,  // Trigger
    GPIO_PORTA, GPIO_PIN3}; // Echo

/* LED configuration */
#define LED_PORT GPIO_PORTB
#define LED_PIN GPIO_PIN0

/* Non-blocking acceleration timing counter */
static u16 AccelerationTick = 0U;

int main(void)
{
    const u8 MotorSpeed = 100U;
    const u8 StopSpeed = 0U;
    u8 CurrentSpeed = MotorSpeed;
    u8 ObstacleState = CAR_OBSTACLE_STATE_CLEAR;
    u16 ClosestDistanceCm = 0U;

    /* Initialize hardware */
    GPIO_Init();
    GPIO_SetPinDirection(LED_PORT, LED_PIN, GPIO_OUTPUT);
    GPIO_SetPinValue(LED_PORT, LED_PIN, GPIO_LOW);

    DCMOTOR_Init(&LeftMotor);
    DCMOTOR_Init(&RightMotor);
    ULTRASONIC_Init(&FrontSensor);
    ULTRASONIC_Init(&BackSensor);
    ULTRASONIC_Init(&LeftSensor);
    ULTRASONIC_Init(&RightSensor);

    /* Initialize interrupt-driven ultrasonic measurements and enable interrupts */
    ULTRASONIC_InitMeasurementSystem(&FrontSensor, &BackSensor, &LeftSensor, &RightSensor);
    IntManager_Init();

    /* Start moving forward */
    DCMOTOR_Forward(&LeftMotor, CurrentSpeed);
    DCMOTOR_Forward(&RightMotor, CurrentSpeed);

    while (1)
    {
        /* Find closest obstacle (non-blocking sensor reads) */
        u16 distances[4] = {
            ULTRASONIC_GetDistanceCm(&FrontSensor),
            ULTRASONIC_GetDistanceCm(&BackSensor),
            ULTRASONIC_GetDistanceCm(&LeftSensor),
            ULTRASONIC_GetDistanceCm(&RightSensor)};

        /* Find minimum non-zero distance */
        ClosestDistanceCm = 0U;
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

        /* Non-blocking acceleration timing */
        if (++AccelerationTick >= CAR_ACCELERATION_DELAY_MS)
        {
            AccelerationTick = 0U;

            /* Obstacle detection logic */
            if ((ClosestDistanceCm != 0U) && (ClosestDistanceCm <= CAR_OBSTACLE_DISTANCE_CM))
            {
                /* Obstacle detected */
                if (ObstacleState != CAR_OBSTACLE_STATE_DETECTED)
                {
                    ObstacleState = CAR_OBSTACLE_STATE_DETECTED;
                    GPIO_SetPinValue(LED_PORT, LED_PIN, GPIO_HIGH);
                }
                Car_Accelerate(&LeftMotor, &RightMotor, &CurrentSpeed, &StopSpeed);
            }
            else
            {
                /* Path clear */
                if (ObstacleState != CAR_OBSTACLE_STATE_CLEAR)
                {
                    ObstacleState = CAR_OBSTACLE_STATE_CLEAR;
                    GPIO_SetPinValue(LED_PORT, LED_PIN, GPIO_LOW);
                }
                Car_Accelerate(&LeftMotor, &RightMotor, &CurrentSpeed, &MotorSpeed);
            }
        }
    }

    return 0;
}
