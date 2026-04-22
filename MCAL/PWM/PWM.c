#include "PWM_interface.h"
#include "PWM_private.h"
#include "PWM_config.h"
#include "../../SERVICES/BIT_MATH.h"

static u16 PWM_Period = 0;

void PWM_Init(u8 Channel, u8 Frequency)
{
    u8 Local_PR2Value = 0;
    u8 Local_T2CONValue = 0;

    switch (Frequency)
    {
    case PWM_FREQ_1kHz:
        Local_PR2Value = 249;
        Local_T2CONValue = 0x05;
        PWM_Period = 250;
        break;

    case PWM_FREQ_2kHz:
        Local_PR2Value = 124;
        Local_T2CONValue = 0x05;
        PWM_Period = 125;
        break;

    case PWM_FREQ_3kHz:
        Local_PR2Value = 82;
        Local_T2CONValue = 0x05;
        PWM_Period = 83;
        break;

    case PWM_FREQ_5kHz:
        Local_PR2Value = 49;
        Local_T2CONValue = 0x05;
        PWM_Period = 50;
        break;

    case PWM_FREQ_10kHz:
        Local_PR2Value = 24;
        Local_T2CONValue = 0x05;
        PWM_Period = 25;
        break;

    case PWM_FREQ_20kHz:
        Local_PR2Value = 11;
        Local_T2CONValue = 0x05;
        PWM_Period = 12;
        break;

    default:
        Local_PR2Value = 49;
        Local_T2CONValue = 0x05;
        PWM_Period = 50;
        break;
    }

    PR2 = Local_PR2Value;
    T2CON = Local_T2CONValue;

    if (Channel == PWM_CHANNEL1)
    {
        CCP1CON = 0x0C;
        CCPR1L = 0;
    }
    else if (Channel == PWM_CHANNEL2)
    {
        CCP2CON = 0x0C;
        CCPR2L = 0;
    }
}

void PWM_SetDutyCycle(u8 Channel, u8 DutyCycle)
{
    u16 Local_Temp = 0;

    if (DutyCycle > 100)
        DutyCycle = 100;

    Local_Temp = (PWM_Period * (u16)DutyCycle) / 100;
    Local_Temp = Local_Temp << 2;

    if (Channel == PWM_CHANNEL1)
    {
        CCPR1L = (u8)(Local_Temp >> 2);
    }
    else if (Channel == PWM_CHANNEL2)
    {
        CCPR2L = (u8)(Local_Temp >> 2);
    }
}

u8 PWM_GetDutyCycle(u8 Channel)
{
    u16 Local_Temp = 0;

    if (Channel == PWM_CHANNEL1)
    {
        Local_Temp = ((u16)CCPR1L) << 2;
    }
    else if (Channel == PWM_CHANNEL2)
    {
        Local_Temp = ((u16)CCPR2L) << 2;
    }

    if (PWM_Period == 0)
        return 0;

    return (u8)((Local_Temp * 100) / (PWM_Period << 2));
}

void PWM_Start(u8 Channel)
{
    if (Channel == PWM_CHANNEL1)
        SET_BIT(CCP1CON, CCP1M3);
    else if (Channel == PWM_CHANNEL2)
        SET_BIT(CCP2CON, CCP1M3);
}

void PWM_Stop(u8 Channel)
{
    if (Channel == PWM_CHANNEL1)
    {
        CCP1CON = 0;
        CCPR1L = 0;
    }
    else if (Channel == PWM_CHANNEL2)
    {
        CCP2CON = 0;
        CCPR2L = 0;
    }
}
