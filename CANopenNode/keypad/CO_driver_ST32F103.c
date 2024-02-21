/*
 * CAN module object for generic microcontroller.
 *
 * This file is a template for other microcontrollers.
 *
 * @file        CO_driver.c
 * @ingroup     CO_driver
 * @author      Janez Paternoster
 * @copyright   2004 - 2020 Janez Paternoster
 *
 * This file is part of CANopenNode, an opensource CANopen Stack.
 * Project home page is <https://github.com/CANopenNode/CANopenNode>.
 * For more information on CANopen see <http://www.can-cia.org/>.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


#include "301/CO_driver.h"
#include "CO_driver_ST32F103.h"

/* CAN masks for identifiers */
#define CANID_MASK                              0x07FF  /*!< CAN standard ID mask */
#define FLAG_RTR                                0x8000  /*!< RTR flag, part of identifier */
/* Mutex for atomic access */
static SemaphoreHandle_t co_mutex = NULL;

/* Semaphore for main app thread synchronization */
SemaphoreHandle_t co_drv_app_thread_sync_semaphore = NULL;

/* Semaphore for periodic thread synchronization */
SemaphoreHandle_t co_drv_periodic_thread_sync_semaphore = NULL;
/* Local CAN module object */
static CO_CANmodule_t* CANModule_local = NULL;  /* Local instance of global CAN module */

uint8_t co_drv_create_os_objects(void) {
    /* Create new mutex for OS context */
   // if (co_mutex == NULL) {

        co_mutex = xSemaphoreCreateRecursiveMutex();

   // }

    /* Semaphore for main app thread synchronization */
   // if (co_drv_app_thread_sync_semaphore == NULL) {

        co_drv_app_thread_sync_semaphore = xSemaphoreCreateBinary(  );

   // }

    /* Semaphore for periodic thread synchronization */
   // if (co_drv_periodic_thread_sync_semaphore == NULL) {

        co_drv_periodic_thread_sync_semaphore = xSemaphoreCreateBinary(  );

  //  }

    return 1;
}

/**
 * \brief           Lock mutex or wait to be available
 * \return          `1` on success, `0` otherwise
 */
uint8_t
co_drv_mutex_lock(void) {
    return xSemaphoreTakeRecursive (co_mutex, portMAX_DELAY) == pdTRUE;
}

/**
 * \brief           Release previously locked mutex
 * \return          `1` on success, `0` otherwise
 */
uint8_t
co_drv_mutex_unlock(void) {
    return xSemaphoreGiveRecursive(co_mutex) == pdTRUE;
}

/******************************************************************************/
void CO_CANsetConfigurationMode(void *CANptr){
    /* Put CAN module in configuration mode */
	 if (CANptr != NULL) {
	     CAN_OperatingModeRequest(CANptr,CAN_OperatingMode_Initialization);
	 }
}


/******************************************************************************/
void CO_CANsetNormalMode(CO_CANmodule_t *CANmodule){
    /* Put CAN module in normal mode */
	  if (CANmodule->CANptr != NULL && CAN_OperatingModeRequest(CANmodule->CANptr,CAN_OperatingMode_Normal) == CAN_ModeStatus_Success) {
	        CANmodule->CANnormal = true;
	  }
}


