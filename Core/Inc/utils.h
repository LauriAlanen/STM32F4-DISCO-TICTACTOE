#ifndef UTILS_H
#define UTILS_H

#include "stm32f429i_discovery.h"
#include "os.h"

extern UART_HandleTypeDef huart1;

void SystemClock_Config(void);
void UART3_Init(void);
void debug_print(char *msg);
extern void HAL_Delay(uint32_t delay); // Overwrite the weak delay provided by HAL
CPU_INT08U verifyMove(CPU_INT08U **GameStateMatrix, CPU_INT08U column, CPU_INT08U row);

#endif