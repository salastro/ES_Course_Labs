#include <xc.h>
#include "UART_interface.h"
#include "UART_private.h"
#include "UART_config.h"
#include "../../SERVICES/BIT_MATH.h"

static UART_RX_CallbackFuncPtr UART_RXCallbackFunc = NULL_PTR;
static UART_TX_CallbackFuncPtr UART_TXCallbackFunc = NULL_PTR;

void UART_Init(u16 BaudRate, u8 DataBits, u8 StopBits __attribute__((unused)))

{

	u8 debug = 0x00;
    u32 Local_SPBRValue = 0;
    u8  Local_TXSTA = 0;
    u8  Local_RCSTA = 0;


    /* 1. Pin directions */
    CLR_BIT(TRISC, UART_TX_PIN);
    SET_BIT(TRISC, UART_RX_PIN);
	debug++;
	eeprom_write(0x01, debug); //1

    /* 2. Baud rate calculation */

    Local_SPBRValue = (UART_FOSC / (16UL * BaudRate)) - 1;

    if (Local_SPBRValue > 255) {
        Local_TXSTA = 0x20;   // BRGH=0, TXEN=1
        Local_SPBRValue = (UART_FOSC / (64UL * BaudRate)) - 1;
    } else {
        Local_TXSTA = 0x24;   // BRGH=1, TXEN=1
    }

    /* 3. Handle 9-bit mode in the LOCAL copies, not the registers */
    Local_RCSTA = 0x90;       // SPEN=1, CREN=1 as baseline



    if (DataBits == UART_DATA_9BITS) {
        SET_BIT(Local_TXSTA, TX9);
        SET_BIT(Local_RCSTA, RX9);
    }
	debug++;
	eeprom_write(0x01, debug); //2



    /* 4. Commit to registers in the correct order:
          TXSTA first (peripheral still off), then SPBRG, then RCSTA last
          (writing SPEN=1 is what actually starts the peripheral) */
    TXSTA = Local_TXSTA;
    SPBRG = (u8)Local_SPBRValue;
    RCSTA = Local_RCSTA;      // Single atomic write — SPEN goes high exactly once

	CLR_BIT(PIR1, RCIF);

    /* 5. Interrupts */
	CLR_BIT(PIR1, RCIF);
	debug++;
	eeprom_write(0x01, debug); //3
	
	eeprom_write(0x03, GIE);
	eeprom_write(0x03, (RCSTA/100)%10);
	eeprom_write(0x04, (RCSTA/10)%10);
	eeprom_write(0x05, RCSTA%10);

    SET_BIT(PIE1, RCIE);
	debug++;
	eeprom_write(0x01, debug); //5

    SET_BIT(INTCON, PEIE);
	debug++;
	eeprom_write(0x01, debug); //4

}


void UART_SendByte(u8 Data)
{

    while (!GET_BIT(TXSTA, TRMT))
        ;

    TXREG = Data;
}

u8 UART_ReceiveByte(void)
{

    while (!GET_BIT(PIR1, RCIF))
        ;

    return RCREG;
}

void UART_SendString(u8 *String)
{
    while (*String != '\0')
    {
        UART_SendByte(*String);
        String++;
    }
}

void UART_SetRXCallback(UART_RX_CallbackFuncPtr Copy_Ptr)
{

    if (Copy_Ptr != NULL_PTR)
        UART_RXCallbackFunc = Copy_Ptr;
}

void UART_SetTXCallback(UART_TX_CallbackFuncPtr Copy_Ptr)
{
    if (Copy_Ptr != NULL_PTR)
    {
        UART_TXCallbackFunc = Copy_Ptr;
        SET_BIT(PIE1, TXIE);
    }
    else
    {
        UART_TXCallbackFunc = NULL_PTR;
        CLR_BIT(PIE1, TXIE);
    }
}

u8 UART_GetRXStatus(void)
{
    return GET_BIT(PIR1, RCIF);
}

u8 UART_GetTXStatus(void)
{
    return GET_BIT(TXSTA, TRMT);
}

/* Interrupt handler function called from INT_Manager */
void UART_InterruptHandler(void)
{
    if (GET_BIT(PIR1, RCIF))
    {
        u8 Local_Data = RCREG;

        if (UART_RXCallbackFunc != NULL_PTR)
            UART_RXCallbackFunc(Local_Data);
    }

    if (GET_BIT(PIR1, TXIF))
    {
        if (UART_TXCallbackFunc != NULL_PTR)
            UART_TXCallbackFunc();
    }
}
