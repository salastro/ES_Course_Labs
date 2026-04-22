#include "../HAL/LED/LED_interface.h"
#include "../MCAL/GPIO/GPIO_interface.h"
#include "../SERVICES/STD_TYPES.h"

void delay_ms(u16 ms)
{
    unsigned int i, j;
    for (i = 0; i < ms; i++)
        for (j = 0; j < 123; j++)
            ;
}

void main(void)
{
    // Initialize GPIO for LED
    GPIO_SetPinDirection(GPIO_PORTB, GPIO_PIN0, GPIO_OUTPUT);
    GPIO_SetPinValue(GPIO_PORTB, GPIO_PIN0, GPIO_LOW);

    while (1)
    {
        // Turn LED on
        LED_On(GPIO_PORTB, GPIO_PIN0);
        delay_ms(1000);

        // Turn LED off
        LED_Off(GPIO_PORTB, GPIO_PIN0);
        delay_ms(1000);
    }
}