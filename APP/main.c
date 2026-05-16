// Configuration bits for 16MHz HS oscillator
#pragma config FOSC = HS
#pragma config WDTE = OFF
#pragma config PWRTE = OFF
#pragma config BOREN = OFF
#pragma config LVP = OFF
#pragma config CPD = OFF
#pragma config WRT = OFF
#pragma config CP = OFF

#define _XTAL_FREQ 16000000

#include "../MCAL/GPIO/GPIO_interface.h"
#include "../HAL/DC_MOTOR/DC_MOTOR_interface.h"
#include "../HAL/ULTRASONIC/ULTRASONIC_interface.h"
#include "../HAL/LCD/LCD_interface.h"
#include "../MCAL/I2C/I2C_interface.h"
#include "../MCAL/PWM/PWM_interface.h"
#include "../SERVICES/STD_TYPES.h"
#include <xc.h>

#define OBSTACLE_DISTANCE_CM 10U

#define LED_PORT GPIO_PORTA
#define LED_PIN GPIO_PIN0

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

static ULTRASONIC_t FrontSensor = {
    GPIO_PORTB, GPIO_PIN6,
    GPIO_PORTB, GPIO_PIN7};

void LCD_ShowData(u8 speed, u16 distance)
{
    char speedText[5];
    char distText[6];

    LCD_Clear();

    LCD_SetCursor(0, 0);
    LCD_WriteString("Speed: ");

    speedText[0] = (char)((speed / 10U) + '0');
    speedText[1] = (char)((speed % 10U) + '0');
    speedText[2] = '%';
    speedText[3] = '\0';

    LCD_WriteString(speedText);

    LCD_SetCursor(1, 0);
    LCD_WriteString("Dist: ");

    if (distance >= 100U)
    {
        distText[0] = (char)((distance / 100U) + '0');
        distText[1] = (char)(((distance / 10U) % 10U) + '0');
        distText[2] = (char)((distance % 10U) + '0');
        distText[3] = '\0';
    }
    else if (distance >= 10U)
    {
        distText[0] = (char)((distance / 10U) + '0');
        distText[1] = (char)((distance % 10U) + '0');
        distText[2] = '\0';
    }
    else
    {
        distText[0] = (char)(distance + '0');
        distText[1] = '\0';
    }

    LCD_WriteString(distText);
    LCD_WriteString(" cm");
}

int main(void)
{
    u8 MotorSpeed = 75U;
    u16 DistanceCm = 0U;
    char distText[4];

    GPIO_Init();

    GPIO_SetPinDirection(LED_PORT, LED_PIN, GPIO_OUTPUT);
    GPIO_SetPinValue(LED_PORT, LED_PIN, GPIO_LOW);

    I2C_Init(I2C_MASTER, I2C_SPEED_100kHz);
    LCD_Init();
    LCD_Clear();

    DCMOTOR_Init(&LeftMotor);
    DCMOTOR_Init(&RightMotor);
    ULTRASONIC_Init(&FrontSensor);

    LCD_SetCursor(0, 0);
    LCD_WriteString("System Start");
    __delay_ms(1000);

    while (1)
    {
        DistanceCm = ULTRASONIC_GetDistanceCm(&FrontSensor);

        // LCD_ShowData(MotorSpeed, DistanceCm);
        LCD_Clear();
        LCD_SetCursor(0, 0);

        distText[0] = (char)((DistanceCm / 10U) + '0');
        distText[1] = (char)((DistanceCm % 10U) + '0');
        distText[2] = '\0';
        LCD_WriteString(distText);
        LCD_WriteString(" cm");

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
            GPIO_SetPinValue(LED_PORT, LED_PIN, GPIO_LOW);
        }

        __delay_ms(300);
    }

    return 0;
}