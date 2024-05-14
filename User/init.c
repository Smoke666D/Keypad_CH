#include "init.h"
#include "main.h"
#include "device_config.h"
#include "hal_gpio.h"
#include "hal_timers.h"

static void MX_SPI2_Init(void);
static void nvic_config(void);



/*
Функция инициализации перефирии устройства
*/
void vInit_DeviceConfig( void )
{	
    HAL_InitGpioLib();
    HAL_SetBit(CS_Port,CSPin);
    HAL_InitGpioAIN(PORT_A,GPIO_3 | GPIO_4 | GPIO_6 | GPIO_7 | GPIO_8 | GPIO_9 | GPIO_10 | GPIO_11 | GPIO_12 | GPIO_15 );
    HAL_InitGpioAIN(PORT_B,GPIO_0 | GPIO_11 | GPIO_14);
    HAL_InitGpioAIN(PORT_C,GPIO_13 | GPIO_14 | GPIO_15);
    HAL_InitGpioIn(KL1_Port,KL1Pin);
    HAL_InitGpioIn(KL2_8_Port,KL2Pin|KL3Pin|KL4Pin |KL5Pin|KL6Pin|KL7Pin|KL8Pin);
    HAL_InitGpioAF(CAN_PORT,CAN_TX_Pin,GPIO_Remap1_CAN1,GPIO_Mode_AF_PP);
    HAL_InitGpioAF(CAN_PORT,CAN_RX_Pin,GPIO_Remap1_CAN1,GPIO_Mode_IPU);
    HAL_InitGpioOut( CS_Port, CSPin);

    HAL_InitGpioAIN(SPI_Port, SPI_MISO_Pin) ;
    HAL_InitGpioAF(SPI_Port,SPI_MOSI_Pin |  SPI_SCK_Pin ,0,GPIO_Mode_AF_PP);
    HAL_InitGpioAF(TIM2_CH1_2_Port,TIM2_CH1_Pin |  TIM2_CH2_Pin  ,GPIO_PartialRemap2_TIM2,GPIO_Mode_AF_PP);
    HAL_InitGpioAF(TIM2_CH3_Port  ,TIM2_CH3_Pin  ,GPIO_PartialRemap2_TIM2,GPIO_Mode_AF_PP);


    HAL_TIMER_InitIt( TIMER3, 17000000, 1000, &TimersCallback );
    HAL_TIMER_PWMTimersInit(TIMER2, 48000000, 1000, TIM_CHANNEL_1 | TIM_CHANNEL_2 | TIM_CHANNEL_3 );
    HAL_SPI_InitDMA(SPI2);


    RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	nvic_config();
	MX_SPI2_Init( );
	return;
}


static void nvic_config(void)
{
    NVIC_InitTypeDef      NVIC_InitStructure = {0};
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

    NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_Init(&NVIC_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = CAN1_RX1_IRQn;
    NVIC_Init(&NVIC_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = USB_HP_CAN1_TX_IRQn;
    NVIC_Init(&NVIC_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = CAN1_SCE_IRQn;
    NVIC_Init(&NVIC_InitStructure);
}
/*
*/






/**
  * @brief SPI2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI2_Init(void)
{

	SPI_I2S_DMACmd(SPI2, SPI_I2S_DMAReq_Tx, ENABLE);


}



/**
  * @brief TIM4 Initialization Function
  * @param None
  * @retval None
  */



void DMA_Tx_Init(DMA_Channel_TypeDef *DMA_CHx, u32 ppadr, u32 memadr, u16 bufsize)
{
    DMA_InitTypeDef DMA_InitStructure = {0};

    DMA_DeInit(DMA_CHx);
    DMA_InitStructure.DMA_PeripheralBaseAddr = ppadr;
    DMA_InitStructure.DMA_MemoryBaseAddr = memadr;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
    DMA_InitStructure.DMA_BufferSize = bufsize;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_Mode =DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA_CHx, &DMA_InitStructure);
}
