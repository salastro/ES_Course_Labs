#include "LCD_interface.h"
#include "LCD_private.h"
#include "LCD_config.h"

#include "../../MCAL/GPIO/GPIO_interface.h"
#include "../../SERVICES/BIT_MATH.h"

static void LCD_DelayLoop(u16 Iterations)
{
    volatile u16 Local_I = 0U;

    for (Local_I = 0U; Local_I < Iterations; Local_I++)
    {
        ;
    }
}

static void LCD_DelayMs(u16 Ms)
{
    u16 Local_I = 0U;
    u16 Local_J = 0U;

    for (Local_I = 0U; Local_I < Ms; Local_I++)
    {
        for (Local_J = 0U; Local_J < LCD_MS_DELAY_ITERATIONS; Local_J++)
        {
            ;
        }
    }
}

static void LCD_PulseEnable(const LCD_t *Display)
{
    GPIO_SetPinValue(Display->EN_Port, Display->EN_Pin, GPIO_LOW);
    LCD_DelayLoop(LCD_STROBE_DELAY_ITER);
    GPIO_SetPinValue(Display->EN_Port, Display->EN_Pin, GPIO_HIGH);
    LCD_DelayLoop(LCD_STROBE_DELAY_ITER);
    GPIO_SetPinValue(Display->EN_Port, Display->EN_Pin, GPIO_LOW);
    LCD_DelayLoop(LCD_STROBE_DELAY_ITER);
}

static void LCD_Write4Bits(const LCD_t *Display, u8 Data)
{
    GPIO_SetPinValue(Display->D4_Port, Display->D4_Pin, (Data & 0x01U) != 0U ? GPIO_HIGH : GPIO_LOW);
    GPIO_SetPinValue(Display->D5_Port, Display->D5_Pin, (Data & 0x02U) != 0U ? GPIO_HIGH : GPIO_LOW);
    GPIO_SetPinValue(Display->D6_Port, Display->D6_Pin, (Data & 0x04U) != 0U ? GPIO_HIGH : GPIO_LOW);
    GPIO_SetPinValue(Display->D7_Port, Display->D7_Pin, (Data & 0x08U) != 0U ? GPIO_HIGH : GPIO_LOW);

    LCD_PulseEnable(Display);
}

static void LCD_SendByte(const LCD_t *Display, u8 Data, u8 IsData)
{
    if (Display == NULL_PTR)
    {
        return;
    }

    GPIO_SetPinValue(Display->RS_Port, Display->RS_Pin, (IsData != 0U) ? GPIO_HIGH : GPIO_LOW);
    LCD_Write4Bits(Display, (u8)((Data >> 4U) & 0x0FU));
    LCD_Write4Bits(Display, (u8)(Data & 0x0FU));
}

static void LCD_InitializePins(const LCD_t *Display)
{
    GPIO_SetPinDirection(Display->RS_Port, Display->RS_Pin, GPIO_OUTPUT);
    GPIO_SetPinDirection(Display->EN_Port, Display->EN_Pin, GPIO_OUTPUT);
    GPIO_SetPinDirection(Display->D4_Port, Display->D4_Pin, GPIO_OUTPUT);
    GPIO_SetPinDirection(Display->D5_Port, Display->D5_Pin, GPIO_OUTPUT);
    GPIO_SetPinDirection(Display->D6_Port, Display->D6_Pin, GPIO_OUTPUT);
    GPIO_SetPinDirection(Display->D7_Port, Display->D7_Pin, GPIO_OUTPUT);

    GPIO_SetPinValue(Display->RS_Port, Display->RS_Pin, GPIO_LOW);
    GPIO_SetPinValue(Display->EN_Port, Display->EN_Pin, GPIO_LOW);
    GPIO_SetPinValue(Display->D4_Port, Display->D4_Pin, GPIO_LOW);
    GPIO_SetPinValue(Display->D5_Port, Display->D5_Pin, GPIO_LOW);
    GPIO_SetPinValue(Display->D6_Port, Display->D6_Pin, GPIO_LOW);
    GPIO_SetPinValue(Display->D7_Port, Display->D7_Pin, GPIO_LOW);
}

