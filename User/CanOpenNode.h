/*
 * CanOpenNode.h
 *
 *  Created on: Nov 17, 2021
 *      Author: igor.dymov
 */

#ifndef INC_CANOPENNODE_H_
#define INC_CANOPENNODE_H_

#include "main.h"

TaskHandle_t * xCanOpenProcessTaskHandle ();
TaskHandle_t * xCanOpenPeriodicTaskHandle ();
void vCanOpenInit(CAN_TypeDef *hcan);
void vCanOpenProcess(void *argument);
void vCanOpenPeriodicProcess(void *argument);

#endif /* INC_CANOPENNODE_H_ */