/******************************************************************************/
CO_ReturnError_t CO_CANmodule_init(
        CO_CANmodule_t         *CANmodule,
        void                   *CANptr,
        CO_CANrx_t              rxArray[],
        uint16_t                rxSize,
        CO_CANtx_t              txArray[],
        uint16_t                txSize,
        uint16_t                CANbitRate)
{
    uint16_t i;
    CAN_InitTypeDef       CAN_InitSturcture = {0};
    CAN_FilterInitTypeDef  sFilterConfig;


    /* verify arguments */
    if(CANmodule==NULL || rxArray==NULL || txArray==NULL){
        return CO_ERROR_ILLEGAL_ARGUMENT;
    }
    CANModule_local = CANmodule;
    /* Configure object variables */
    CANmodule->CANptr = CANptr;
    CANmodule->rxArray = rxArray;
    CANmodule->rxSize = rxSize;
    CANmodule->txArray = txArray;
    CANmodule->txSize = txSize;
    CANmodule->CANerrorStatus = 0;
    CANmodule->CANnormal = false;
    CANmodule->useCANrxFilters = (rxSize <= 32U) ? true : false;/* microcontroller dependent */
    CANmodule->bufferInhibitFlag = false;
    CANmodule->firstCANtxMessage = true;
    CANmodule->CANtxCount = 0U;
    CANmodule->errOld = 0U;

    for(i=0U; i<rxSize; i++){
        rxArray[i].ident = 0U;
        rxArray[i].mask = 0xFFFFU;
        rxArray[i].object = NULL;
        rxArray[i].CANrx_callback = NULL;
    }
    for(i=0U; i<txSize; i++){
        txArray[i].bufferFull = false;
    }

    /* Configure CAN module registers */
    CAN_InitSturcture.CAN_Mode = CAN_Mode_Normal;
    CAN_InitSturcture.CAN_SJW  = CAN_SJW_4tq;
    CAN_InitSturcture.CAN_BS1  = CAN_BS1_12tq;
    CAN_InitSturcture.CAN_BS2  = CAN_BS2_5tq;
    CAN_InitSturcture.CAN_TTCM = DISABLE;
    CAN_InitSturcture.CAN_ABOM = ENABLE;
    CAN_InitSturcture.CAN_AWUM = DISABLE;
    CAN_InitSturcture.CAN_NART = ENABLE;
    CAN_InitSturcture.CAN_RFLM = DISABLE;
    CAN_InitSturcture.CAN_TXFP = DISABLE;

    /* Configure CAN timing */
        switch (CANbitRate)
        {
            case 1000: CAN_InitSturcture.CAN_Prescaler  = 2;
                   break;
            case 500:  CAN_InitSturcture.CAN_Prescaler = 4;
                   break;
            case 250:  CAN_InitSturcture.CAN_Prescaler = 8;
                   break;
            default:
            case 125:  CAN_InitSturcture.CAN_Prescaler = 16;
                  break;
            case 100:  CAN_InitSturcture.CAN_Prescaler = 20;
                 break;
            case 50:  CAN_InitSturcture.CAN_Prescaler = 120;
                 break;
            case 20:  CAN_InitSturcture.CAN_Prescaler = 300;
                 break;
            case 10:  CAN_InitSturcture.CAN_Prescaler = 600;
                 break;
        }

        CAN_Init(CAN1, &CAN_InitSturcture);

        sFilterConfig.CAN_FilterMode  = CAN_FilterMode_IdList;
        sFilterConfig.CAN_FilterScale = CAN_FilterScale_16bit;
        sFilterConfig.CAN_FilterIdLow =   ( 0x180 | vGetNodeId() ) <<5;
        sFilterConfig.CAN_FilterIdHigh =  ( 0x200 | vGetNodeId() ) <<5;
        sFilterConfig.CAN_FilterMaskIdLow  = ( 0x300 | vGetNodeId() ) <<5;
        sFilterConfig.CAN_FilterMaskIdHigh = ( 0x400 | vGetNodeId() ) <<5;
        sFilterConfig.CAN_FilterFIFOAssignment = CAN_Filter_FIFO0;
        sFilterConfig.CAN_FilterNumber = 0;
        sFilterConfig.CAN_FilterActivation =ENABLE;
        CAN_FilterInit(&sFilterConfig);

        sFilterConfig.CAN_FilterIdLow = ( 0x500 | vGetNodeId() ) <<5;
        sFilterConfig.CAN_FilterIdHigh =  ( 0x600 | vGetNodeId() ) <<5;
        sFilterConfig.CAN_FilterMaskIdLow  = 0;
        sFilterConfig.CAN_FilterMaskIdHigh = 0;
        sFilterConfig.CAN_FilterFIFOAssignment = CAN_Filter_FIFO1;
        sFilterConfig.CAN_FilterNumber = 1;

        CAN_FilterInit(&sFilterConfig);

//Сбрасываем флаги прерываний, на случай если мы зашлию сюда по перегрузке
       CAN_ClearITPendingBit(CAN1, CAN_IT_BOF);
       CAN_ClearITPendingBit(CAN1, CAN_IT_TME );
       CAN_ClearITPendingBit(CAN1, CAN_IT_FMP0);
       CAN_ClearITPendingBit(CAN1, CAN_IT_FMP1);




       CAN_ITConfig(CAN1, CAN_IT_TME | CAN_IT_BOF | CAN_IT_FMP0 | CAN_IT_FMP1 ,ENABLE);
          printf("can inir\r\n");
    return ( CO_ERROR_NO );
}


