/********************************** (C) COPYRIGHT *******************************
 * File Name          : ch32v20x_it.c
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2021/06/06
 * Description        : Main Interrupt Service Routines.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#include "ch32v20x_it.h"
#include "main.h"
#include "device_config.h"
#include "hal_gpio.h"

void NMI_Handler(void) __attribute__((interrupt()));
void HardFault_Handler(void) __attribute__((interrupt()));
//void   USB_LP_CAN1_RX0_IRQHandler(void) __attribute__((interrupt())); /* USB LP and CAN1RX0 */
//void   CAN1_RX1_IRQHandler(void) __attribute__((interrupt()));        /* CAN1 RX1 */

/*********************************************************************
 * @fn      NMI_Handler
 *
 * @brief   This function handles NMI exception.
 *
 * @return  none
 */
void NMI_Handler(void)
{
    while(1)
    {

    }
}

/*******************************************************************************
 * @fn        HardFault_Handler
 *
 * @brief     This function handles Hard Fault exception.
 *
 * @return    None
 */
void HardFault_Handler(void)
{
    printf("HardFault_Handler\r\n");

      printf("mepc  :%08x\r\n", __get_MEPC());
      printf("mcause:%08x\r\n", __get_MCAUSE());
      printf("mtval :%08x\r\n", __get_MTVAL());

  while (1)
  {
  }
}
/*
void   USB_LP_CAN1_RX0_IRQHandler(void)
{

    prv_read_can_received_msg(CAN1,CAN_FIFO0);
}
void   CAN1_RX1_IRQHandler(void)
{

    prv_read_can_received_msg(CAN1,CAN_FIFO1);
}
*/
