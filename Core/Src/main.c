#include "main.h"
#include <stdio.h>

#define TASK_STK_SIZE 256

#define START_TASK_PRIORITY 12
static OS_TCB App_TaskStartTCB;
static CPU_STK App_TaskStartStk[TASK_STK_SIZE];
static void App_TaskStart(void *p_arg);

#define TASK_GET_TOUCH_PRIORITY 11
static OS_TCB App_TaskGetTouchTCB;
static CPU_STK App_TaskGetTouchStk[TASK_STK_SIZE];
static void App_TaskGetTouch(void *p_arg);

int main()
{
    OS_ERR os_error;
    uint8_t app_error = 0;

    HAL_Init();
    SystemClock_Config();

    BSP_LED_Init(LED3);

    app_error = APP_LCD_Initialize();
    if (app_error)
    {
        return 1;
    }

    APP_Draw_Board();
    UART3_Init();

    OSInit(&os_error);
    
    OSSemCreate(&TS_Semaphore, "Touch screen semaphore", 0, &os_error);
    OSSemCreate(&LCD_Semaphore, "LCD semaphore", 1, &os_error);

    OSQCreate(&TSEventQ,
        "Touch Event Queue",
        TOUCH_QUEUE_SIZE,
        &os_error);

    OSMemCreate(&TSMemPool,
            "TS Buffer",
            &TSMemPoolBuffer,
            TOUCH_POOL_SIZE,
            (OS_MEM_SIZE)sizeof(TS_StateTypeDef),
            &os_error);

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
                (OS_ERR *)&os_error);

    OSStart(&os_error);  
    
    while (DEF_ON){}
    
    return 0;
}   

static void App_TaskStart(void *p_arg)
{   
    OS_ERR os_error;

    p_arg = p_arg;

    uint32_t systick_freq = HAL_RCC_GetHCLKFreq();
    OS_CPU_SysTickInit(systick_freq / OS_CFG_TICK_RATE_HZ); 

    OSTaskCreate((OS_TCB *)&App_TaskGetTouchTCB,
                (CPU_CHAR *)"App Task Get Touch",
                (OS_TASK_PTR) App_TaskGetTouch,
                (void *) 0,
                (OS_PRIO) TASK_GET_TOUCH_PRIORITY,
                (CPU_STK *)&App_TaskGetTouchStk[0],
                (CPU_STK_SIZE)(TASK_STK_SIZE / 10u),
                (CPU_STK_SIZE) TASK_STK_SIZE,
                (OS_MSG_QTY) 0,
                (OS_TICK) 0,
                (void *) 0,
                (OS_OPT)(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                (OS_ERR *)&os_error);

    while (DEF_ON)
    {
        OSTimeDlyHMSM(0u, 0u, 0u, 500u,
            OS_OPT_TIME_HMSM_STRICT,
            &os_error);
    }
}

static void App_TaskGetTouch(void *p_arg)
{
    OS_ERR os_error;
    TS_StateTypeDef* TS_state;
    char debug_buffer[20];
    p_arg = p_arg;

    while (DEF_ON)
    {
        TS_state = (TS_StateTypeDef *)OSQPend((OS_Q *)&TSEventQ,
                    0,
                    OS_OPT_PEND_BLOCKING,
                    (OS_MSG_SIZE *)sizeof(TS_StateTypeDef),
                    DEF_NULL,                
                    &os_error);
        // snprintf(debug_buffer, 20, "O-%d:%d\r\n", TS_state->X, TS_state->Y);
        // debug_print(debug_buffer);

        APP_TS_Get_Cell(TS_state);
        OSMemPut(&TSMemPool, (void *)TS_state, &os_error);
        OSQFlush(&TSEventQ, &os_error);
        HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
    }
}