/******************************************************************************/
void CO_CANmodule_disable(CO_CANmodule_t *CANmodule) {
	if (CANmodule != NULL && CANmodule->CANptr != NULL) {
	         CAN_OperatingModeRequest(CANmodule->CANptr,CAN_OperatingMode_Initialization);
	    }
}


/******************************************************************************/
CO_ReturnError_t CO_CANrxBufferInit(
        CO_CANmodule_t         *CANmodule,
        uint16_t                index,
        uint16_t                ident,
        uint16_t                mask,
        bool_t                  rtr,
        void                   *object,
        void                  (*CANrx_callback)(void *object, void *message))
{
    CO_ReturnError_t ret = CO_ERROR_NO;

    if((CANmodule!=NULL) && (object!=NULL) && (CANrx_callback!=NULL) && (index < CANmodule->rxSize)){
        /* buffer, which will be configured */
        CO_CANrx_t *buffer = &CANmodule->rxArray[index];

        /* Configure object variables */
        buffer->object = object;
        buffer->CANrx_callback = CANrx_callback;

        /* CAN identifier and CAN mask, bit aligned with CAN module. Different on different microcontrollers. */
        buffer->ident = ident & 0x07FFU;
        if(rtr){
            buffer->ident |= 0x0800U;
        }
        buffer->mask = (mask & 0x07FFU) | 0x0800U;

        /* Set CAN hardware module filter and mask. */
        if(CANmodule->useCANrxFilters){

        }
    }
    else{
        ret = CO_ERROR_ILLEGAL_ARGUMENT;
    }

    return ret;
}


/******************************************************************************/
CO_CANtx_t *CO_CANtxBufferInit(
        CO_CANmodule_t         *CANmodule,
        uint16_t                index,
        uint16_t                ident,
        bool_t                  rtr,
        uint8_t                 noOfBytes,
        bool_t                  syncFlag)
{
    CO_CANtx_t *buffer = NULL;

    if((CANmodule != NULL) && (index < CANmodule->txSize)){
        /* get specific buffer */
        buffer = &CANmodule->txArray[index];

        /* CAN identifier, DLC and rtr, bit aligned with CAN module transmit buffer.
         * Microcontroller specific. */
        buffer->ident = ((uint32_t)ident & CANID_MASK)
                               | ((uint32_t)(rtr ? FLAG_RTR : 0x00));
        buffer->DLC = noOfBytes;
        buffer->bufferFull = false;
        buffer->syncFlag = syncFlag;
    }

    return buffer;
}





static CanTxMsg pTXHeader =  {0,0,CAN_Id_Standard,0,0,{0}};


static uint32_t prv_send_can_message(CO_CANmodule_t* CANmodule, CO_CANtx_t *buffer) {

    uint8_t error_code;
    /* Check if TX FIFO is ready to accept more messages */

    pTXHeader.DLC                = (uint32_t)buffer->DLC;
    pTXHeader.RTR                = (buffer->ident & FLAG_RTR) ? CAN_RTR_REMOTE : CAN_RTR_DATA;
    pTXHeader.StdId              =  buffer->ident & CANID_MASK;
    for (int i=0;i<buffer->DLC;i++)
    {
        pTXHeader.Data[i] = buffer->data[i];
    }
    error_code = CAN_Transmit(CAN1, &pTXHeader);
    return ((error_code != CAN_TxStatus_NoMailBox) ? 1 : 0U);
}







