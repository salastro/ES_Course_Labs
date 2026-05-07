#include "TIMER0_interface.h"
#include "TIMER0_private.h"
#include "TIMER0_config.h"
#include "../../SERVICES/BIT_MATH.h"

static TIMER0_CallbackFuncPtr TIMER0_CallbackFunc = NULL_PTR;

/* Periodic task system */
static TIMER0_PeriodicTaskFuncPtr TIMER0_PeriodicTaskFunc = NULL_PTR;
static u16 TIMER0_PeriodicTaskInterval = 0U;
static u16 TIMER0_PeriodicTaskLastMs = 0U;

/* Millisecond timing counter for system-wide use */
static u16 TIMER0_SystemMs = 0U;
static u8 TIMER0_MsCounter = 0U;

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

void TIMER0_SetPeriodicTask(TIMER0_PeriodicTaskFuncPtr Copy_Ptr, u16 IntervalMs)
{
    if (Copy_Ptr != NULL_PTR)
    {
        TIMER0_PeriodicTaskFunc = Copy_Ptr;
        TIMER0_PeriodicTaskInterval = IntervalMs;
        TIMER0_PeriodicTaskLastMs = TIMER0_SystemMs;
    }
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
    /* Increment millisecond counter (TIMER0 fires ~8 times per millisecond with prescaler 8) */
    if (++TIMER0_MsCounter >= 8U)
    {
        TIMER0_MsCounter = 0U;
        TIMER0_SystemMs++;
    }

    /* Call primary callback if registered */
    if (TIMER0_CallbackFunc != NULL_PTR)
        TIMER0_CallbackFunc();

    /* Check if periodic task interval has elapsed */
    if (TIMER0_PeriodicTaskFunc != NULL_PTR)
    {
        u16 ElapsedMs = TIMER0_SystemMs - TIMER0_PeriodicTaskLastMs;
        if (ElapsedMs >= TIMER0_PeriodicTaskInterval)
        {
            TIMER0_PeriodicTaskLastMs = TIMER0_SystemMs;
            TIMER0_PeriodicTaskFunc();
        }
    }
}

/* Get elapsed milliseconds since system start */
u16 TIMER0_GetSystemMs(void)
{
    return TIMER0_SystemMs;
}
