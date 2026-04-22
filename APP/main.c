#include "../HAL/LED/LED_interface.h"
#include "../MCAL/GPIO/GPIO_interface.h"
#include "../MCAL/INT/INT_interface.h"
#include "../MCAL/INT_Manager/INT_Manager.h"
#include "../MCAL/TIMER0/TIMER0_interface.h"
#include "../MCAL/PWM/PWM_interface.h"
#include "../MCAL/ADC/ADC_interface.h"
#include "../MCAL/UART/UART_interface.h"
#include "../SERVICES/STD_TYPES.h"

volatile u8 ButtonPressed = 0;
volatile u8 Timer0Count = 0;
u16 ADCValue = 0;
u8 PWMDutyCycle = 50;

void delay_ms(u16 ms)
{
    unsigned int i, j;
    for (i = 0; i < ms; i++)
        for (j = 0; j < 123; j++)
            ;
}

void Button_Pressed_Callback(void)
{
    ButtonPressed = 1;
    UART_SendString((u8 *)"Button Pressed!\r\n");
}

void Timer0_Overflow_Callback(void)
{
    Timer0Count++;

    if (Timer0Count >= 5)
    {
        Timer0Count = 0;
        LED_Toggle(GPIO_PORTB, GPIO_PIN0);
    }
}

void ADC_Conversion_Complete_Callback(u16 Result)
{
    ADCValue = Result;

    PWMDutyCycle = (u8)((ADCValue * 100) / 1024);

    PWM_SetDutyCycle(PWM_CHANNEL1, PWMDutyCycle);
}

void UART_RX_Callback(u8 Data)
{
    UART_SendByte(Data);

    switch (Data)
    {
    case '1':
        UART_SendString((u8 *)"LED ON\r\n");
        LED_On(GPIO_PORTB, GPIO_PIN0);
        break;

    case '0':
        UART_SendString((u8 *)"LED OFF\r\n");
        LED_Off(GPIO_PORTB, GPIO_PIN0);
        break;

    case 'T':
        UART_SendString((u8 *)"LED TOGGLE\r\n");
        LED_Toggle(GPIO_PORTB, GPIO_PIN0);
        break;

    case 'A':
    {
        u8 ADC_High = (u8)(ADCValue >> 8);
        u8 ADC_Low = (u8)(ADCValue & 0xFF);
        UART_SendByte(ADC_High);
        UART_SendByte(ADC_Low);
    }
    break;

    case 'P':
        UART_SendByte(PWMDutyCycle);
        break;

    default:
        UART_SendString((u8 *)"Unknown Command\r\n");
        break;
    }
}

void UART_TX_Callback(void)
{
}

void GPIO_Devices_Init(void)
{
    GPIO_SetPinDirection(GPIO_PORTB, GPIO_PIN0, GPIO_OUTPUT);
    GPIO_SetPinValue(GPIO_PORTB, GPIO_PIN0, GPIO_LOW);

    GPIO_SetPinDirection(GPIO_PORTB, GPIO_PIN1, GPIO_OUTPUT);
    GPIO_SetPinValue(GPIO_PORTB, GPIO_PIN1, GPIO_LOW);

    GPIO_SetPinDirection(GPIO_PORTA, GPIO_PIN0, GPIO_INPUT);

    GPIO_SetPinDirection(GPIO_PORTA, GPIO_PIN2, GPIO_INPUT);
}

void Peripherals_Init(void)
{
    IntManager_Init();

    GPIO_Devices_Init();

    INT_Init(INT_RISING_EDGE);
    INT_SetCallback(Button_Pressed_Callback);
    INT_Enable();

    TIMER0_Init(TIMER0_TIMER_MODE, TIMER0_PRESCALER_256, TIMER0_INT_ENABLE);
    TIMER0_SetCallback(Timer0_Overflow_Callback);
    TIMER0_SetValue(0);
    TIMER0_Start();

    PWM_Init(PWM_CHANNEL1, PWM_FREQ_5kHz);
    PWM_SetDutyCycle(PWM_CHANNEL1, 50);
    PWM_Start(PWM_CHANNEL1);

    ADC_Init(ADC_CHANNEL2, ADC_REF_VSS_VDD, ADC_PRESCALER_8);
    ADC_SetCallback(ADC_Conversion_Complete_Callback);

    UART_Init(UART_BAUD_9600, UART_DATA_8BITS, UART_STOP_1BIT);
    UART_SetRXCallback(UART_RX_Callback);
    UART_SetTXCallback(UART_TX_Callback);

    delay_ms(100);
    UART_SendString((u8 *)"\r\n=== Peripheral Drivers Example ===\r\n");
    UART_SendString((u8 *)"Commands:\r\n");
    UART_SendString((u8 *)"1 - LED ON\r\n");
    UART_SendString((u8 *)"0 - LED OFF\r\n");
    UART_SendString((u8 *)"T - LED TOGGLE\r\n");
    UART_SendString((u8 *)"A - Get ADC Value\r\n");
    UART_SendString((u8 *)"P - Get PWM Duty Cycle\r\n");
    UART_SendString((u8 *)"\r\n");
}

void main(void)
{
    Peripherals_Init();

    while (1)
    {
        ADC_SelectChannel(ADC_CHANNEL2);
        ADC_StartConversion();

        if (ButtonPressed)
        {
            ButtonPressed = 0;
            LED_Toggle(GPIO_PORTB, GPIO_PIN0);
            delay_ms(20);
        }

        delay_ms(100);
    }
}