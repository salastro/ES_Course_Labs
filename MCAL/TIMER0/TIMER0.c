#include "TIMER0_interface.h"
#include "TIMER0_private.h"
#include "TIMER0_config.h"
#include "../../SERVICES/BIT_MATH.h"

static TIMER0_CallbackFuncPtr TIMER0_CallbackFunc = NULL_PTR;

void TIMER0_Init(u8 Mode, u8 Prescaler, u8 IntEnable)
{
    u8 Local_OptionReg = OPTION_REG;

    if (Mode == TIMER0_TIMER_MODE)
        CLR_BIT(Local_OptionReg, T0CS);
    else
        SET_BIT(Local_OptionReg, T0CS);

    CLR_BIT(Local_OptionReg, PSA);
    Local_OptionReg = (Local_OptionReg & 0xF8) | Prescaler;

    OPTION_REG = Local_OptionReg;

    CLR_BIT(INTCON, T0IF);

    if (IntEnable == TIMER0_INT_ENABLE)
    {
        SET_BIT(INTCON, T0IE);
        SET_BIT(INTCON, GIE);
    }
}

void TIMER0_SetValue(u8 TimerValue)
{
    TMR0 = TimerValue;
}

u8 TIMER0_GetValue(void)
{
    return TMR0;
}

void TIMER0_SetCallback(TIMER0_CallbackFuncPtr Copy_Ptr)
{
    if (Copy_Ptr != NULL_PTR)
        TIMER0_CallbackFunc = Copy_Ptr;
}

void TIMER0_Start(void)
{
    /* Timer0 starts immediately after configuration */
}

void TIMER0_Stop(void)
{
    /* Stop Timer0 by disabling the interrupt */
    CLR_BIT(INTCON, T0IE);
}

void TIMER0_InterruptHandler(void)
{
    if (TIMER0_CallbackFunc != NULL_PTR)
        TIMER0_CallbackFunc();
}
