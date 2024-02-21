/*
 * CO_driver_ST32F103.h
 *
 *  Created on: Nov 17, 2021
 *      Author: igor.dymov
 */

#ifndef KEYPAD_CO_DRIVER_ST32F103_H_
#define KEYPAD_CO_DRIVER_ST32F103_H_

#include "main.h"
#include "CO_driver_target.h"

#define CAN_SFID_MASK                      ((uint32_t)0x000007FFU)      /*!< mask of standard identifier */

uint8_t co_drv_create_os_objects(void);
uint8_t co_drv_mutex_lock(void);
uint8_t co_drv_mutex_unlock(void);

/* Semaphore for main app thread synchronization */
extern SemaphoreHandle_t co_drv_app_thread_sync_semaphore;

/* Semaphore for periodic thread synchronization */
extern SemaphoreHandle_t co_drv_periodic_thread_sync_semaphore;

/* Wakeup specific threads */
#define CO_WAKEUP_APP_THREAD()                          xSemaphoreGive(co_drv_app_thread_sync_semaphore)
#define CO_WAKEUP_PERIODIC_THREAD()                     xSemaphoreGive(co_drv_periodic_thread_sync_semaphore)
#define CO_WAIT_SYNC_APP_THREAD(max_time_in_ms)         xSemaphoreTake(co_drv_app_thread_sync_semaphore, (max_time_in_ms))
#define CO_WAIT_SYNC_PERIODIC_THREAD(max_time_in_ms)    xSemaphoreTake(co_drv_periodic_thread_sync_semaphore, (max_time_in_ms))
/* (un)lock critical section in CO_CANsend() */



uint8_t  co_drv_create_os_objects(void);
uint8_t co_drv_mutex_lock(void);
uint8_t co_drv_mutex_unlock(void);
void CO_CANsetConfigurationMode(void *CANptr);
void  prv_read_can_received_msg(CAN_TypeDef* can, uint32_t fifo);
void CAN_SendMessage();
void HAL_CAN_ErrorCallback(CAN_TypeDef *hcan);
#endif /* KEYPAD_CO_DRIVER_ST32F103_H_ */
