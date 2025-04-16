#include "main.h"
#include "app_lcd.h"
#include "app_ts.h"
#include "utils.h"
#include <stdio.h>

#define TASK_STK_SIZE           256u

/* Task Priorities */
#define START_TASK_PRIORITY     15u
#define CROSS_TASK_PRIORITY     13u
#define CIRCLE_TASK_PRIORITY    12u
#define GAME_STATE_TASK_PRIORITY 14u

/* Global RTOS objects */
OS_MUTEX GameStateMutex;
OS_FLAG_GRP GameFlags;
static uint8_t GameStateMatrix[BOARD_SIZE][BOARD_SIZE];

/* RTOS Task Control Blocks and Stacks */
static OS_TCB App_TaskStartTCB;
static CPU_STK App_TaskStartStk[TASK_STK_SIZE];

static OS_TCB App_TaskCrossTCB;
static CPU_STK App_TaskCrossStk[TASK_STK_SIZE];

static OS_TCB App_TaskCircleTCB;
static CPU_STK App_TaskCircleStk[TASK_STK_SIZE];

static OS_TCB App_TaskGameStateCheckerTCB;
static CPU_STK App_TaskGameStateCheckerStk[TASK_STK_SIZE];

/* Function Prototypes */
static void App_TaskStart(void *p_arg);
static void App_TaskCircle(void *p_arg);
static void App_TaskCross(void *p_arg);
static void App_TaskGameStateChecker(void *p_arg);

/* Helper function prototype for touch processing */
typedef CPU_INT08U (*DrawFuncPtr)(uint8_t column, uint8_t row);
static void ProcessTouchEvent(OS_FLAGS turnFlag, OS_FLAGS nextTurnFlag,
                                DrawFuncPtr drawFunc, uint8_t playerMark);

