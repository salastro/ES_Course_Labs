#include "I2C_interface.h"
#include "I2C_private.h"
#include "I2C_config.h"
#include "../../SERVICES/BIT_MATH.h"

static I2C_CallbackFuncPtr I2C_CallbackFunc = NULL_PTR;

static u8 I2C_GlobalStatus = I2C_IDLE;

void I2C_Init(u8 Mode, u32 ClockSpeed)
{
    u8 Local_BRGVAL = 0;
    u8 Local_SSPCONValue = 0;

    if (Mode == I2C_MASTER)
    {
        Local_BRGVAL = (u8)((I2C_FOSC / (4 * ClockSpeed)) - 1);

        SSPADD = Local_BRGVAL;

        Local_SSPCONValue = 0x28;
    }
    else
    {
        SSPADD = I2C_SLAVE_ADDRESS;

        Local_SSPCONValue = 0x36;
    }

    SSPCON = Local_SSPCONValue;

    SET_BIT(SSPSTAT, SMP);

    CLR_BIT(PIR1, SSPIF);

    SET_BIT(PIE1, SSPI);
    SET_BIT(INTCON, 7);

    I2C_GlobalStatus = I2C_IDLE;
}

void I2C_StartCondition(void)
{
    SET_BIT(SSPCON2, SEN);

    while (GET_BIT(SSPCON2, SEN))
        ;

    I2C_GlobalStatus = I2C_BUSY;
}

void I2C_StopCondition(void)
{
    SET_BIT(SSPCON2, PEN);

    while (GET_BIT(SSPCON2, PEN))
        ;

    I2C_GlobalStatus = I2C_IDLE;
}

void I2C_SendByte(u8 Data)
{
    SSPBUF = Data;

    /* Wait for transfer complete */
    while (GET_BIT(SSPSTAT, RW))
        ;

    I2C_GlobalStatus = I2C_BUSY;
}

u8 I2C_ReceiveByte(void)
{
    /* Enable receive mode */
    SET_BIT(SSPCON2, RCEN);

    /* Wait for byte reception */
    while (GET_BIT(SSPCON2, RCEN))
        ;

    while (!GET_BIT(SSPSTAT, BF))
        ;

    return SSPBUF;
}

u8 I2C_ReadAck(void)
{
    return GET_BIT(SSPCON2, ACKSTAT);
}

void I2C_SendAck(void)
{
    CLR_BIT(SSPCON2, ACKDT);
    SET_BIT(SSPCON2, ACKEN);

    while (GET_BIT(SSPCON2, ACKEN))
        ;
}

void I2C_SendNack(void)
{
    SET_BIT(SSPCON2, ACKDT);
    SET_BIT(SSPCON2, ACKEN);

    while (GET_BIT(SSPCON2, ACKEN))
        ;
}

void I2C_SetCallback(I2C_CallbackFuncPtr Copy_Ptr)
{
    if (Copy_Ptr != NULL_PTR)
        I2C_CallbackFunc = Copy_Ptr;
}

u8 I2C_GetStatus(void)
{
    return I2C_GlobalStatus;
}

void I2C_InterruptHandler(void)
{
    if (I2C_CallbackFunc != NULL_PTR)
    {
        u8 Local_Data = SSPBUF;
        u8 Local_Status = GET_BIT(SSPCON2, ACKSTAT);
        I2C_CallbackFunc(Local_Data, Local_Status);
    }
}
