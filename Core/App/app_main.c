#include "os.h"
#include "main.h"
#include "stm32f429i_discovery.h"
#include "stm32f429i_discovery_lcd.h"

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

int main()
{
    OS_ERR err;

    Device_Init();
    BSP_LCD_Init();

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
        HAL_GPIO_TogglePin(LD3_GPIO_Port, LD3_Pin);
        OSTimeDlyHMSM(0u, 0u, 0u, 500u,
            OS_OPT_TIME_HMSM_STRICT,
            &err);
    }
}

static void App_TaskBlink2(void *p_arg)
{
    OS_ERR err;

    p_arg = p_arg;

    while (DEF_ON)
    {
        HAL_GPIO_TogglePin(LD4_GPIO_Port, LD4_Pin);
        OSTimeDlyHMSM(0u, 0u, 0u, 500u,
            OS_OPT_TIME_HMSM_STRICT,
            &err);
    }
}