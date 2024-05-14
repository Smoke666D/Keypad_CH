/*
 * system_init.c
 *
 *  Created on: May 13, 2024
 *      Author: i.dymov
 */
#include "system_init.h"
#include "led.h"
#include "CanOpenNode.h"
#include "keyboard_task.h"
#include "process.h"
#include "hal_wdt.h"

static void StartDefaultTask(void *argument);
static StaticTask_t xIdleTaskTCB                                     __SECTION(RAM_SECTION_CCMRAM);
static StaticTask_t xTimerTaskTCB                                    __SECTION(RAM_SECTION_CCMRAM);
static StaticTask_t defaultTaskControlBlock;
static StaticTask_t CanOpneProccesTaskControlBlock;
static StaticTask_t CanOpnePeriodicTaskControlBlock;
static StaticTask_t KeyboardTaskControlBlock;
static StaticTask_t ProcessTaskControlBlock;

static StackType_t uxIdleTaskStack[ configMINIMAL_STACK_SIZE ]       __SECTION(RAM_SECTION_CCMRAM);
static StackType_t uxTimerTaskStack[ configTIMER_TASK_STACK_DEPTH ]  __SECTION(RAM_SECTION_CCMRAM);
static StackType_t defaultTaskBuffer[DEFAULT_TASK_STACK_SIZE];
static StackType_t CanOpneProccesTaskBuffer[CAN_OPEN_STK_SIZE ];
static StackType_t CanOpnePeriodicTaskBuffer[PERIODIC_CAN_STK_SIZE ];
static StackType_t KeyboardTaskBuffer[KEYBOARD_STK_SIZE ];
static StackType_t ProcessTaskBuffer[ PROCESS_STK_SIZE ];
static TaskHandle_t DefautTask_Handler;

uint8_t ucQueueStorageArea[  16U * sizeof( KeyEvent ) ];
static StaticQueue_t xStaticQueue;

static EventGroupHandle_t xResetEventHandle;
static StaticEventGroup_t xCreatedEventGroup;
/*
 * 妤快把快技快扶扶抑快
 */

void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer,
                                    StackType_t **ppxIdleTaskStackBuffer,
                                    uint32_t *pulIdleTaskStackSize )
{
   *ppxIdleTaskTCBBuffer = &xIdleTaskTCB;
   *ppxIdleTaskStackBuffer = uxIdleTaskStack;
   *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}


void vApplicationGetTimerTaskMemory( StaticTask_t **ppxTimerTaskTCBBuffer,
                                     StackType_t **ppxTimerTaskStackBuffer,
                                     uint32_t *pulTimerTaskStackSize )
{

   *ppxTimerTaskTCBBuffer = &xTimerTaskTCB;
   *ppxTimerTaskStackBuffer = uxTimerTaskStack;
   *pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
}




void vSYStaskInit ( void )
{
  (* xProcessTaskHandle ())
         = xTaskCreateStatic( vProcessTask, "ProcessTask", PROCESS_STK_SIZE , ( void * ) 1, PROCESS_TASK_PRIO  ,
                                 (StackType_t * const )ProcessTaskBuffer, &ProcessTaskControlBlock );
 (* xKeyboardProcessTaskHandle ())
         = xTaskCreateStatic( vKeyboardTask, "KeyboardTask", KEYBOARD_STK_SIZE , ( void * ) 1, KEYBOARD_TASK_PRIO   ,
                         (StackType_t * const )KeyboardTaskBuffer, &KeyboardTaskControlBlock );
  (* xCanOpenPeriodicTaskHandle ())
         = xTaskCreateStatic( vCanOpenPeriodicProcess, "CanOpenPeriodic", PERIODIC_CAN_STK_SIZE , ( void * ) 1, PERIODIC_CAN_TASK_PRIO ,
                 (StackType_t * const )CanOpnePeriodicTaskBuffer, &CanOpnePeriodicTaskControlBlock );
 (* xCanOpenProcessTaskHandle())
         = xTaskCreateStatic( vCanOpenProcess, "CanOpenProcessTask", CAN_OPEN_STK_SIZE , ( void * ) 1, CAN_OPEN_TASK_PRIO ,
 (StackType_t * const )CanOpneProccesTaskBuffer, &CanOpneProccesTaskControlBlock );
  DefautTask_Handler = xTaskCreateStatic( StartDefaultTask, "DefTask", DEFAULT_TASK_STACK_SIZE , ( void * ) 1, 3, (StackType_t * const )defaultTaskBuffer, &defaultTaskControlBlock );
  return;
}

void vSYSqueueInit ( void )
{
     *( xKeyboardQueue()) = xQueueCreateStatic( 16U, sizeof( KeyEvent ),ucQueueStorageArea, &xStaticQueue );
}
/*----------------------------------------------------------------------------*/
void vSYSeventInit ( void )
{
    xResetEventHandle =   xEventGroupCreateStatic( &xCreatedEventGroup );

}

void StartDefaultTask(void *argument)
{

  vLedDriverStart();
  xEventGroupSetBits( xResetEventHandle, RESTART_DISABLE);
  for(;;)
  {

    xEventGroupWaitBits(xResetEventHandle,RESTART_DISABLE,pdFALSE,pdFALSE,portMAX_DELAY );
    vTaskDelay(100);
    HAL_WDTInit();
  }
  /* USER CODE END 5 */
}


void vRestartNode( void )
{
    xEventGroupClearBitsFromISR(xResetEventHandle,RESTART_DISABLE);
    return;
}