int main(void)
{
    OS_ERR os_error;
    uint8_t app_error = 0;

    /* HAL and Board Initialization */
    HAL_Init();
    SystemClock_Config();
    BSP_LED_Init(LED3);

    /* Initialize LCD and draw the game board */
    app_error = APP_LCD_Initialize();
    if (app_error)
    {
        return 1;
    }
    APP_Draw_Board();
    UART3_Init();

    /* Initialize the OS and create synchronization objects */
    OSInit(&os_error);

    OSSemCreate(&TS_Semaphore, "Touch screen semaphore", 0, &os_error);
    OSSemCreate(&LCD_Semaphore, "LCD semaphore", 1, &os_error);
    OSMutexCreate(&GameStateMutex, "Game State Mutex", &os_error);

    OSQCreate(&TSEventQ, "Touch Event Queue", TOUCH_QUEUE_SIZE, &os_error);
    OSMemCreate(&TSMemPool, "TS Buffer", &TSMemPoolBuffer, TOUCH_POOL_SIZE,
                (OS_MEM_SIZE)sizeof(TS_StateTypeDef), &os_error);

    OSFlagCreate(&GameFlags, (CPU_CHAR *)"Game Flags", (OS_FLAGS)0, &os_error);

    /* Create the start task */
    OSTaskCreate(&App_TaskStartTCB,
                 (CPU_CHAR *)"App Task Start",
                 App_TaskStart,
                 (void *)0,
                 START_TASK_PRIORITY,
                 App_TaskStartStk,
                 (CPU_STK_SIZE)(TASK_STK_SIZE / 10u),
                 TASK_STK_SIZE,
                 0u,
                 0u,
                 (void *)0,
                 (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 &os_error);

    OSStart(&os_error);

    /* Should never reach here */
    while (DEF_ON) { }
    return 0;
}

/*-----------------------------------------------------------*/
/*                  Application Task Start                   */
/*-----------------------------------------------------------*/
static void App_TaskStart(void *p_arg)
{   
    OS_ERR os_error;
    CPU_INT32U systick_freq;

    (void)p_arg;  /* Prevent compiler warning */

    /* Initialize the SysTick for the OS tick rate */
    systick_freq = HAL_RCC_GetHCLKFreq();
    OS_CPU_SysTickInit(systick_freq / OS_CFG_TICK_RATE_HZ);

    /* Set initial game flags. Start with circles turn */
    OSFlagPost(&GameFlags, FLAG_TURN_CIRCLES, OS_OPT_POST_FLAG_SET, &os_error);
    OSFlagPost(&GameFlags, FLAG_TURN_CROSSES, OS_OPT_POST_FLAG_CLR, &os_error);

    /* Create game tasks */
    OSTaskCreate(&App_TaskCircleTCB,
                 (CPU_CHAR *)"App Task Circle",
                 App_TaskCircle,
                 (void *)0,
                 CIRCLE_TASK_PRIORITY,
                 App_TaskCircleStk,
                 (CPU_STK_SIZE)(TASK_STK_SIZE / 10u),
                 TASK_STK_SIZE,
                 0u,
                 0u,
                 (void *)0,
                 (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 &os_error);

    OSTaskCreate(&App_TaskCrossTCB,
                 (CPU_CHAR *)"App Task Cross",
                 App_TaskCross,
                 (void *)0,
                 CROSS_TASK_PRIORITY,
                 App_TaskCrossStk,
                 (CPU_STK_SIZE)(TASK_STK_SIZE / 10u),
                 TASK_STK_SIZE,
                 0u,
                 0u,
                 (void *)0,
                 (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 &os_error);

    OSTaskCreate(&App_TaskGameStateCheckerTCB,
                 (CPU_CHAR *)"App Task Game State Checker",
                 App_TaskGameStateChecker,
                 (void *)0,
                 GAME_STATE_TASK_PRIORITY,
                 App_TaskGameStateCheckerStk,
                 (CPU_STK_SIZE)(TASK_STK_SIZE / 10u),
                 TASK_STK_SIZE,
                 0u,
                 0u,
                 (void *)0,
                 (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 &os_error);

    while (DEF_ON)
    {
        OSTimeDlyHMSM(0u, 0u, 1u, 0u, OS_OPT_TIME_HMSM_STRICT, &os_error);
        BSP_LED_Toggle(LED3);
    }
}

/*-----------------------------------------------------------*/
/*                Touch Event Helper Function                */
/*-----------------------------------------------------------*/
/*
    ProcessTouchEvent:
    - Waits for the game flag indicating the turn for a particular player.
    - Blocks until a touch event occurs.
    - Gets the touch cell, checks if the move is valid, then draws the corresponding mark.
    - Posts the turn flag for the other player depending on success.
*/
static void ProcessTouchEvent(OS_FLAGS turnFlag, OS_FLAGS nextTurnFlag,
                                DrawFuncPtr drawFunc, uint8_t playerMark)
{
    OS_ERR error;
    TS_StateTypeDef *TS_state = NULL;
    Cell touched_cell;
    CPU_INT08U game_error = 0;

    OSFlagPend(&GameFlags,
               turnFlag,
               0,
               OS_OPT_PEND_FLAG_SET_ANY | OS_OPT_PEND_FLAG_CONSUME | OS_OPT_PEND_BLOCKING,
               DEF_NULL,
               &error);

    /* Block until a touch event is available */
    TS_state = (TS_StateTypeDef *)OSQPend((OS_Q *)&TSEventQ,
                                          0,
                                          OS_OPT_PEND_BLOCKING,
                                          (OS_MSG_SIZE *)sizeof(TS_StateTypeDef),
                                          DEF_NULL,
                                          &error);
    if (error != OS_ERR_NONE || TS_state == NULL)
    {
        __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_15);
        HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
        return;
    }

    /* Convert the touch event to a grid cell */
    game_error += APP_TS_Get_Cell(TS_state, &touched_cell);
    if (!game_error)
    {
        OSMutexPend(&GameStateMutex, 0, OS_OPT_PEND_BLOCKING, DEF_NULL, &error);
        if (GameStateMatrix[touched_cell.column][touched_cell.row] == FLAG_EMPTY)
        {
            game_error += drawFunc(touched_cell.column, touched_cell.row);
            GameStateMatrix[touched_cell.column][touched_cell.row] = playerMark;
        }
        else
        {
            game_error++;
        }
        OSMutexPost(&GameStateMutex, OS_OPT_POST_NONE, &error);
    }

    /* Return the touch state buffer to the memory pool */
    OSMemPut(&TSMemPool, (void *)TS_state, &error);

    OSTimeDlyHMSM(0u, 0u, 0u, 50u, OS_OPT_TIME_HMSM_STRICT, &error);

    /* Post the flag for the next player's turn, repeat current turn if error occurred */
    OSFlagPost(&GameFlags,
               (game_error) ? turnFlag : nextTurnFlag,
               OS_OPT_POST_FLAG_SET,
               &error);

    /* Clear external interrupt flags and re-enable interrupts */
    __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_15);
    HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
}

/*-----------------------------------------------------------*/
/*                    Circle and Cross Tasks                 */
/*-----------------------------------------------------------*/
static void App_TaskCircle(void *p_arg)
{
    (void)p_arg;

    while (DEF_ON)
    {
        ProcessTouchEvent(FLAG_TURN_CIRCLES, FLAG_TURN_CROSSES, APP_Draw_Circle, FLAG_CIRCLE);
    }
}

static void App_TaskCross(void *p_arg)
{
    (void)p_arg;

    while (DEF_ON)
    {
        ProcessTouchEvent(FLAG_TURN_CROSSES, FLAG_TURN_CIRCLES, APP_Draw_Cross, FLAG_CROSS);
    }
}

/*-----------------------------------------------------------*/
/*                 Game State Checker Task                   */
/*-----------------------------------------------------------*/
void App_TaskGameStateChecker(void *p_arg)
{
    OS_ERR error;
    CPU_INT08U state = 0;

    (void)p_arg;

    while (DEF_ON)
    {
        /* Delay between game state checks */
        OSTimeDlyHMSM(0u, 0u, 0u, 50u, OS_OPT_TIME_HMSM_STRICT, &error);

        OSMutexPend(&GameStateMutex, 0, OS_OPT_PEND_BLOCKING, DEF_NULL, &error);
        state = checkGameState(GameStateMatrix);
        OSMutexPost(&GameStateMutex, OS_OPT_POST_NONE, &error);

        /* Report game status */
        if (state == 1)
        {
            debug_print("Circle wins!\n");
        }
        else if (state == 2)
        {
            debug_print("Cross wins!\n");
        }
        else
        {
            debug_print("No win yet.\n");
        }
    }
}
