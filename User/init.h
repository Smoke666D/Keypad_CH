#ifndef __INIT_H
#define __INIT_H


#include "FreeRTOS.h"
#include "task.h"
#include "flash_data.h"
//#include "cmsis_os.h"
#include "main.h"


#define PWM_TIM_PERIOD   1000



void vInitKeybord();
void vInit_DeviceConfig( void  );
void DMA_Tx_Init(DMA_Channel_TypeDef *DMA_CHx, u32 ppadr, u32 memadr, u16 bufsize);
#endif
