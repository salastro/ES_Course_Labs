#ifndef LCD_INTERFACE_H
#define LCD_INTERFACE_H

#include "../../SERVICES/STD_TYPES.h"

void LCD_Init(void);
void LCD_Clear(void);
void LCD_SetCursor(u8 Row, u8 Col);
void LCD_WriteChar(u8 Data);
void LCD_WriteString(const char *Text);
void LCD_WriteUnsigned(u16 Number);
void LCD_WriteUnsignedPadded(u16 Number, u8 Width);

#endif