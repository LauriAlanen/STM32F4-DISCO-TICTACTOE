#include "app_lcd.h"

uint8_t APP_LCD_Initialize()
{
    BSP_LCD_Init();
    BSP_LCD_LayerDefaultInit(LCD_BACKGROUND_LAYER, LCD_FRAME_BUFFER);
    BSP_LCD_LayerDefaultInit(LCD_FOREGROUND_LAYER, LCD_FRAME_BUFFER);
    BSP_LCD_SelectLayer(LCD_BACKGROUND_LAYER);
    BSP_LCD_DisplayOn();
    BSP_LCD_Clear(LCD_COLOR_BLACK);
    BSP_LCD_SetTextColor(LCD_COLOR_WHITE);

    return 0;
}

void APP_Draw_Board()
{
    BSP_LCD_SetTextColor(LCD_COLOR_YELLOW);

    uint16_t x = BSP_LCD_GetXSize() - 1; // 240 + SPACING
    uint16_t y = BSP_LCD_GetYSize(); // 320
    uint16_t x_spacing = x / COLS;
    uint16_t y_spacing = y / ROWS;
    
    uint16_t draw_pos = 0;

    for (uint8_t i = 1; i < COLS; i++)
    {
        draw_pos = i * x_spacing;
        BSP_LCD_DrawLine(draw_pos, 0, draw_pos, y);
    }

    for (uint8_t i = 1; i < ROWS; i++)
    {
        draw_pos = i * y_spacing;
        BSP_LCD_DrawLine(0, draw_pos, x, draw_pos);
    }
}
