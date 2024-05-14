/*
 * main.h
 *
 *  Created on: Feb 16, 2024
 *      Author: i.dymov
 */

#ifndef USER_MAIN_H_
#define USER_MAIN_H_

#include "debug.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "CO_driver_ST32F103.h"
#include "flash_data.h"
#include "led.h"
#include "stdlib.h"
#include "init.h"




#define RESTART_DISABLE  0x0001

void vRestartNode( void );
void Error_Handler(void);


#endif /* USER_MAIN_H_ */
