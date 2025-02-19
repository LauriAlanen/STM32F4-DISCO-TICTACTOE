#include "os.h"
#include "main.h"

int app_main()
{
    HAL_GPIO_TogglePin(LD3_GPIO_Port, LD3_Pin);
    HAL_Delay(100);

    return 0;
}
