#include "ADC_interface.h"
#include "ADC_private.h"
#include "ADC_config.h"
#include "../../SERVICES/BIT_MATH.h"

static ADC_CallbackFuncPtr ADC_CallbackFunc = NULL_PTR;

void ADC_Init(u8 Channel, u8 RefVoltage, u8 Prescaler)
{
    u8 Local_ADCON0 = 0;
    u8 Local_ADCON1 = 0;

    switch (RefVoltage)
    {
    case ADC_REF_VSS_VDD:
        Local_ADCON1 = 0x0E;
        break;

    case ADC_REF_VREF_VSS:
        Local_ADCON1 = 0x0D;
        break;

    case ADC_REF_VSS_VREF:
        Local_ADCON1 = 0x0C;
        break;

    case ADC_REF_VREF_VREF:
        Local_ADCON1 = 0x0B;
        break;

    default:
        Local_ADCON1 = 0x0E;
        break;
    }

    ADCON1 = Local_ADCON1;

    switch (Prescaler)
    {
    case ADC_PRESCALER_2:
        Local_ADCON0 = 0x00;
        break;

    case ADC_PRESCALER_8:
        Local_ADCON0 = 0x40;
        break;

    case ADC_PRESCALER_32:
        Local_ADCON0 = 0x80;
        break;

    case ADC_PRESCALER_OSC:
        Local_ADCON0 = 0xC0;
        break;

    default:
        Local_ADCON0 = 0x40;
        break;
    }

    /* Select Channel */
    Local_ADCON0 |= (Channel << 3) & 0x38;

    /* Enable ADC */
    SET_BIT(Local_ADCON0, ADON);

    ADCON0 = Local_ADCON0;

    /* Clear ADC Interrupt Flag */
    CLR_BIT(PIR1, ADIF);

    /* Enable ADC Interrupt */
    SET_BIT(PIE1, ADIE);
    SET_BIT(INTCON, 7); /* Enable Global Interrupts */
}

void ADC_StartConversion(void)
{
    SET_BIT(ADCON0, GO);
}

u16 ADC_GetResult(void)
{
    u16 Local_Result = 0;

    Local_Result = (u16)ADRESH;
    Local_Result = (Local_Result << 8) | ADRESL;

    return Local_Result;
}

u8 ADC_GetResultByte(void)
{
    return ADRESH;
}

void ADC_SetCallback(ADC_CallbackFuncPtr Copy_Ptr)
{
    if (Copy_Ptr != NULL_PTR)
        ADC_CallbackFunc = Copy_Ptr;
}

void ADC_SelectChannel(u8 Channel)
{
    u8 Local_ADCON0 = ADCON0;

    Local_ADCON0 = (u8)((Local_ADCON0 & 0xC7) | ((Channel & 0x07) << 3));

    ADCON0 = Local_ADCON0;
}

u8 ADC_GetConversionStatus(void)
{
    return GET_BIT(ADCON0, GO);
}

void ADC_InterruptHandler(void)
{
    if (ADC_CallbackFunc != NULL_PTR)
    {
        u16 Local_Result = ADC_GetResult();
        ADC_CallbackFunc(Local_Result);
    }
}
