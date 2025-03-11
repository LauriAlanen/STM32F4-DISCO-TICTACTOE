#include "app_ts.h"

OS_SEM TS_Semaphore;
OS_SEM Draw_Semaphore;

CPU_INT08U APP_TS_Init(void)
{
    CPU_INT08U error;

    error = BSP_TS_Init(BSP_LCD_GetXSize(), BSP_LCD_GetYSize());
    if (error)
    {
        return 1;
    }

    APP_TS_INT_Enable();

    return 0;
}

void APP_TS_INT_Enable()
{
    BSP_LED_Init(LED4);
    BSP_TS_ITConfig();  

    GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_RCC_GPIOA_CLK_ENABLE();

    GPIO_InitStruct.Pin = GPIO_PIN_15;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    HAL_NVIC_SetPriority(EXTI15_10_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
}

// In TS top left corner is (0, 320)
// This function is reentrant since global variables are only read
void APP_TS_Get_Cell()
{  
    TS_StateTypeDef TS_state;
    CPU_INT08U column = 0, row = 0;

    BSP_TS_GetState(&TS_state);
    if (TS_state.TouchDetected && (TS_state.X < x_size && TS_state.Y < y_size))
    {
        BSP_LED_Toggle(LED3);
        column = TS_state.X / x_spacing;
        row = (y_size - TS_state.Y) / y_spacing;
        APP_Draw_Circle(row, column);
    }
}

void EXTI15_10_IRQHandler(void)
{
    OS_ERR os_error;

    OSIntEnter();

    if (__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_15) != RESET)
    {
        __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_15);

        if (BSP_TS_ITGetStatus() == 1)
        {
            OSSemPost(&TS_Semaphore, OS_OPT_POST_1, &os_error);
            BSP_LED_Toggle(LED4);
        }
    }

    BSP_TS_ITClear();
    OSIntExit();
}
