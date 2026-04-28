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
#include "../MCAL/PWM/PWM_interface.h"
#include "../SERVICES/STD_TYPES.h"
#include "../MCAL/ADC/ADC_private.h"

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
    GPIO_PORTD, GPIO_PIN0,
    GPIO_PORTD, GPIO_PIN1,
    GPIO_PORTC, GPIO_PIN2,
    DCMOTOR_USE_PWM,
    PWM_CHANNEL1};

static DCMOTOR_t RightMotor = {
    GPIO_PORTD, GPIO_PIN2,
    GPIO_PORTD, GPIO_PIN3,
    GPIO_PORTC, GPIO_PIN1,
    DCMOTOR_USE_PWM,
    PWM_CHANNEL2};

int main(void)
{
    u8 MotorSpeed = 75U;

    GPIO_Init();

    DCMOTOR_Init(&LeftMotor);
    DCMOTOR_Init(&RightMotor);

    while (1)
    {
        DCMOTOR_Forward(&LeftMotor, MotorSpeed);
        DCMOTOR_Forward(&RightMotor, MotorSpeed);
        delay_ms(5000U);

        DCMOTOR_Reverse(&LeftMotor, MotorSpeed);
        DCMOTOR_Reverse(&RightMotor, MotorSpeed);
        delay_ms(5000U);
    }

    return 0;
}