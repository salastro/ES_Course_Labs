#ifndef LCD_I2C_INTERFACE_H
#define LCD_I2C_INTERFACE_H

#include "../../SERVICES/STD_TYPES.h"

void LCD_I2C_Init(void);
void LCD_I2C_Clear(void);
void LCD_I2C_SetCursor(u8 Row, u8 Col);
void LCD_I2C_WriteChar(u8 Data);
void LCD_I2C_WriteString(const char *Text);
void LCD_I2C_WriteUnsigned(u16 Number);
void LCD_I2C_WriteUnsignedPadded(u16 Number, u8 Width);

#endif