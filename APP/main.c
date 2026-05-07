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

/* UART API and helpers */
#include "../MCAL/UART/UART_interface.h"

/* Simple UART RX parsing buffer (commands terminated by '\n') */
static u8 UART_RxBuffer[16];
static u8 UART_RxIndex = 0;
/* Shared target speed (updated by UART commands) */
static volatile u8 TargetSpeed = 100U;

static void utoa_u16(u16 value, char *buf)
{
    char tmp[6];
    int i = 0, j = 0;

    if (value == 0)
    {
        buf[0] = '0';
        buf[1] = '\0';
        return;
    }

    while (value > 0 && i < 5)
    {
        tmp[i++] = (char)('0' + (value % 10U));
        value /= 10U;
    }

    while (i > 0)
        buf[j++] = tmp[--i];

    buf[j] = '\0';
}

static void UART_SendTelemetry(u16 frontCm, u16 backCm, u8 speed)
{
    char num[6];

    /* Format: F:<front>,B:<back>,S:<speed>\n */
    UART_SendString((u8 *)"F:");
    utoa_u16(frontCm, num);
    UART_SendString((u8 *)num);
    UART_SendString((u8 *)",B:");
    utoa_u16(backCm, num);
    UART_SendString((u8 *)num);
    UART_SendString((u8 *)",S:");
    utoa_u16(speed, num);
    UART_SendString((u8 *)num);
    UART_SendString((u8 *)"\n");
}

/* RX handler called from UART interrupt (byte-at-a-time) */
static void UART_RX_Handler(u8 Data)
{
    if (Data == '\r')
        return;

    if (Data == '\n' || UART_RxIndex >= (sizeof(UART_RxBuffer) - 1))
    {
        UART_RxBuffer[UART_RxIndex] = '\0';

        /* Command parsing: S<value> sets target speed (0-100) */
        if (UART_RxIndex > 0)
        {
            if (UART_RxBuffer[0] == 'S' || UART_RxBuffer[0] == 's')
            {
                u8 val = 0U;
                u8 i = 1U;
                while (UART_RxBuffer[i] >= '0' && UART_RxBuffer[i] <= '9')
                {
                    val = (u8)(val * 10U + (UART_RxBuffer[i] - '0'));
                    i++;
                }
                if (val > 100U)
                    val = 100U;
                /* write to shared target; main loop will accelerate/decelerate */
                TargetSpeed = val;
            }
        }

        UART_RxIndex = 0;
    }
    else
    {
        UART_RxBuffer[UART_RxIndex++] = Data;
    }
}

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
    GPIO_PORTB, GPIO_PIN6,  // Trigger
    GPIO_PORTB, GPIO_PIN7}; // Echo

// static ULTRASONIC_t LeftSensor = {
//     GPIO_PORTA, GPIO_PIN0, // Trigger
//     GPIO_PORTA, GPIO_PIN1}; // Echo

// static ULTRASONIC_t RightSensor = {
//     GPIO_PORTA, GPIO_PIN2, // Trigger
//     GPIO_PORTA, GPIO_PIN3}; // Echo

#define LED_PORT GPIO_PORTB
#define LED_PIN GPIO_PIN0

int main(void)
{
    // Initial motor speed and state variables
    const u8 StopSpeed = 0U;
    u8 CurrentSpeed = TargetSpeed;
    u16 FrontDistanceCm = 0U;
    u16 BackDistanceCm = 0U;
    // u16 LeftDistanceCm = 0U;
    // u16 RightDistanceCm = 0U;
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
    // ULTRASONIC_Init(&LeftSensor);
    // ULTRASONIC_Init(&RightSensor);

    DCMOTOR_Forward(&LeftMotor, CurrentSpeed);
    DCMOTOR_Forward(&RightMotor, CurrentSpeed);

    /* Initialize UART for Raspberry Pi communication */
    UART_Init(UART_BAUD_9600, UART_DATA_8BITS, UART_STOP_1BIT);
    UART_SetRXCallback(UART_RX_Handler);

    while (1)
    {
        FrontDistanceCm = ULTRASONIC_GetDistanceCm(&FrontSensor);
        BackDistanceCm = ULTRASONIC_GetDistanceCm(&BackSensor);
        // LeftDistanceCm = ULTRASONIC_GetDistanceCm(&LeftSensor);
        // RightDistanceCm = ULTRASONIC_GetDistanceCm(&RightSensor);

        ClosestDistanceCm = FrontDistanceCm;

        if ((BackDistanceCm != 0U) && ((ClosestDistanceCm == 0U) || (BackDistanceCm < ClosestDistanceCm)))
        {
            ClosestDistanceCm = BackDistanceCm;
        }

        // if ((LeftDistanceCm != 0U) && ((ClosestDistanceCm == 0U) || (LeftDistanceCm < ClosestDistanceCm)))
        // {
        //     ClosestDistanceCm = LeftDistanceCm;
        // }

        // if ((RightDistanceCm != 0U) && ((ClosestDistanceCm == 0U) || (RightDistanceCm < ClosestDistanceCm)))
        // {
        //     ClosestDistanceCm = RightDistanceCm;
        // }

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

            // Gradually accelerate back to target speed
            Car_Accelerate(&LeftMotor, &RightMotor, &CurrentSpeed, &TargetSpeed);
            delay_ms(CAR_ACCELERATION_DELAY_MS);
        }

        /* Send telemetry over UART: front, back, current speed */
        UART_SendTelemetry(FrontDistanceCm, BackDistanceCm, CurrentSpeed);
    }

    return 0;
}
