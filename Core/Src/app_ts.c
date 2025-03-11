#include "app_ts.h"

uint8_t APP_TS_Init(void)
{
    uint8_t error;

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

// This function should be made to provide a interrupt to the kernel
// For some odd reason the TS coordinates are not the same as screen coordinates
// In TS top left corner is (0, 320)
void APP_TS_Get_Cell(void *p_arg)
{  
    TS_StateTypeDef TS_state;
    uint8_t column = 0, row = 0;

    p_arg = p_arg;

    BSP_TS_GetState(&TS_state);
    if (TS_state.TouchDetected && (TS_state.X < x_size && TS_state.Y < y_size))
    {
        column = TS_state.X / x_spacing;
        row = (y_size - TS_state.Y) / y_spacing;

        APP_Draw_Circle(row, column);
        
        BSP_LED_Toggle(LED3);
    }
}

void EXTI15_10_IRQHandler(void)
{
    if (BSP_TS_ITGetStatus() == 1)
    {
        BSP_LED_Toggle(LED4);
    }
    
    BSP_TS_ITClear();
}
