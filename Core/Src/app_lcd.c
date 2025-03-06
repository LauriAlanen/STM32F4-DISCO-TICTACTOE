#include "app_lcd.h"

static uint16_t x_size, y_size, x_spacing, y_spacing; // Screen size, and grid spacing (only read)

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

    error = BSP_TS_Init(BSP_LCD_GetXSize(), BSP_LCD_GetYSize());
    if (error)
    {
        return 1;
    }

    x_size = BSP_LCD_GetXSize(); // 240
    y_size = BSP_LCD_GetYSize(); // 320
    x_spacing = x_size / COLS;
    y_spacing = y_size / ROWS;

    // Calculate the size for drawable icons


    return 0;
}

void APP_Draw_Board()
{
    BSP_LCD_SetTextColor(LCD_COLOR_YELLOW);

    uint16_t draw_pos = 0;
    
    for (uint8_t i = 1; i < COLS; i++)
    {
        draw_pos = i * x_spacing;
        BSP_LCD_DrawLine(draw_pos, 0, draw_pos, y_size);
    }
    
    for (uint8_t i = 1; i < ROWS; i++)
    {
        draw_pos = i * y_spacing;
        BSP_LCD_DrawLine(0, draw_pos, x_size, draw_pos);
    }
}

uint8_t APP_Draw_Circle(uint8_t column, uint8_t row)
{
    uint16_t x_icon_size, y_icon_size = 0, y_padding = 0, x_padding = 0;

    if (column > COLS || row > ROWS)
    {
        return 1;
    }

    x_icon_size = x_spacing - ICON_PADDING; 
    y_icon_size = y_spacing - ICON_PADDING; 
    y_padding = (y_spacing - y_icon_size) / 2;
    x_padding = (x_spacing - x_icon_size) / 2;
    
    uint16_t radii = x_icon_size / 2;
    uint16_t x_draw_pos = row * x_spacing + x_padding;
    uint16_t y_draw_pos = column * y_spacing + y_padding;
    
    BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
    BSP_LCD_DrawCircle(x_draw_pos, y_draw_pos, radii);
    
    return 0;
}