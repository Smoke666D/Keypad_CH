/*
 * system_init.h
 *
 *  Created on: May 13, 2024
 *      Author: i.dymov
 */

#ifndef USER_SYSTEM_INIT_H_
#define USER_SYSTEM_INIT_H_

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "list.h"
#include "event_groups.h"

#define __SECTION(x)


#define  DEFAULT_TASK_STACK_SIZE        128U
#define  CAN_OPEN_TASK_PRIO             7
#define  CAN_OPEN_STK_SIZE              700
#define PERIODIC_CAN_TASK_PRIO          5
#define PERIODIC_CAN_STK_SIZE           256
#define PROCESS_TASK_PRIO               10  //§¥§°§­§¨§¦§¯ §¢§½§´§¾ §³§¡§®§½ §£§½§³§°§¬§ª§«
#define PROCESS_STK_SIZE                256
#define KEYBOARD_TASK_PRIO              5
#define KEYBOARD_STK_SIZE               128

#define TASK_RUN_NOTIFY    0x01
#define TASK_STOP_NOTIFY   0x02
#define TASK_INIT_NOTIFY   0x04
#define INPUTS_DRIVER_READY   0x08
#define AIN_DRIVER_READY   0x10
#define ADC2_READY         0x20
#define ADC3_READY         0x40
#define ADC1_READY         0x80



typedef enum
{
    STATE_INIT,
    STATE_IDLE,
    STATE_RUN,
    STATE_WHAIT_TO_RAEDY,
} TaskFSM_t;

void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer,
                                    StackType_t **ppxIdleTaskStackBuffer,
                                    uint32_t *pulIdleTaskStackSize );

void vApplicationGetTimerTaskMemory( StaticTask_t **ppxTimerTaskTCBBuffer,
                                     StackType_t **ppxTimerTaskStackBuffer,
                                     uint32_t *pulTimerTaskStackSize );

void vSYStaskInit ( void );
void vSYSqueueInit ( void );
void vSYSeventInit ( void );

#endif /* USER_SYSTEM_INIT_H_ */
