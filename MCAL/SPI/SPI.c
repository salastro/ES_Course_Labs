#include "SPI_interface.h"
#include "SPI_private.h"
#include "SPI_config.h"
#include "../../SERVICES/BIT_MATH.h"

static SPI_CallbackFuncPtr SPI_CallbackFunc = NULL_PTR;

void SPI_Init(u8 Mode, u8 Prescaler, u8 ClockPolarity, u8 ClockPhase)
{
    u8 Local_SSPCONValue = 0;
    u8 Local_SSPSTATValue = 0;

    if (Mode == SPI_MASTER)
    {
        Local_SSPCONValue = (Prescaler & 0x03);
        Local_SSPCONValue |= 0x00;
    }
    else
    {
        Local_SSPCONValue = 0x04;
    }

    if (ClockPolarity == SPI_CLK_IDLE_HIGH)
        SET_BIT(Local_SSPCONValue, CKP);

    /* Enable SSP Module */
    SET_BIT(Local_SSPCONValue, SSPEN);

    if (ClockPhase == SPI_SAMPLE_END)
        SET_BIT(Local_SSPSTATValue, CKE);

    SSPCON = Local_SSPCONValue;
    SSPSTAT = Local_SSPSTATValue;

    CLR_BIT(PIR1, SSPIF);

    SET_BIT(PIE1, SSPI);
    SET_BIT(INTCON, 7);
}

void SPI_SendByte(u8 Data)
{

    while (GET_BIT(SSPSTAT, BF))
        ;

    SSPBUF = Data;
}

u8 SPI_ReceiveByte(void)
{

    while (!GET_BIT(SSPSTAT, BF))
        ;

    return SSPBUF;
}

u8 SPI_TransceiveByte(u8 Data)
{
    /* Send data */
    SPI_SendByte(Data);

    /* Receive response */
    return SPI_ReceiveByte();
}

void SPI_SetCallback(SPI_CallbackFuncPtr Copy_Ptr)
{
    if (Copy_Ptr != NULL_PTR)
        SPI_CallbackFunc = Copy_Ptr;
}

u8 SPI_GetStatus(void)
{
    return GET_BIT(SSPSTAT, BF);
}

void SPI_InterruptHandler(void)
{
    u8 Local_Data = SSPBUF;

    if (SPI_CallbackFunc != NULL_PTR)
        SPI_CallbackFunc(Local_Data);
}
