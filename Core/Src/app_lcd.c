#include "app_lcd.h"
#include <stdio.h>
#include "os.h"

uint16_t x_size, y_size, x_spacing, y_spacing;

uint8_t APP_LCD_Initialize()
{
    uint8_t error;

    error = BSP_LCD_Init();
    if (error)
    {
        return 1;
    }
    
    BSP_LCD_LayerDefaultInit(LCD_BACKGROUND_LAYER, LCD_FRAME_BUFFER);
    BSP_LCD_LayerDefaultInit(LCD_FOREGROUND_LAYER, LCD_FRAME_BUFFER);
    BSP_LCD_SelectLayer(LCD_BACKGROUND_LAYER);
    BSP_LCD_DisplayOn();
    BSP_LCD_Clear(LCD_COLOR_BLACK);
    BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
    
    APP_TS_Init();    

    x_size = BSP_LCD_GetXSize(); // 240
    y_size = BSP_LCD_GetYSize(); // 320
    x_spacing = x_size / BOARD_SIZE;
    y_spacing = y_size / BOARD_SIZE;

    return 0;
}

void APP_Draw_Board()
{
    uint16_t draw_pos = 0;
    
    BSP_LCD_SetTextColor(LCD_COLOR_YELLOW);
    
    for (uint8_t i = 1; i < BOARD_SIZE; i++)
    {
        draw_pos = i * x_spacing;
        BSP_LCD_DrawLine(draw_pos, 0, draw_pos, y_size);
    }
    
    for (uint8_t i = 1; i < BOARD_SIZE; i++)
    {
        draw_pos = i * y_spacing;
        BSP_LCD_DrawLine(0, draw_pos, x_size, draw_pos);
    }
}

uint8_t APP_Draw_Circle(uint8_t column, uint8_t row)
{
    if (column > BOARD_SIZE || row > BOARD_SIZE)
    {
        return 1;
    }

    uint16_t radii = (x_spacing - ICON_PADDING) / 2;
    uint16_t x_draw_pos = (row * x_spacing) + x_spacing / 2;
    uint16_t y_draw_pos = (column * y_spacing) + y_spacing / 2;
    
    BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
    BSP_LCD_DrawCircle(x_draw_pos, y_draw_pos, radii);
    
    return 0;
}

void APP_Draw_Text(uint16_t Line, uint8_t *ptr)
{
    uint32_t color = BSP_LCD_GetTextColor();
    
    BSP_LCD_SetTextColor(LCD_COLOR_BLACK); 
    BSP_LCD_DisplayStringAtLine(Line, ptr);
    BSP_LCD_SetTextColor(color);
}