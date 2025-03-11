#ifndef APP_TS_H
#define APP_TS_H

#include "stm32f429i_discovery_lcd.h"
#include "stm32f429i_discovery_ts.h"
#include "app_lcd.h"
#include "os.h"

extern OS_SEM TS_Semaphore;

void APP_TS_Get_Cell();
void APP_TS_INT_Enable();
CPU_INT08U APP_TS_Init();

#endif