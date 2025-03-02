#include "main.h"
#include "os.h"

#define TASK_STK_SIZE 256

#define START_TASK_PRIORITY 10
static OS_TCB App_TaskStartTCB;
static CPU_STK App_TaskStartStk[TASK_STK_SIZE];

#define BLINK_TASK_PRIORITY_1 11
static OS_TCB App_TaskBlink1TCB;
static CPU_STK App_TaskBlink1Stk[TASK_STK_SIZE];

#define BLINK_TASK_PRIORITY_2 12
static OS_TCB App_TaskBlink2TCB;
static CPU_STK App_TaskBlink2Stk[TASK_STK_SIZE];

static void App_TaskStart(void *p_arg);
static void App_TaskBlink1(void *p_arg);
static void App_TaskBlink2(void *p_arg);

static void SystemClock_Config(void);

int main()
{
    OS_ERR err;
    
    HAL_Init();
    SystemClock_Config();
    BSP_LED_Init(LED3);
    BSP_LED_Init(LED4);
    BSP_LED_On(LED3);
    BSP_LCD_Init();
    BSP_LCD_LayerDefaultInit(LCD_BACKGROUND_LAYER, LCD_FRAME_BUFFER);
    BSP_LCD_LayerDefaultInit(LCD_FOREGROUND_LAYER, LCD_FRAME_BUFFER);
    BSP_LCD_SelectLayer(LCD_FOREGROUND_LAYER);
    BSP_LCD_DisplayOn();
    BSP_LCD_Clear(LCD_COLOR_WHITE);
    BSP_LCD_SetTextColor(LCD_COLOR_BLACK);

    OSInit(&err);

    OSTaskCreate((OS_TCB *)&App_TaskStartTCB,
                (CPU_CHAR *)"App Task Start",
                (OS_TASK_PTR) App_TaskStart,
                (void *) 0,
                (OS_PRIO) START_TASK_PRIORITY,
                (CPU_STK *)&App_TaskStartStk[0],
                (CPU_STK_SIZE)(TASK_STK_SIZE / 10u),
                (CPU_STK_SIZE) TASK_STK_SIZE,
                (OS_MSG_QTY) 0,
                (OS_TICK) 0,
                (void *) 0,
                (OS_OPT)(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                (OS_ERR *)&err);

    OSStart(&err);  
    
    while (DEF_ON){}
    
    return 0;
}   

static void App_TaskStart(void *p_arg)
{   
    OS_ERR err;

    p_arg = p_arg;

    uint32_t systick_freq = HAL_RCC_GetHCLKFreq();
    OS_CPU_SysTickInit(systick_freq / OS_CFG_TICK_RATE_HZ); 

    OSTaskCreate((OS_TCB *)&App_TaskBlink1TCB,
                (CPU_CHAR *)"App Task Blink 1",
                (OS_TASK_PTR) App_TaskBlink1,
                (void *) 0,
                (OS_PRIO) BLINK_TASK_PRIORITY_1,
                (CPU_STK *)&App_TaskBlink1Stk[0],
                (CPU_STK_SIZE)(TASK_STK_SIZE / 10u),
                (CPU_STK_SIZE) TASK_STK_SIZE,
                (OS_MSG_QTY) 0,
                (OS_TICK) 0,
                (void *) 0,
                (OS_OPT)(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                (OS_ERR *)&err);
    
    OSTaskCreate((OS_TCB *)&App_TaskBlink2TCB,
                (CPU_CHAR *)"App Task Blink 2",
                (OS_TASK_PTR) App_TaskBlink2,
                (void *) 0,
                (OS_PRIO) BLINK_TASK_PRIORITY_2,
                (CPU_STK *)&App_TaskBlink2Stk[0],
                (CPU_STK_SIZE)(TASK_STK_SIZE / 10u),
                (CPU_STK_SIZE) TASK_STK_SIZE,
                (OS_MSG_QTY) 0,
                (OS_TICK) 0,
                (void *) 0,
                (OS_OPT)(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                (OS_ERR *)&err);

    while (DEF_ON)
    {
        OSTimeDlyHMSM(0u, 0u, 0u, 500u,
            OS_OPT_TIME_HMSM_STRICT,
            &err);
    }
}

static void App_TaskBlink1(void *p_arg)
{
    OS_ERR err;

    p_arg = p_arg;

    while (DEF_ON)
    {
        HAL_GPIO_TogglePin(LED3_GPIO_PORT, LED3_PIN);
        OSTimeDlyHMSM(0u, 0u, 1u, 0u,
            OS_OPT_TIME_HMSM_STRICT,
            &err);
    }
}

static void App_TaskBlink2(void *p_arg)
{
    OS_ERR err;

    p_arg = p_arg;

    BSP_LCD_DrawCircle(19, 50, 50);

    while (DEF_ON)
    {
        HAL_GPIO_TogglePin(LED4_GPIO_PORT, LED4_PIN);
        OSTimeDlyHMSM(0u, 0u, 1u, 0u,
            OS_OPT_TIME_HMSM_STRICT,
            &err);
    }
}

static void SystemClock_Config(void)
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

void HAL_Delay(uint32_t delay)
{
    OS_ERR err;

    OSTimeDly((OS_TICK)delay,
              (OS_OPT)OS_OPT_TIME_DLY,
              (OS_ERR *)&err);
}