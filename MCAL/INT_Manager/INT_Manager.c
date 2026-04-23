#include "INT_Manager.h"
#include "../INT/INT_interface.h"
#include "../TIMER0/TIMER0_interface.h"
#include "../ADC/ADC_interface.h"
#include "../UART/UART_interface.h"
#include "../SPI/SPI_interface.h"
#include "../I2C/I2C_interface.h"
#include "../../SERVICES/BIT_MATH.h"

#define INTCON (*((volatile u8 *)0x0B))
#define PIR1 (*((volatile u8 *)0x0C))

#define GIE 7
#define INTF 1
#define T0IF 2

#define ADIF 6
#define RCIF 5
#define TXIF 4
#define SSPIF 3

void IntManager_Init(void)
{
    SET_BIT(INTCON, GIE);
}

extern void INT0_InterruptHandler(void);
extern void TIMER0_InterruptHandler(void);
extern void ADC_InterruptHandler(void);
extern void UART_InterruptHandler(void);
extern void SPI_InterruptHandler(void);
extern void I2C_InterruptHandler(void);

#pragma interrupt ISR_Handler
void ISR_Handler(void)
{
    if (GET_BIT(INTCON, INTF))
    {
        INT0_InterruptHandler();
        CLR_BIT(INTCON, INTF);
    }

    if (GET_BIT(INTCON, T0IF))
    {
        TIMER0_InterruptHandler();
        CLR_BIT(INTCON, T0IF);
    }

    if (GET_BIT(PIR1, ADIF))
    {
        ADC_InterruptHandler();
        CLR_BIT(PIR1, ADIF);
    }

    if (GET_BIT(PIR1, RCIF) || GET_BIT(PIR1, TXIF))
    {
        UART_InterruptHandler();
    }

    if (GET_BIT(PIR1, SSPIF))
    {
        SPI_InterruptHandler();
    }
}
