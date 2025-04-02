#include "utils.h"
#include <string.h>

UART_HandleTypeDef huart1;

void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
    RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

    /** Configure the main internal regulator output voltage
     */
    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
    /** Initializes the CPU, AHB and APB busses clocks
     */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
    RCC_OscInitStruct.PLL.PLLM = 8;
    RCC_OscInitStruct.PLL.PLLN = 180;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
    RCC_OscInitStruct.PLL.PLLQ = 7;
    HAL_RCC_OscConfig(&RCC_OscInitStruct);

    /** Activate the Over-Drive mode
     */
    HAL_PWREx_EnableOverDrive();

    /** Initializes the CPU, AHB and APB busses clocks
     */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

    HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5);

    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_LTDC;
    PeriphClkInitStruct.PLLSAI.PLLSAIN = 216;
    PeriphClkInitStruct.PLLSAI.PLLSAIR = 2;
    PeriphClkInitStruct.PLLSAIDivR = RCC_PLLSAIDIVR_2;
    HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct);
}

void UART3_Init()
{
    __HAL_RCC_USART1_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();

    GPIO_InitTypeDef GPIO_InitStruct = {0};

    GPIO_InitStruct.Pin = GPIO_PIN_9 | GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART3;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    // Configure UART3
    huart1.Instance = USART1;
    huart1.Init.BaudRate = 9600;
    huart1.Init.WordLength = UART_WORDLENGTH_8B;
    huart1.Init.StopBits = UART_STOPBITS_1;
    huart1.Init.Parity = UART_PARITY_NONE;
    huart1.Init.Mode = UART_MODE_TX_RX;
    huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart1.Init.OverSampling = UART_OVERSAMPLING_16;
    
    if (HAL_UART_Init(&huart1) != HAL_OK)
    {
        // Handle error
        while (1);
    }
}

CPU_INT08U checkGameState(CPU_INT08U GameStateMatrix[BOARD_SIZE][BOARD_SIZE])
{
    CPU_INT08U winner = 0;

    for (int i = 0; i < BOARD_SIZE; i++)
    {
        CPU_INT08U first = GameStateMatrix[i][0];
        if (first != 0)
        {
            CPU_INT08U rowWin = 1;
            for (int j = 1; j < BOARD_SIZE; j++)
            {
                if (GameStateMatrix[i][j] != first)
                {
                    rowWin = 0;
                    break;
                }
            }
            if (rowWin)
            {
                winner = first;
                break;
            }
        }
    }

    // Check columns for a win if no winner yet
    if (winner == 0)
    {
        for (int j = 0; j < BOARD_SIZE; j++)
        {
            CPU_INT08U first = GameStateMatrix[0][j];
            if (first != 0)
            {
                CPU_INT08U colWin = 1;
                for (int i = 1; i < BOARD_SIZE; i++)
                {
                    if (GameStateMatrix[i][j] != first)
                    {
                        colWin = 0;
                        break;
                    }
                }
                if (colWin)
                {
                    winner = first;
                    break;
                }
            }
        }
    }

    // Check main diagonal for a win if still no winner
    if (winner == 0)
    {
        CPU_INT08U first = GameStateMatrix[0][0];
        if (first != 0)
        {
            CPU_INT08U diagWin = 1;
            for (int i = 1; i < BOARD_SIZE; i++)
            {
                if (GameStateMatrix[i][i] != first)
                {
                    diagWin = 0;
                    break;
                }
            }
            if (diagWin)
            {
                winner = first;
            }
        }
    }

    // Check anti-diagonal for a win if still no winner
    if (winner == 0)
    {
        CPU_INT08U first = GameStateMatrix[0][BOARD_SIZE - 1];
        if (first != 0)
        {
            CPU_INT08U antiDiagWin = 1;
            for (int i = 1; i < BOARD_SIZE; i++)
            {
                if (GameStateMatrix[i][BOARD_SIZE - i - 1] != first)
                {
                    antiDiagWin = 0;
                    break;
                }
            }
            if (antiDiagWin)
            {
                winner = first;
            }
        }
    }
    
    return winner;
}

void debug_print(char *msg)
{
    #if DEBUG == 1
    HAL_UART_Transmit(&huart1, (CPU_INT08U *)msg, strlen(msg), HAL_MAX_DELAY);
    #endif
}

void HAL_Delay(uint32_t delay)
{
    OS_ERR err;

    OSTimeDly((OS_TICK)delay,
              (OS_OPT)OS_OPT_TIME_DLY,
              (OS_ERR *)&err);
}