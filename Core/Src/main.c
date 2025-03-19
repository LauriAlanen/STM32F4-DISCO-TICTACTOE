#include "main.h"
#include <stdio.h>

#define TASK_STK_SIZE 256

#define START_TASK_PRIORITY 15
static OS_TCB App_TaskStartTCB;
static CPU_STK App_TaskStartStk[TASK_STK_SIZE];
static void App_TaskStart(void *p_arg);

#define CROSS_TASK_PRIORITY 13
static OS_TCB App_TaskCrossTCB;
static CPU_STK App_TaskCrossStk[TASK_STK_SIZE];
static void App_TaskCross(void *p_arg);

#define CIRCLE_TASK_PRIORITY 12
static OS_TCB App_TaskCircleTCB;
static CPU_STK App_TaskCircleStk[TASK_STK_SIZE];
static void App_TaskCircle(void *p_arg);

OS_FLAG_GRP GameFlags;

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

    OSFlagCreate(&GameFlags,
                (CPU_CHAR *)"Game Flags",
                (OS_FLAGS)0,
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
   
    OSFlagPost(&GameFlags, FLAG_TURN_CIRCLES, OS_OPT_POST_FLAG_SET, &os_error);
    OSFlagPost(&GameFlags, FLAG_TURN_CROSSES, OS_OPT_POST_FLAG_CLR, &os_error);

    OSTaskCreate((OS_TCB *)&App_TaskCircleTCB,
                (CPU_CHAR *)"App Task Circle",
                (OS_TASK_PTR) App_TaskCircle,
                (void *) 0,
                (OS_PRIO) CIRCLE_TASK_PRIORITY,
                (CPU_STK *)&App_TaskCircleStk[0],
                (CPU_STK_SIZE)(TASK_STK_SIZE / 10u),
                (CPU_STK_SIZE) TASK_STK_SIZE,
                (OS_MSG_QTY) 0,
                (OS_TICK) 0,
                (void *) 0,
                (OS_OPT)(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                (OS_ERR *)&os_error);

    OSTaskCreate((OS_TCB *)&App_TaskCrossTCB,
                (CPU_CHAR *)"App Task Cross",
                (OS_TASK_PTR) App_TaskCross,
                (void *) 0,
                (OS_PRIO) CROSS_TASK_PRIORITY,
                (CPU_STK *)&App_TaskCrossStk[0],
                (CPU_STK_SIZE)(TASK_STK_SIZE / 10u),
                (CPU_STK_SIZE) TASK_STK_SIZE,
                (OS_MSG_QTY) 0,
                (OS_TICK) 0,
                (void *) 0,
                (OS_OPT)(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                (OS_ERR *)&os_error);

    debug_print("\n\r");

    while (DEF_ON)
    {
        OSTimeDlyHMSM(0u, 0u, 0u, 500u,
            OS_OPT_TIME_HMSM_STRICT,
            &os_error);
        BSP_LED_Toggle(LED3);
    }
}

static void App_TaskCircle(void *p_arg)
{
    OS_ERR os_error;
    TS_StateTypeDef* TS_state;  
    Cell touched_cell;
    
    p_arg = p_arg;

    while (DEF_ON)
    {
        debug_print("TaskCircle: Waiting for flag.\n\r");
        OSFlagPend(&GameFlags,
                    FLAG_TURN_CIRCLES,
                    0,
                    OS_OPT_PEND_FLAG_SET_ANY | OS_OPT_PEND_FLAG_CONSUME | OS_OPT_PEND_BLOCKING,
                    DEF_NULL,
                    &os_error);
        
        debug_print("TaskCircle: Waiting for queue\n\r");
        OSQFlush(&TSEventQ, &os_error);
        TS_state = (TS_StateTypeDef *)OSQPend((OS_Q *)&TSEventQ,
                    0,
                    OS_OPT_PEND_BLOCKING,
                    (OS_MSG_SIZE *)sizeof(TS_StateTypeDef),
                    DEF_NULL,                
                    &os_error);
        debug_print("TaskCircle: Got data!\n\r");

        APP_TS_Get_Cell(TS_state, &touched_cell);
        APP_Draw_Circle(touched_cell.column, touched_cell.row);
        OSMemPut(&TSMemPool, (void *)TS_state, &os_error);

        OSTimeDlyHMSM(0u, 0u, 0u, 200u,
                    OS_OPT_TIME_HMSM_STRICT,
                    &os_error);

        OSFlagPost(&GameFlags,
            FLAG_TURN_CROSSES,
            OS_OPT_POST_FLAG_SET,
            &os_error);

        debug_print("TaskCircle: Interrupts enabled!\n\r");
        __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_15);
        HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
        debug_print("TaskCircle: Exiting!\n\r");
    }
}
    
static void App_TaskCross(void *p_arg)
{
    OS_ERR os_error;
    TS_StateTypeDef* TS_state;
    Cell touched_cell;
    
    p_arg = p_arg;
    
    while (DEF_ON)
    {
        debug_print("TaskCross: Waiting for flag.\n\r");
        OSFlagPend(&GameFlags,
                    FLAG_TURN_CROSSES,
                    0,
                    OS_OPT_PEND_FLAG_SET_ANY | OS_OPT_PEND_FLAG_CONSUME | OS_OPT_PEND_BLOCKING,
                    DEF_NULL,
                    &os_error);

        debug_print("TaskCross: Waiting for queue\n\r");
        OSQFlush(&TSEventQ, &os_error);
        TS_state = (TS_StateTypeDef *)OSQPend((OS_Q *)&TSEventQ,
                    0,
                    OS_OPT_PEND_BLOCKING,
                    (OS_MSG_SIZE *)sizeof(TS_StateTypeDef),
                    DEF_NULL,                
                    &os_error);
        debug_print("TaskCross: Got data!\n\r");
        
        APP_TS_Get_Cell(TS_state, &touched_cell);
        APP_Draw_Cross(touched_cell.column, touched_cell.row);
        OSMemPut(&TSMemPool, (void *)TS_state, &os_error);

        OSTimeDlyHMSM(0u, 0u, 0u, 200u,
            OS_OPT_TIME_HMSM_STRICT,
            &os_error);

        OSFlagPost(&GameFlags,
            FLAG_TURN_CIRCLES,
            OS_OPT_POST_FLAG_SET,
            &os_error);

        debug_print("TaskCross : Interrupts enabled!\n\r");
        __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_15);
        HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
        debug_print("TaskCross: Exiting!\n\r");
    }
}