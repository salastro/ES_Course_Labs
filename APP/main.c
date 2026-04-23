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
    GPIO_Init();
    GPIO_SetPinDirection(GPIO_PORTB, GPIO_PIN0, GPIO_OUTPUT);
    GPIO_SetPinValue(GPIO_PORTB, GPIO_PIN0, GPIO_LOW);

    // Turn LED on and keep it on
    LED_On(GPIO_PORTB, GPIO_PIN0);

    while (1)
    {
        // Keep running (LED stays on)
    }
}