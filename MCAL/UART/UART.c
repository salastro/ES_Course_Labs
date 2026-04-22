#include "UART_interface.h"
#include "UART_private.h"
#include "UART_config.h"
#include "../../SERVICES/BIT_MATH.h"

static UART_RX_CallbackFuncPtr UART_RXCallbackFunc = NULL_PTR;
static UART_TX_CallbackFuncPtr UART_TXCallbackFunc = NULL_PTR;

void UART_Init(u16 BaudRate, u8 DataBits, u8 StopBits)
{
    u16 Local_SPBRValue = 0;
    u8 Local_RCSTA = 0;
    u8 Local_TXSTA = 0;

    Local_SPBRValue = (UART_FOSC / (16 * (u32)BaudRate)) - 1;
    SPBRG = (u8)Local_SPBRValue;

    Local_TXSTA = 0x24;

    if (DataBits == UART_DATA_9BITS)
    {
        SET_BIT(Local_RCSTA, RX9);
        SET_BIT(Local_TXSTA, TX9);
    }

    Local_RCSTA = 0x90;

    RCSTA = Local_RCSTA;
    TXSTA = Local_TXSTA;

    CLR_BIT(PIR1, RCIF);
    CLR_BIT(PIR1, TXIF);

    SET_BIT(PIE1, RCIE);
    SET_BIT(PIE1, TXIE);

    SET_BIT(INTCON, 7);
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
        UART_TXCallbackFunc = Copy_Ptr;
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