/******************************************************************************/
void CO_CANclearPendingSyncPDOs(CO_CANmodule_t *CANmodule){
    uint32_t tpdoDeleted = 0U;

    CO_LOCK_CAN_SEND(CANmodule);
    /* Abort message from CAN module, if there is synchronous TPDO.
     * Take special care with this functionality. */
    if(/*messageIsOnCanBuffer && */CANmodule->bufferInhibitFlag){
        /* clear TXREQ */
        CANmodule->bufferInhibitFlag = false;
        tpdoDeleted = 1U;
    }
    /* delete also pending synchronous TPDOs in TX buffers */
    if(CANmodule->CANtxCount != 0U){
        uint16_t i;
        CO_CANtx_t *buffer = &CANmodule->txArray[0];
        for(i = CANmodule->txSize; i > 0U; i--){
            if(buffer->bufferFull){
                if(buffer->syncFlag){
                    buffer->bufferFull = false;
                    CANmodule->CANtxCount--;
                    tpdoDeleted = 2U;
                }
            }
            buffer++;
        }
    }
    CO_UNLOCK_CAN_SEND(CANmodule);


    if(tpdoDeleted != 0U){
        CANmodule->CANerrorStatus |= CO_CAN_ERRTX_PDO_LATE;
    }
}


/******************************************************************************/
/* Get error counters from the module. If necessary, function may use
    * different way to determine errors. */
static uint16_t rxErrors=0, txErrors=0, overflow=0;

void CO_CANmodule_process(CO_CANmodule_t *CANmodule) {
    uint32_t err;

    err = ((uint32_t)txErrors << 16) | ((uint32_t)rxErrors << 8) | overflow;

    if (CANmodule->errOld != err) {
        uint16_t status = CANmodule->CANerrorStatus;

        CANmodule->errOld = err;

        if (txErrors >= 256U) {
            /* bus off */
            status |= CO_CAN_ERRTX_BUS_OFF;
        }
        else {
            /* recalculate CANerrorStatus, first clear some flags */
            status &= 0xFFFF ^ (CO_CAN_ERRTX_BUS_OFF |
                                CO_CAN_ERRRX_WARNING | CO_CAN_ERRRX_PASSIVE |
                                CO_CAN_ERRTX_WARNING | CO_CAN_ERRTX_PASSIVE);

            /* rx bus warning or passive */
            if (rxErrors >= 128) {
                status |= CO_CAN_ERRRX_WARNING | CO_CAN_ERRRX_PASSIVE;
            } else if (rxErrors >= 96) {
                status |= CO_CAN_ERRRX_WARNING;
            }

            /* tx bus warning or passive */
            if (txErrors >= 128) {
                status |= CO_CAN_ERRTX_WARNING | CO_CAN_ERRTX_PASSIVE;
            } else if (rxErrors >= 96) {
                status |= CO_CAN_ERRTX_WARNING;
            }

            /* if not tx passive clear also overflow */
            if ((status & CO_CAN_ERRTX_PASSIVE) == 0) {
                status &= 0xFFFF ^ CO_CAN_ERRTX_OVERFLOW;
            }
        }

        if (overflow != 0) {
            /* CAN RX bus overflow */
            status |= CO_CAN_ERRRX_OVERFLOW;
        }

        CANmodule->CANerrorStatus = status;
    }
}


/******************************************************************************/


