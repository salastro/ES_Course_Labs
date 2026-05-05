#ifndef LCD_INTERFACE_H
#define LCD_INTERFACE_H

#include "../../SERVICES/STD_TYPES.h"

typedef struct
{
    u8 RS_Port;
    u8 RS_Pin;
    u8 EN_Port;
    u8 EN_Pin;
    u8 D4_Port;
    u8 D4_Pin;
    u8 D5_Port;
    u8 D5_Pin;
    u8 D6_Port;
    u8 D6_Pin;
    u8 D7_Port;
    u8 D7_Pin;
} LCD_t;

void LCD_Init(const LCD_t *Display);
void LCD_Clear(const LCD_t *Display);
void LCD_ReturnHome(const LCD_t *Display);
void LCD_SendCommand(const LCD_t *Display, u8 Command);
void LCD_SendData(const LCD_t *Display, u8 Data);
void LCD_SendString(const LCD_t *Display, const char *String);
void LCD_GotoXY(const LCD_t *Display, u8 Row, u8 Column);
void LCD_WriteNumber(const LCD_t *Display, s32 Number);
void LCD_WriteChar(const LCD_t *Display, char Character);

#endif