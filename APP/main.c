#pragma config FOSC = HS
#pragma config WDTE = OFF
#pragma config PWRTE = OFF
#pragma config BOREN = OFF
#pragma config LVP = OFF
#pragma config CPD = OFF
#pragma config WRT = OFF
#pragma config CP = OFF

#define _XTAL_FREQ 16000000

#include <xc.h>
#include "../MCAL/GPIO/GPIO_interface.h"
#include "../MCAL/UART/UART_interface.h"
#include "../SERVICES/STD_TYPES.h"
#include "../MCAL/I2C/I2C_interface.h"
#include "../HAL/LCD_I2C/LCD_I2C_interface.h"

/* LED تعريف */
#define LED_PORT GPIO_PORTA
#define LED_PIN GPIO_PIN0

void delay_ms(u16 ms)
{
    for (u16 i = 0; i < ms; i++)
        for (u16 i = 0; i < 100; i++)
            asm("nop");
}

/* UART RX Callback */
void CtrlLed(u8 data)
{
    if (data == '1')
    {
        GPIO_SetPinValue(LED_PORT, LED_PIN, GPIO_HIGH);
        LCD_I2C_SetCursor(1, 0);
        LCD_I2C_WriteString("LED ON ");
    }
    else if (data == '0')
    {
        GPIO_SetPinValue(LED_PORT, LED_PIN, GPIO_LOW);
        LCD_I2C_SetCursor(1, 0);
        LCD_I2C_WriteString("LED OFF");
    }
}

void main(void)
{
    /* Initialize GPIO */
    GPIO_Init();

    UART_Init(UART_BAUD_28800, UART_DATA_8BITS, UART_STOP_1BIT);
    eeprom_write(0x00, 0x02);
    UART_SetRXCallback(CtrlLed);
    eeprom_write(0x00, 0x03);

    /* Initialize LED */
    GPIO_SetPinDirection(LED_PORT, LED_PIN, GPIO_OUTPUT);
    GPIO_SetPinValue(LED_PORT, LED_PIN, GPIO_HIGH);
	__delay_ms(1000);
    GPIO_SetPinValue(LED_PORT, LED_PIN, GPIO_LOW);
	__delay_ms(1000);

    /* Initialize LCD */
    I2C_Init(I2C_MASTER, I2C_SPEED_100kHz);
    LCD_I2C_Init();
    GPIO_SetPinValue(LED_PORT, LED_PIN, GPIO_HIGH);
    LCD_I2C_Clear();
    LCD_I2C_SetCursor(0, 0);
	//u8 code = eeprom_read(0x01);
	char text[6];
	for (u8 i = 0; i < 6; i++){
		text[i] = eeprom_read(i) + 48;
	}
	text[5] = 0;

	LCD_I2C_WriteString(text);
	__delay_ms(3000);

    LCD_I2C_WriteString("System Start");
    eeprom_write(0x00, 0x00);
    __delay_ms(1000);
    LCD_I2C_Clear();
    LCD_I2C_SetCursor(0, 0);
    LCD_I2C_WriteString("initiaizing UART");
    eeprom_write(0x00, 0x01);

    /* Initialize UART */

    __delay_ms(1000);
    LCD_I2C_Clear();
    LCD_I2C_SetCursor(0, 0);
    while (1)
    {
        /* Send HIGH every second */
        UART_SendByte('1');
        LCD_I2C_SetCursor(0, 0);
        LCD_I2C_WriteString("Sent: 1 ");
        delay_ms(1000);

        UART_SendByte('0');
        LCD_I2C_SetCursor(0, 0);
        LCD_I2C_WriteString("Sent: 0 ");
        delay_ms(1000);
    }
}