/******************************************************************************/
CO_ReturnError_t CO_CANsend(CO_CANmodule_t *CANmodule, CO_CANtx_t *buffer){
    CO_ReturnError_t err = CO_ERROR_NO;

    /* Verify overflow */
    if(buffer->bufferFull){
        if(!CANmodule->firstCANtxMessage){
            /* don't set error, if bootup message is still on buffers */
            CANmodule->CANerrorStatus |= CO_CAN_ERRTX_OVERFLOW;
        }
        err = CO_ERROR_TX_OVERFLOW;
    }

    CO_LOCK_CAN_SEND(CANmodule);
    /* if CAN TX buffer is free, copy message to it */
    if (prv_send_can_message(CANmodule, buffer) ) {
          CANmodule->bufferInhibitFlag = buffer->syncFlag;
      } else {
          buffer->bufferFull = true;
          CANmodule->CANtxCount++;
      }
    CO_UNLOCK_CAN_SEND(CANmodule);
    return err;
}



 void CAN_SendMessage()
{
	CANModule_local->firstCANtxMessage = false;
	CANModule_local->bufferInhibitFlag = false;
    if(CANModule_local->CANtxCount > 0U)
    {
        uint16_t i;             /* index of transmitting message */
        CO_CANtx_t *buffer = &CANModule_local->txArray[0];
        CO_LOCK_CAN_SEND(CANModule_local);

        for (i = CANModule_local->txSize; i > 0U; --i, ++buffer) {
                   /* Try to send message */
                   if (buffer->bufferFull) {
                       if (prv_send_can_message(CANModule_local, buffer)) {
                           buffer->bufferFull = false;
                           CANModule_local->CANtxCount--;
                           CANModule_local->bufferInhibitFlag = buffer->syncFlag;
                       }
                   }
               }
        /* Clear counter if no more messages */
        if(i == 0U){
        	CANModule_local->CANtxCount = 0U;
        }
        CO_UNLOCK_CAN_SEND(CANModule_local);
    }
}

/*
 *
 */

void CAN_GetRxMessage(CAN_TypeDef  *hcan, uint32_t RxFifo,  CO_CANrxMsg_t * pCANData)
{

    CanRxMsg CanRxStructure;

    CAN_Receive(CAN1, RxFifo, &CanRxStructure);
    pCANData->ident =   (CanRxStructure.StdId & CAN_SFID_MASK) | ( ( CanRxStructure.RTR ==  CAN_RTR_Remote) ? FLAG_RTR : 0x00 );
    pCANData->dlc = CanRxStructure.DLC;
    pCANData->data[0] = (uint8_t) CanRxStructure.Data[0];
    pCANData->data[1] = (uint8_t) CanRxStructure.Data[1];
    pCANData->data[2] = (uint8_t) CanRxStructure.Data[2];
    pCANData->data[3] = (uint8_t) CanRxStructure.Data[3];
    pCANData->data[4] = (uint8_t) CanRxStructure.Data[4];
    pCANData->data[5] = (uint8_t) CanRxStructure.Data[5];
    pCANData->data[6] = (uint8_t) CanRxStructure.Data[6];
    pCANData->data[7] = (uint8_t) CanRxStructure.Data[7];
    return;

}



 void  prv_read_can_received_msg(CAN_TypeDef * can, uint32_t fifo) {

    static CO_CANrxMsg_t rcvMsg;
    CAN_GetRxMessage(can, fifo,  &rcvMsg);

    	CO_CANrx_t * buffer = CANModule_local->rxArray;
    	    for (uint8_t index = CANModule_local->rxSize; index > 0U; --index, ++buffer)
    	    {
    	         if (((rcvMsg.ident ^ buffer->ident) & buffer->mask) == 0U)
    	         {
    	              if (buffer != NULL && buffer->CANrx_callback != NULL)
    	              {
    	                  buffer->CANrx_callback(buffer->object, (void*) &rcvMsg);
    	              }
    	              break;
    	          }
    	    }

    return;
}
/*
 *
 */
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_TypeDef  *hcan)
{
	prv_read_can_received_msg(hcan, CAN_FIFO0);
}
/*
 *
 */
void HAL_CAN_RxFifo1MsgPendingCallback(CAN_TypeDef  *hcan)
{
	prv_read_can_received_msg(hcan, CAN_FIFO1);
}


void HAL_CAN_ErrorCallback(CAN_TypeDef  *hcan)
{

    if(CAN_GetITStatus(CAN1, CAN_IT_BOF) != RESET)
    {
        vRestartNode();
    }

}

