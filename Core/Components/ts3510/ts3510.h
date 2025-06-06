/**
  ******************************************************************************
  * @file    ts3510.h
  * @author  MCD Application Team
  * @brief   This file contains all the functions prototypes for the
  *          ts3510.c IO expander driver.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2014 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */ 

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __TS3510_H
#define __TS3510_H

#ifdef __cplusplus
 extern "C" {
#endif   
   
/* Includes ------------------------------------------------------------------*/
#include "../Common/ts.h"

/** @addtogroup BSP
  * @{
  */ 

/** @addtogroup Component
  * @{
  */
    
/** @defgroup TS3510
  * @{
  */    

/* Exported types ------------------------------------------------------------*/

/** @defgroup TS3510_Exported_Types
  * @{
  */ 

/* Exported constants --------------------------------------------------------*/
  
/** @defgroup TS3510_Exported_Constants
  * @{
  */ 

/*  */   
#define TS3510_READ_BLOCK_REG                     0x8A
#define TS3510_SEND_CMD_REG                       0x00
#define TS3510_READ_CMD                           0x81  
#define TS3510_WRITE_CMD                          0x08     


/**
  * @}
  */ 
  
/* Exported macro ------------------------------------------------------------*/
   
/** @defgroup ts3510_Exported_Macros
  * @{
  */ 

/* Exported functions --------------------------------------------------------*/
  
/** @defgroup ts3510_Exported_Functions
  * @{
  */

/** 
  * @brief ts3510 Control functions
  */
void     ts3510_Init(uint16_t DeviceAddr);
void     ts3510_Reset(uint16_t DeviceAddr);
uint16_t ts3510_ReadID(uint16_t DeviceAddr);
void     ts3510_TS_Start(uint16_t DeviceAddr);
uint8_t  ts3510_TS_DetectTouch(uint16_t DeviceAddr);
void     ts3510_TS_GetXY(uint16_t DeviceAddr, uint16_t *X, uint16_t *Y);
void     ts3510_TS_EnableIT(uint16_t DeviceAddr);
void     ts3510_TS_DisableIT(uint16_t DeviceAddr);
uint8_t  ts3510_TS_ITStatus (uint16_t DeviceAddr);
void     ts3510_TS_ClearIT (uint16_t DeviceAddr);

void     IOE_Init(void);
void     IOE_Delay(uint32_t delay);
uint8_t  IOE_Read(uint8_t addr, uint8_t reg);
uint16_t IOE_ReadMultiple(uint8_t addr, uint8_t reg, uint8_t *buffer, uint16_t length);
void     IOE_WriteMultiple(uint8_t addr, uint8_t reg, uint8_t *buffer, uint16_t length);

/* Touch screen driver structure */
extern TS_DrvTypeDef ts3510_ts_drv;

#ifdef __cplusplus
}
#endif
#endif /* __TS3510_H */


/**
  * @}
  */ 

/**
  * @}
  */

/**
  * @}
  */ 

/**
  * @}
  */
