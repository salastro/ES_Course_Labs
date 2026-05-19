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

#include "../MCAL/UART/UART_interface.h"
#include "CAR_interface.h"

int main(void)
{
    u8 UART_Command = 'S';

    CAR_Init();

    while (1)
    {
        if (UART_GetRXStatus())
        {
            UART_Command = UART_ReceiveByte();

            CAR_HandleCommand(UART_Command);
        }
    }

    return 0;
}
