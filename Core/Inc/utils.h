#ifndef UTILS_H
#define UTILS_H

#include "stm32f429i_discovery.h"
#include "os.h"

void SystemClock_Config(void);
extern void HAL_Delay(uint32_t delay); // Overwrite the weak delay provided by HAL

#endif