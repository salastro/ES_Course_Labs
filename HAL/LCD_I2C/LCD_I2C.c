#define _XTAL_FREQ 16000000

#include <xc.h>

#include "LCD_I2C_interface.h"
#include "LCD_I2C_private.h"
#include "LCD_I2C_config.h"
#include "../../MCAL/I2C/I2C_interface.h"

static void LCD_I2C_WriteExpander(u8 Data)
{
    I2C_StartCondition();
    I2C_SendByte((u8)(LCD_I2C_ADDRESS << 1));

    if (I2C_ReadAck() == I2C_ACK_RECEIVED)
    {
        I2C_SendByte(Data);
        (void)I2C_ReadAck();
    }

    I2C_StopCondition();
}

static void LCD_I2C_PulseEnable(u8 Data)
{
    LCD_I2C_WriteExpander((u8)(Data | LCD_I2C_ENABLE));
    __delay_us(1);
    LCD_I2C_WriteExpander((u8)(Data & (u8)(~LCD_I2C_ENABLE)));
    __delay_us(50);
}

static void LCD_I2C_SendNibble(u8 Data, u8 IsData)
{
    u8 Local_Control = LCD_I2C_BACKLIGHT;

    if (IsData != 0U)
    {
        Local_Control |= LCD_I2C_REGISTERSELECT;
    }

    LCD_I2C_PulseEnable((u8)((Data & 0xF0U) | Local_Control));
}

static void LCD_I2C_SendByte(u8 Data, u8 IsData)
{
    LCD_I2C_SendNibble(Data, IsData);
    LCD_I2C_SendNibble((u8)(Data << 4), IsData);
}

static void LCD_I2C_SendCommand(u8 Command)
{
    LCD_I2C_SendByte(Command, 0U);
}

static void LCD_I2C_SendData(u8 Data)
{
    LCD_I2C_SendByte(Data, 1U);
}

static u8 LCD_I2C_UIntToString(u16 Number, char *Text)
{
    char Local_Reverse[5];
    u8 Local_Length = 0U;
    u8 Local_Index = 0U;

    if (Number == 0U)
    {
        Text[0] = '0';
        Text[1] = '\0';
        return 1U;
    }

    while ((Number > 0U) && (Local_Length < 5U))
    {
        Local_Reverse[Local_Length] = (char)('0' + (Number % 10U));
        Number /= 10U;
        Local_Length++;
    }

    while (Local_Length > 0U)
    {
        Local_Length--;
        Text[Local_Index] = Local_Reverse[Local_Length];
        Local_Index++;
    }

    Text[Local_Index] = '\0';

    return Local_Index;
}

void LCD_I2C_Init(void)
{
    __delay_ms(50);

    LCD_I2C_SendNibble(0x30U, 0U);
    __delay_ms(5);
    LCD_I2C_SendNibble(0x30U, 0U);
    __delay_us(150);
    LCD_I2C_SendNibble(0x30U, 0U);
    LCD_I2C_SendNibble(0x20U, 0U);

    LCD_I2C_SendCommand(LCD_I2C_FUNCTION_SET);
    LCD_I2C_SendCommand(LCD_I2C_DISPLAY_OFF);
    LCD_I2C_SendCommand(LCD_I2C_CLEAR_DISPLAY);
    __delay_ms(2);
    LCD_I2C_SendCommand(LCD_I2C_ENTRY_MODE);
    LCD_I2C_SendCommand(LCD_I2C_DISPLAY_ON);
}

void LCD_I2C_Clear(void)
{
    LCD_I2C_SendCommand(LCD_I2C_CLEAR_DISPLAY);
    __delay_ms(2);
}

void LCD_I2C_SetCursor(u8 Row, u8 Col)
{
    u8 Local_Address = Col;

    if (Row != 0U)
    {
        Local_Address = (u8)(0x40U + Col);
    }

    LCD_I2C_SendCommand((u8)(0x80U | Local_Address));
}

void LCD_I2C_WriteChar(u8 Data)
{
    LCD_I2C_SendData(Data);
}

void LCD_I2C_WriteString(const char *Text)
{
    while (*Text != '\0')
    {
        LCD_I2C_WriteChar((u8)*Text);
        Text++;
    }
}

void LCD_I2C_WriteUnsigned(u16 Number)
{
    char Local_Text[6];

    (void)LCD_I2C_UIntToString(Number, Local_Text);
    LCD_I2C_WriteString(Local_Text);
}

void LCD_I2C_WriteUnsignedPadded(u16 Number, u8 Width)
{
    char Local_Text[6];
    u8 Local_Length;
    u8 Local_Padding = 0U;

    Local_Length = LCD_I2C_UIntToString(Number, Local_Text);

    if (Width > Local_Length)
    {
        Local_Padding = (u8)(Width - Local_Length);
    }

    while (Local_Padding > 0U)
    {
        LCD_I2C_WriteChar((u8)' ');
        Local_Padding--;
    }

    LCD_I2C_WriteString(Local_Text);
}