void LCD_Init(const LCD_t *Display)
{
    if (Display == NULL_PTR)
    {
        return;
    }

    LCD_InitializePins(Display);
    LCD_DelayMs(LCD_POWER_ON_DELAY_MS);

    GPIO_SetPinValue(Display->RS_Port, Display->RS_Pin, GPIO_LOW);

    LCD_Write4Bits(Display, 0x03U);
    LCD_DelayMs(LCD_INIT_DELAY_MS);
    LCD_Write4Bits(Display, 0x03U);
    LCD_DelayLoop(100U);
    LCD_Write4Bits(Display, 0x03U);
    LCD_DelayLoop(100U);
    LCD_Write4Bits(Display, 0x02U);
    LCD_DelayLoop(100U);

    LCD_SendCommand(Display, LCD_CMD_FUNCTION_SET);
    LCD_SendCommand(Display, LCD_CMD_DISPLAY_OFF);
    LCD_Clear(Display);
    LCD_SendCommand(Display, LCD_CMD_ENTRY_MODE_SET);
    LCD_SendCommand(Display, LCD_CMD_DISPLAY_ON);
}

void LCD_Clear(const LCD_t *Display)
{
    LCD_SendCommand(Display, LCD_CMD_CLEAR_DISPLAY);
    LCD_DelayMs(LCD_LONG_DELAY_MS);
}

void LCD_ReturnHome(const LCD_t *Display)
{
    LCD_SendCommand(Display, LCD_CMD_RETURN_HOME);
    LCD_DelayMs(LCD_LONG_DELAY_MS);
}

void LCD_SendCommand(const LCD_t *Display, u8 Command)
{
    if (Display == NULL_PTR)
    {
        return;
    }

    LCD_SendByte(Display, Command, 0U);

    if ((Command == LCD_CMD_CLEAR_DISPLAY) || (Command == LCD_CMD_RETURN_HOME))
    {
        LCD_DelayMs(LCD_LONG_DELAY_MS);
    }
    else
    {
        LCD_DelayMs(LCD_SHORT_DELAY_MS);
    }
}

void LCD_SendData(const LCD_t *Display, u8 Data)
{
    if (Display == NULL_PTR)
    {
        return;
    }

    LCD_SendByte(Display, Data, 1U);
    LCD_DelayMs(LCD_SHORT_DELAY_MS);
}

void LCD_SendString(const LCD_t *Display, const char *String)
{
    if ((Display == NULL_PTR) || (String == NULL_PTR))
    {
        return;
    }

    while (*String != '\0')
    {
        LCD_SendData(Display, (u8)(*String));
        String++;
    }
}

void LCD_GotoXY(const LCD_t *Display, u8 Row, u8 Column)
{
    u8 Local_Address = 0U;

    if (Display == NULL_PTR)
    {
        return;
    }

    if (Column >= LCD_COLS)
    {
        Column = (u8)(LCD_COLS - 1U);
    }

    if (Row >= LCD_ROWS)
    {
        Row = (u8)(LCD_ROWS - 1U);
    }

    if (Row == 0U)
    {
        Local_Address = Column;
    }
    else
    {
        Local_Address = (u8)(0x40U + Column);
    }

    LCD_SendCommand(Display, (u8)(LCD_CMD_SET_DDRAM_ADDRESS | Local_Address));
}

void LCD_WriteNumber(const LCD_t *Display, s32 Number)
{
    char Local_Buffer[11];
    u8 Local_Index = 0U;
    u32 Local_Value = 0U;

    if (Display == NULL_PTR)
    {
        return;
    }

    if (Number == 0)
    {
        LCD_SendData(Display, (u8)'0');
        return;
    }

    if (Number < 0)
    {
        LCD_SendData(Display, (u8)'-');
        Local_Value = (u32)(-(Number + 1)) + 1U;
    }
    else
    {
        Local_Value = (u32)Number;
    }

    while (Local_Value > 0U)
    {
        Local_Buffer[Local_Index] = (char)('0' + (Local_Value % 10U));
        Local_Value /= 10U;
        Local_Index++;
    }

    while (Local_Index > 0U)
    {
        Local_Index--;
        LCD_SendData(Display, (u8)Local_Buffer[Local_Index]);
    }
}

void LCD_WriteChar(const LCD_t *Display, char Character)
{
    LCD_SendData(Display, (u8)Character);
}