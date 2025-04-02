#ifndef MAIN_H
#define MAIN_H

#include "stm32f429i_discovery.h"
#include "app_lcd.h"
#include "app_ts.h"
#include "utils.h"
#include "os.h"

extern OS_FLAG_GRP GameFlags;

#define FLAG_GAME_OVER 0x0
#define FLAG_TURN_CROSSES 0x1
#define FLAG_TURN_CIRCLES 0x2

#define EMPTY DEF_NULL
#define CIRCLE 0x01
#define CROSS 0x02

#endif