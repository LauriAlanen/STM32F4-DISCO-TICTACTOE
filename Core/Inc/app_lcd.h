#ifndef __APP_LCD_H
#define __APP_LCD_H

#include "stm32f429i_discovery_lcd.h"
#include "stm32f429i_discovery_ts.h"
#include "app_ts.h"

#define BOARD_SIZE 6
#define ICON_PADDING 10

extern uint16_t x_size, y_size, x_spacing, y_spacing; // Screen size, and grid spacing (only read)

uint8_t APP_LCD_Initialize();
uint8_t APP_Draw_Cross(uint8_t column, uint8_t row);
uint8_t APP_Draw_Circle(uint8_t column, uint8_t row);
void APP_Draw_Board();
void APP_Draw_Text(uint16_t Line, uint8_t *ptr);

#endif