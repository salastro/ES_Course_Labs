#include "INT_interface.h"
#include "INT_private.h"
#include "INT_config.h"
#include "../../SERVICES/BIT_MATH.h"

static INT_CallbackFuncPtr INT_CallbackFunc = NULL_PTR;

void INT_Init(u8 IntEdge)
{
    if (IntEdge == INT_RISING_EDGE)
        CLR_BIT(OPTION_REG, INTEDG);
    else
        SET_BIT(OPTION_REG, INTEDG);

    CLR_BIT(INTCON, INTF);

    SET_BIT(INTCON, GIE);
}

void INT_Enable(void)
{
    SET_BIT(INTCON, INTE);
}

void INT_Disable(void)
{
    CLR_BIT(INTCON, INTE);
}

void INT_SetCallback(INT_CallbackFuncPtr Copy_Ptr)
{
    if (Copy_Ptr != NULL_PTR)
        INT_CallbackFunc = Copy_Ptr;
}

void INT0_InterruptHandler(void)
{
    if (INT_CallbackFunc != NULL_PTR)
        INT_CallbackFunc();
}
