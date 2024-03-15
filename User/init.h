#ifndef __INIT_H
#define __INIT_H


#include "FreeRTOS.h"
#include "task.h"
#include "flash_data.h"
//#include "cmsis_os.h"


#define KL1_Pin GPIO_Pin_5
#define KL1_GPIO_Port GPIOA
#define KL2_Pin GPIO_Pin_1
#define KL2_GPIO_Port GPIOB
#define KL3_Pin GPIO_Pin_2
#define KL3_GPIO_Port GPIOB
#define CS_Pin GPIO_Pin_12
#define CS_GPIO_Port GPIOB
#define KL4_Pin GPIO_Pin_3
#define KL4_GPIO_Port GPIOB
#define KL5_Pin GPIO_Pin_4
#define KL5_GPIO_Port GPIOB
#define KL6_Pin GPIO_Pin_5
#define KL6_GPIO_Port GPIOB
#define KL7_Pin GPIO_Pin_6
#define KL7_GPIO_Port GPIOB
#define KL8_Pin GPIO_Pin_7
#define KL8_GPIO_Port GPIOB

#define PWM_TIM_PERIOD   1000




void vInitKeybord();
void vInit_DeviceConfig( void  );
void DMA_Tx_Init(DMA_Channel_TypeDef *DMA_CHx, u32 ppadr, u32 memadr, u16 bufsize);
#endif
