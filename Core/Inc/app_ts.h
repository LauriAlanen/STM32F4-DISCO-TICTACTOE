#ifndef APP_TS_H
#define APP_TS_H

#include "stm32f429i_discovery_lcd.h"
#include "stm32f429i_discovery_ts.h"
#include "app_lcd.h"
#include "os.h"
#include "utils.h"

#define TOUCH_QUEUE_SIZE 10
#define TOUCH_POOL_SIZE 10

extern OS_SEM TS_Semaphore;
extern OS_Q TSEventQ;

extern OS_MEM TSMemPool;
extern TS_StateTypeDef TSMemPoolBuffer[TOUCH_POOL_SIZE];

void APP_TS_Get_Cell(TS_StateTypeDef* TS_state);
void APP_TS_INT_Enable();
CPU_INT08U APP_TS_Init();

#endif