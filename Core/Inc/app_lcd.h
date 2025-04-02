#ifndef APP_LCD_H
#define APP_LCD_H

#include "stm32f429i_discovery_lcd.h"
#include "stm32f429i_discovery_ts.h"
#include "app_ts.h"
#include "utils.h"
#include "os.h"

#define BOARD_SIZE 10
#define ICON_PADDING (50 / BOARD_SIZE)

extern CPU_INT16U x_size, y_size, x_spacing, y_spacing; // Screen size, and grid spacing (only read)
extern OS_SEM LCD_Semaphore;
extern OS_SEM LCD_Semaphore; 

CPU_INT08U APP_LCD_Initialize();
CPU_INT08U APP_Draw_Cross(CPU_INT08U column, CPU_INT08U row);
CPU_INT08U APP_Draw_Circle(CPU_INT08U column, CPU_INT08U row);
void APP_Draw_Board();
void APP_Draw_Text(CPU_INT16U Line, CPU_INT08U *ptr);

#endif