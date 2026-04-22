#ifndef ADC_INTERFACE_H
#define ADC_INTERFACE_H

#include "../../SERVICES/STD_TYPES.h"

/* ADC Channels */
#define ADC_CHANNEL0 0
#define ADC_CHANNEL1 1
#define ADC_CHANNEL2 2
#define ADC_CHANNEL3 3
#define ADC_CHANNEL4 4
#define ADC_CHANNEL5 5
#define ADC_CHANNEL6 6
#define ADC_CHANNEL7 7

/* ADC Reference Voltage */
#define ADC_REF_VSS_VDD 0
#define ADC_REF_VREF_VSS 1
#define ADC_REF_VSS_VREF 2
#define ADC_REF_VREF_VREF 3

/* ADC Clock Prescaler */
#define ADC_PRESCALER_2 0
#define ADC_PRESCALER_8 1
#define ADC_PRESCALER_32 2
#define ADC_PRESCALER_OSC 3

/* ADC Result Mode */
#define ADC_RIGHT_JUSTIFIED 0
#define ADC_LEFT_JUSTIFIED 1

/* Function Pointers */
typedef void (*ADC_CallbackFuncPtr)(u16 Result);

/* Function Prototypes */
void ADC_Init(u8 Channel, u8 RefVoltage, u8 Prescaler);
void ADC_StartConversion(void);
u16 ADC_GetResult(void);
u8 ADC_GetResultByte(void);
void ADC_SetCallback(ADC_CallbackFuncPtr Copy_Ptr);
void ADC_SelectChannel(u8 Channel);
u8 ADC_GetConversionStatus(void);
void ADC_InterruptHandler(void); /* Called from INT_Manager */
#endif
