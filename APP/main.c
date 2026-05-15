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
#include "../MCAL/UART/UART_interface.h"
#include "../SERVICES/STD_TYPES.h"
#include <xc.h>

/* LED تعريف */
#define LED_PORT GPIO_PORTA
#define LED_PIN GPIO_PIN0

void delay_ms(u16 ms)
{
    for (u16 i = 0; i < ms; i++)
        for (u16 i = 0; i < 100; i++)
            asm("nop");
}

/* UART RX Callback */
void CtrlLed(u8 data)
{
    if (data == '1')
    {
        GPIO_SetPinValue(LED_PORT, LED_PIN, GPIO_HIGH);
    }
    else if (data == '0')
    {
        GPIO_SetPinValue(LED_PORT, LED_PIN, GPIO_LOW);
    }
}

void main(void)
{
    /* Initialize GPIO */
    GPIO_Init();
    GPIO_SetPinDirection(LED_PORT, LED_PIN, GPIO_OUTPUT);
    GPIO_SetPinValue(LED_PORT, LED_PIN, GPIO_HIGH);

    /* Initialize UART */
    UART_Init(UART_BAUD_9600, UART_DATA_8BITS, UART_STOP_1BIT);
    UART_SetRXCallback(CtrlLed);

    while (1)
    {
        /* Send HIGH every second */
        UART_SendByte('1');
        delay_ms(1000);

        UART_SendByte('0');
        delay_ms(1000);
    }
}