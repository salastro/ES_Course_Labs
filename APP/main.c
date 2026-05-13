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
#include "../HAL/LCD_I2C/LCD_I2C_interface.h"
#include "../MCAL/I2C/I2C_interface.h"
#include "../SERVICES/STD_TYPES.h"

void delay_ms(u16 ms)
{
    unsigned int i, j;

    for (i = 0; i < ms; i++)
    {
        for (j = 0; j < 100; j++)
        {
            __asm("nop");
        }
    }
}

int main(void)
{
    // Initialize GPIO
    GPIO_Init();

    // Initialize I2C
    I2C_Init(I2C_MASTER, I2C_SPEED_100kHz);

    // Initialize LCD
    LCD_I2C_Init();

    // Clear LCD
    LCD_I2C_Clear();

    // Set cursor to row 0 column 0
    LCD_I2C_SetCursor(0, 0);

    // Write text
    LCD_I2C_WriteString("Hello World");

    while (1)
    {
        // Do nothing
    }

    return 0;
}