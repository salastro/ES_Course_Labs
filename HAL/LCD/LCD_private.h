#ifndef LCD_PRIVATE_H
#define LCD_PRIVATE_H

#define LCD_CMD_CLEAR_DISPLAY      0x01U
#define LCD_CMD_RETURN_HOME        0x02U
#define LCD_CMD_ENTRY_MODE_SET     0x06U
#define LCD_CMD_DISPLAY_OFF        0x08U
#define LCD_CMD_DISPLAY_ON         0x0CU
#define LCD_CMD_FUNCTION_SET       0x28U
#define LCD_CMD_SET_DDRAM_ADDRESS  0x80U

#define LCD_COLS                   16U
#define LCD_ROWS                   2U

#define LCD_POWER_ON_DELAY_MS      20U
#define LCD_INIT_DELAY_MS           5U
#define LCD_SHORT_DELAY_MS          1U
#define LCD_LONG_DELAY_MS           2U

#define LCD_STROBE_DELAY_ITER      20U
#define LCD_MS_DELAY_ITERATIONS  1600U

#endif