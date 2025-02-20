#include "os.h"
#include "main.h"

#define TASK_STK_SIZE 256

#define START_TASK_PRIORITY 10
static OS_TCB App_TaskStartTCB;
static CPU_STK App_TaskStartStk[TASK_STK_SIZE];

#define BLINK_TASK_PRIORITY 11
static OS_TCB App_TaskBlinkTCB;
static CPU_STK App_TaskBlinkStk[TASK_STK_SIZE];

static void App_TaskStart(void *p_arg);
static void App_TaskBlink(void *p_arg);

int app_main()
{
    OS_ERR err;
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

    OSTaskCreate((OS_TCB *)&App_TaskBlinkTCB,
                (CPU_CHAR *)"App Task Blink",
                (OS_TASK_PTR) App_TaskBlink,
                (void *) 0,
                (OS_PRIO) BLINK_TASK_PRIORITY,
                (CPU_STK *)&App_TaskBlinkStk[0],
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

static void App_TaskBlink(void *p_arg)
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