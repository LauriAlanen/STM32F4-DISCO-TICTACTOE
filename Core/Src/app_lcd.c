#include "app_lcd.h"

CPU_INT16U x_size, y_size, x_spacing, y_spacing;

OS_SEM LCD_Semaphore; 

CPU_INT08U APP_LCD_Initialize()
{
    CPU_INT08U error;

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
    CPU_INT16U draw_pos = 0;
    
    BSP_LCD_SetTextColor(LCD_COLOR_YELLOW);
    
    for (CPU_INT08U i = 1; i < BOARD_SIZE; i++)
    {
        draw_pos = i * x_spacing;
        BSP_LCD_DrawLine(draw_pos, 0, draw_pos, y_size);
    }
    
    for (CPU_INT08U i = 1; i < BOARD_SIZE; i++)
    {
        draw_pos = i * y_spacing;
        BSP_LCD_DrawLine(0, draw_pos, x_size, draw_pos);
    }
}

CPU_INT08U APP_Draw_Circle(CPU_INT08U column, CPU_INT08U row)
{
    if (column > BOARD_SIZE || row > BOARD_SIZE)
    {
        return 1;
    }

    CPU_INT16U radii = (x_spacing - ICON_PADDING) / 2;
    CPU_INT16U x_draw_pos = (row * x_spacing) + x_spacing / 2;
    CPU_INT16U y_draw_pos = (column * y_spacing) + y_spacing / 2;
    
    BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
    BSP_LCD_DrawCircle(x_draw_pos, y_draw_pos, radii);

    return 0;
}

void APP_Draw_Text(CPU_INT16U Line, CPU_INT08U *ptr)
{
    CPU_INT32U color = BSP_LCD_GetTextColor();
    
    BSP_LCD_SetTextColor(LCD_COLOR_BLACK); 
    BSP_LCD_DisplayStringAtLine(Line, ptr);
    BSP_LCD_SetTextColor(color);
}