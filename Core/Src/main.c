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

#if DEBUG == 1
#define PRINT_TASK_PRIORITY 17
static OS_TCB App_TaskPrintTCB;
static CPU_STK App_TaskPrintStk[TASK_STK_SIZE];
static void PrintGameStateTask(void *p_arg);
#endif

OS_MUTEX GameStateMutex;
static uint8_t GameStateMatrix[BOARD_SIZE][BOARD_SIZE];

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
    OSMutexCreate(&GameStateMutex, "Game State Mutex", &os_error);

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
#if DEBUG == 1
    OSTaskCreate((OS_TCB *)&App_TaskPrintTCB,
                (CPU_CHAR *)"App Task Print",
                (OS_TASK_PTR) PrintGameStateTask,
                (void *) 0,
                (OS_PRIO) CROSS_TASK_PRIORITY,
                (CPU_STK *)&App_TaskPrintStk[0],
                (CPU_STK_SIZE)(TASK_STK_SIZE / 10u),
                (CPU_STK_SIZE) TASK_STK_SIZE,
                (OS_MSG_QTY) 0,
                (OS_TICK) 0,
                (void *) 0,
                (OS_OPT)(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                (OS_ERR *)&os_error);
#endif
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
    OS_ERR error;
    TS_StateTypeDef *TS_state = NULL;
    Cell touched_cell;
    CPU_INT08U game_error = 0;

    (void)p_arg;

    while (DEF_ON)
    {
        OSFlagPend(&GameFlags,
                   FLAG_TURN_CIRCLES,
                   0,
                   OS_OPT_PEND_FLAG_SET_ANY | OS_OPT_PEND_FLAG_CONSUME | OS_OPT_PEND_BLOCKING,
                   DEF_NULL,
                   &error);

        TS_state = (TS_StateTypeDef *)OSQPend((OS_Q *)&TSEventQ,
                                              0,
                                              OS_OPT_PEND_BLOCKING,
                                              (OS_MSG_SIZE *)sizeof(TS_StateTypeDef),
                                              DEF_NULL,
                                              &error);
        if (error != OS_ERR_NONE || TS_state == NULL)
        {
            debug_print("TaskCircle: OSQPend error or null state!\n\r");
            __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_15);
            HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
        }

        game_error += APP_TS_Get_Cell(TS_state, &touched_cell);
        if (!game_error)
        {
            OSMutexPend(&GameStateMutex, 0, OS_OPT_PEND_BLOCKING, DEF_NULL, &error);

            if (GameStateMatrix[touched_cell.column][touched_cell.row] ==  EMPTY)
            {
                game_error += APP_Draw_Circle(touched_cell.column, touched_cell.row);
                GameStateMatrix[touched_cell.column][touched_cell.row] = CIRCLE;
            }

            else
            {
                debug_print("Circle: Cell not empty! \n\r");
                game_error++;
            }

            OSMutexPost(&GameStateMutex, OS_OPT_POST_NONE, &error);
        }

        OSMemPut(&TSMemPool, (void *)TS_state, &error);
        OSTimeDlyHMSM(0u, 0u, 0u, 100u,
                      OS_OPT_TIME_HMSM_STRICT,
                      &error);

        if (game_error)
        {
            debug_print("TaskCircle: Retrying draw!\n\r");
            OSFlagPost(&GameFlags,
                       FLAG_TURN_CIRCLES,
                       OS_OPT_POST_FLAG_SET,
                       &error);
        }
        else
        {
            OSFlagPost(&GameFlags,
                       FLAG_TURN_CROSSES,
                       OS_OPT_POST_FLAG_SET,
                       &error);
        }

        game_error = 0;

        __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_15);
        HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
    }
}

static void App_TaskCross(void *p_arg)
{
    OS_ERR error;
    TS_StateTypeDef *TS_state = NULL;
    Cell touched_cell;
    CPU_INT08U game_error = 0;

    (void)p_arg;

    while (DEF_ON)
    {
        OSFlagPend(&GameFlags,
                   FLAG_TURN_CROSSES,
                   0,
                   OS_OPT_PEND_FLAG_SET_ANY | OS_OPT_PEND_FLAG_CONSUME | OS_OPT_PEND_BLOCKING,
                   DEF_NULL,
                   &error);

        TS_state = (TS_StateTypeDef *)OSQPend((OS_Q *)&TSEventQ,
                                              0,
                                              OS_OPT_PEND_BLOCKING,
                                              (OS_MSG_SIZE *)sizeof(TS_StateTypeDef),
                                              DEF_NULL,
                                              &error);
        if (error != OS_ERR_NONE || TS_state == NULL)
        {
            debug_print("TaskCross: OSQPend error or null state!\n\r");
            __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_15);
            HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
        }

        game_error += APP_TS_Get_Cell(TS_state, &touched_cell);
        if (!game_error)
        {
            OSMutexPend(&GameStateMutex, 0, OS_OPT_PEND_BLOCKING, DEF_NULL, &error);

            if (GameStateMatrix[touched_cell.column][touched_cell.row] ==  EMPTY)
            {
                game_error += APP_Draw_Cross(touched_cell.column, touched_cell.row);
                GameStateMatrix[touched_cell.column][touched_cell.row] = CROSS;
            }

            else
            {
                debug_print("Cross: Cell not empty! \n\r");
                game_error++;
            }
            
            OSMutexPost(&GameStateMutex, OS_OPT_POST_NONE, &error);
        }

        OSMemPut(&TSMemPool, (void *)TS_state, &error);
        OSTimeDlyHMSM(0u, 0u, 0u, 100u,
                      OS_OPT_TIME_HMSM_STRICT,
                      &error);

        if (game_error)
        {
            debug_print("TaskCross: Retrying draw!\n\r");
            OSFlagPost(&GameFlags,
                       FLAG_TURN_CROSSES,
                       OS_OPT_POST_FLAG_SET,
                       &error);
        }
        else
        {
            OSFlagPost(&GameFlags,
                       FLAG_TURN_CIRCLES,
                       OS_OPT_POST_FLAG_SET,
                       &error);
        }

        game_error = 0;

        __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_15);
        HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
    }
}

#if DEBUG == 1
void PrintGameStateTask(void *p_arg) {
    (void)p_arg;
    OS_ERR err;
    char debug_buffer[100];

    while (1) 
    {
        OSMutexPend(&GameStateMutex, 
                    0,
                    OS_OPT_PEND_BLOCKING, 
                    NULL,
                    &err);
        if (err == OS_ERR_NONE) 
        {
            for (int i = 0; i < BOARD_SIZE; i++) 
            {
                for (int j = 0; j < BOARD_SIZE; j++) 
                {
                    snprintf(debug_buffer, 100, "%3d ", GameStateMatrix[i][j]);
                    debug_print(debug_buffer);
                }
                debug_print("\n");
            }
            debug_print("\n");

            OSMutexPost(&GameStateMutex, OS_OPT_POST_NONE, &err);
        }

        OSTimeDlyHMSM(0, 0, 2, 0, OS_OPT_TIME_HMSM_STRICT, &err);
    }
}
#endif