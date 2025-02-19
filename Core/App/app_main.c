#include "os.h"
#include "main.h"

#define TASK_STK_SIZE 4096

#define START_TASK_PRIORITY 25
static OS_TCB App_TaskStartTCB;
static CPU_STK_SIZE App_TaskStartStk[TASK_STK_SIZE];

static void App_TaskStart(void *p_arg);

int app_main()
{
    OS_ERR err;
    OSInit(&err);

    OSTaskCreate((OS_TCB     *)&App_TaskStartTCB,
    (CPU_CHAR   *)"App Task Start",
    (OS_TASK_PTR ) App_TaskStart,
    (void       *) 0,
    (OS_PRIO     ) START_TASK_PRIORITY,
    (CPU_STK    *)&App_TaskStartStk[0],
    (CPU_STK     )(TASK_STK_SIZE / 10u),
    (CPU_STK_SIZE) TASK_STK_SIZE,
    (OS_MSG_QTY  ) 0,
    (OS_TICK     ) 0,
    (void       *) 0,
    (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
    (OS_ERR     *)&err);

    OSStart(&err);

    while (DEF_ON){}
    
    return 0;
}

static void App_TaskStart(void *p_arg)
{
    OS_ERR err;

    CPU_Init();
    OS_CPU_SysTickInit(0);

    while (DEF_ON)
    {
        HAL_GPIO_TogglePin(LD3_GPIO_Port, LD3_Pin);
        OSTimeDlyHMSM(0u, 0u, 1u, 0u,
            OS_OPT_TIME_HMSM_STRICT,
            &err);
    }
}