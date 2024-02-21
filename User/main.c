/********************************** (C) COPYRIGHT *******************************
 * File Name          : main.c
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2021/06/06
 * Description        : Main program body.
 *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for 
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/

/*
 *@Note
 *task1 and task2 alternate printing
 */

#include "main.h"
#include "process.h"
#include "CanOpenNode.h"
/* Global define */
#define PROCESS_TASK_PRIO       10  //§¥§°§­§¨§¦§¯ §¢§½§´§¾ §³§¡§®§½ §£§½§³§°§¬§ª§«
#define PROCESS_STK_SIZE        256
#define KEYBOARD_TASK_PRIO      5
#define KEYBOARD_STK_SIZE       128
#define DEFAULT_TASK_PRIO       5
#define DEFAULT_STK_SIZE        128
#define PERIODIC_CAN_TASK_PRIO  5
#define PERIODIC_CAN_STK_SIZE   256
#define CAN_OPEN_TASK_PRIO      7
#define CAN_OPEN_STK_SIZE       700

/* Global Variable */
TaskHandle_t ProcessTaskHandle;

TaskHandle_t KeyboardTaskHandle;


TaskHandle_t DefautTask_Handler;


TaskHandle_t CanOpenProcessHandle;


TaskHandle_t CanOpenPeriodicHandle;




void StartDefaultTask(void *argument);
extern void vKeyboardTask(void *argument);
extern void vProcessTask(void *argument);
extern void vCanOpenPeriodicProcess(void *argument);
extern void vCanOpenProcess(void *argument);

EventGroupHandle_t xResetEventHandle;
StaticEventGroup_t xCreatedEventGroup;
/*********************************************************************
 * @fn      main
 *
 * @brief   Main program.
 *
 * @return  none
 */
int main(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    SystemCoreClockUpdate();
    Delay_Init();
    USART_Printf_Init(115200);
    printf("SystemClk:%d\r\n",SystemCoreClock);
    printf( "ChipID:%08x\r\n", DBGMCU_GetCHIPID() );
    printf("FreeRTOS Kernel Version:%s\r\n",tskKERNEL_VERSION_NUMBER);
    vInit_DeviceConfig( );




   xTaskCreate((TaskFunction_t )StartDefaultTask,
                            (const char*    )"defaultTask",
                            (uint16_t       )DEFAULT_STK_SIZE,
                            (void*          )NULL,
                            (UBaseType_t    )DEFAULT_TASK_PRIO,
                            (TaskHandle_t*  )&DefautTask_Handler);

    xTaskCreate((TaskFunction_t )vKeyboardTask,
                        (const char*    )"KeyboardTask",
                        (uint16_t       )KEYBOARD_STK_SIZE,
                        (void*          )NULL,
                        (UBaseType_t    )KEYBOARD_TASK_PRIO,
                        (TaskHandle_t*  )&KeyboardTaskHandle);

    xTaskCreate((TaskFunction_t )vProcessTask,
                    (const char*    )"ProcessTask",
                    (uint16_t       )PROCESS_STK_SIZE,
                    (void*          )NULL,
                    (UBaseType_t    )PROCESS_TASK_PRIO,
                    (TaskHandle_t*  )&ProcessTaskHandle);
    xTaskCreate((TaskFunction_t )vCanOpenProcess,
                          (const char*    )"CanOpenProcess",
                          (uint16_t       )CAN_OPEN_STK_SIZE,
                          (void*          )NULL,
                          (UBaseType_t    )CAN_OPEN_TASK_PRIO,
                          (TaskHandle_t*  )&CanOpenProcessHandle);
   xTaskCreate((TaskFunction_t )vCanOpenPeriodicProcess,
                       (const char*    )"CanOpenPeriodic",
                       (uint16_t       )PERIODIC_CAN_STK_SIZE,
                       (void*          )NULL,
                       (UBaseType_t    )PERIODIC_CAN_TASK_PRIO,
                       (TaskHandle_t*  )&CanOpenPeriodicHandle);


     vFDInit();
     vLedInit();
     vSetupKeyboard();
     vProceesInit();
     vCanOpenInit(CAN1);
//
    /* USER CODE END RTOS_THREADS */
    /* Create the event(s) */
    /* creation of xResetEvent */
    xResetEventHandle =   xEventGroupCreate(  );
    vTaskStartScheduler();

    while(1)
    {
        printf("shouldn't run at here!!\n");
    }
}


void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN 5 */
  /* Infinite loop */
  vLedDriverStart();
  printf("led_START\n\r");
  xEventGroupSetBits( xResetEventHandle, RESTART_DISABLE);
  for(;;)
  {

  //  xEventGroupWaitBits(xResetEventHandle,RESTART_DISABLE,pdFALSE,pdFALSE,portMAX_DELAY );
    vTaskDelay(500);
    IWDG_ReloadCounter();
    printf("We alive!!\r\n");
  }
  /* USER CODE END 5 */
}


void vRestartNode( void )
{
    xEventGroupClearBitsFromISR(xResetEventHandle,RESTART_DISABLE);
    return;
}

void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}


