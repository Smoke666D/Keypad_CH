#include "init.h"
#include "main.h"
#include "drivers.h"

static void MX_GPIO_Init( void );
static void MX_IWDG_Init(void);
static void MX_SPI2_Init(void);
static void MX_TIM3_Init(void);
static void nvic_config(void);
static void InitBaseTick( void );
static void MX_TIM2_Init(void);




static uint8_t STATUS[KEY_COUNT];
static uint8_t  COUNTERS[KEY_COUNT];
static  const xKeyPortStruct xKeyPortMass[KEY_COUNT]={
        { KL1_GPIO_Port, KL1_Pin },
        { KL2_GPIO_Port, KL2_Pin },
        { KL3_GPIO_Port, KL3_Pin },
        { KL4_GPIO_Port, KL4_Pin },
        { KL5_GPIO_Port, KL5_Pin },
        { KL6_GPIO_Port, KL6_Pin },
        { KL7_GPIO_Port, KL7_Pin },
        { KL8_GPIO_Port, KL8_Pin },
};

FlagStatus fPortState (uint8_t i)
{
    return GPIO_ReadInputDataBit( xKeyPortMass[i].KeyPort, xKeyPortMass[i].KeyPin );
}
/*
 *
 *
 */
void vInitKeybord()
{
    KeybaordStruct_t KeyboardInit;
    KeyboardInit.KEYBOARD_COUNT    = KEY_COUNT;
    KeyboardInit.COUNTERS          = COUNTERS;
    KeyboardInit.STATUS            = STATUS;
    KeyboardInit.REPEAT_TIME       = vFDGetRegState( REPEAT_TIME_ADDRESS );
    KeyboardInit.KEYDOWN_HOLD_TIME = vFDGetRegState( KEYDOWN_HOLD_ADDRESS) ;
    KeyboardInit.KEYDOWN_DELAY     = vFDGetRegState( KEYDOWN_DELAY_ADRRES);
    KeyboardInit.KEYBOARD_PERIOD   = vFDGetRegState(KEYBOARD_PERIOD_ADRRES)*10U;
    KeyboardInit.getPortCallback = &fPortState;
    eKeyboardInit(&KeyboardInit);

}

/*
Функция инициализации перефирии устройства
*/
void vInit_DeviceConfig( void )
{	
    //Настройка тактирования всех устрйоств
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

	MX_GPIO_Init( );
	InitBaseTick();
	nvic_config();
	MX_SPI2_Init( );
	MX_TIM2_Init();
	MX_TIM3_Init( );
	MX_IWDG_Init();

	return;
}


static void nvic_config(void)
{
    NVIC_InitTypeDef      NVIC_InitStructure = {0};
    NVIC_InitStructure.NVIC_IRQChannel =  TIM3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

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


static void InitBaseTick( void )
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
    TIM_DeInit(TIM1);
    TIM_InternalClockConfig(TIM1);
    TIM_TimeBaseStructInit(&TIM_TimeBaseInitStructure);
    TIM_TimeBaseInitStructure.TIM_Period = 1000;
    TIM_TimeBaseInitStructure.TIM_Prescaler = 72;
    TIM_TimeBaseInit( TIM1, &TIM_TimeBaseInitStructure);
    TIM_Cmd( TIM1, ENABLE );
}


/*
Функция инициализации портов
*/
static void MX_GPIO_Init(void)
{
  /* GPIO Ports Clock Enable */
    GPIO_InitTypeDef      GPIO_InitSturcture = {0};
  /*Configure GPIO pin Output Level */
    GPIO_ResetBits(CS_GPIO_Port, CS_Pin);

  /*Configure GPIO pin Output Level */
  GPIO_ResetBits(GPIOA, GPIO_Pin_8);
	
  //Конфигурируем порты в
  GPIO_InitSturcture.GPIO_Pin =  GPIO_Pin_13|GPIO_Pin_14| GPIO_Pin_15;
  GPIO_InitSturcture.GPIO_Mode =GPIO_Mode_AIN ;
  GPIO_InitSturcture.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOC, &GPIO_InitSturcture);

  GPIO_InitSturcture.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_6
          |GPIO_Pin_7|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11 |GPIO_Pin_12|GPIO_Pin_15;
  GPIO_Init(GPIOA, &GPIO_InitSturcture);


  GPIO_InitSturcture.GPIO_Pin =GPIO_Pin_0|GPIO_Pin_11;
  GPIO_Init(GPIOB, &GPIO_InitSturcture);

  /*Configure GPIO pins : PA2 PA3 PA4 PA6
                           PA7 PA9 PA10 PA11
                           PA12 PA15 */
	

  /*Configure GPIO pin : KL1_Pin */
   GPIO_InitSturcture.GPIO_Pin = KL2_Pin|KL3_Pin|KL4_Pin |KL5_Pin|KL6_Pin|KL7_Pin|KL8_Pin;
   GPIO_InitSturcture.GPIO_Mode =GPIO_Mode_IN_FLOATING ;
   GPIO_Init(GPIOB, &GPIO_InitSturcture);

   GPIO_InitSturcture.GPIO_Pin =KL1_Pin;
   GPIO_Init(KL1_GPIO_Port, &GPIO_InitSturcture);
     /*Configure GPIO pins : KL2_Pin KL3_Pin PB14 KL4_Pin
                           KL5_Pin KL6_Pin KL7_Pin KL8_Pin */

  /*Configure GPIO pin : CS_Pin */
  GPIO_InitSturcture.GPIO_Pin = CS_Pin;
  GPIO_InitSturcture.GPIO_Mode =GPIO_Mode_Out_PP;

  GPIO_Init(CS_GPIO_Port, &GPIO_InitSturcture);

  GPIO_InitSturcture.GPIO_Pin = GPIO_Pin_8;
  GPIO_Init(GPIOA, &GPIO_InitSturcture);
  //Порты таймера
  GPIO_PinRemapConfig(GPIO_PartialRemap2_TIM2,ENABLE);
  GPIO_InitSturcture.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
  GPIO_InitSturcture.GPIO_Mode =GPIO_Mode_AF_PP;
  GPIO_Init(GPIOA, &GPIO_InitSturcture);

  GPIO_InitSturcture.GPIO_Pin = GPIO_Pin_10 ;
  GPIO_Init(GPIOB, &GPIO_InitSturcture);

  GPIO_InitSturcture.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_15;
  GPIO_Init(GPIOB, &GPIO_InitSturcture);

  GPIO_PinRemapConfig(GPIO_Remap1_CAN1, ENABLE);
  GPIO_InitSturcture.GPIO_Pin = GPIO_Pin_9;
  GPIO_Init(GPIOB, &GPIO_InitSturcture);
  GPIO_InitSturcture.GPIO_Pin = GPIO_Pin_8;
  GPIO_InitSturcture.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_Init(GPIOB, &GPIO_InitSturcture);

}

/**
  * @brief IWDG Initialization Function
  * @param None
  * @retval None
  */
static void MX_IWDG_Init(void)
{
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
    IWDG_SetPrescaler(IWDG_Prescaler_64);
    IWDG_SetReload(4095);
    IWDG_ReloadCounter();
    IWDG_Enable();
}

/**
  * @brief SPI2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI2_Init(void)
{
  SPI_InitTypeDef  SPI_InitStructure = {0};
  
    SPI_I2S_DeInit(SPI2);
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB ;
    SPI_InitStructure.SPI_CRCPolynomial = 0;
	SPI_Init(SPI2, &SPI_InitStructure);
	SPI_I2S_DMACmd(SPI2, SPI_I2S_DMAReq_Tx, ENABLE);


}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
    TIM_OCInitTypeDef TIM_OCInitStructure={0};
	
	TIM_DeInit(TIM2);
	TIM_InternalClockConfig(TIM2);
	TIM_TimeBaseStructInit(&TIM_TimeBaseInitStructure);
	TIM_TimeBaseInitStructure.TIM_Period = PWM_TIM_PERIOD;
	TIM_TimeBaseInitStructure.TIM_Prescaler = 2;
	TIM_TimeBaseInit( TIM2, &TIM_TimeBaseInitStructure);

    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = 500;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OC1Init( TIM2, &TIM_OCInitStructure );
    TIM_OC2Init( TIM2, &TIM_OCInitStructure );
    TIM_OC3Init( TIM2, &TIM_OCInitStructure );
    TIM_OC1PreloadConfig( TIM2, TIM_OCPreload_Disable );
    TIM_OC2PreloadConfig( TIM2, TIM_OCPreload_Disable );
    TIM_OC3PreloadConfig( TIM2, TIM_OCPreload_Disable );

    TIM_ARRPreloadConfig( TIM2, ENABLE );
    TIM_CtrlPWMOutputs(TIM2, ENABLE );

}

/**
  * @brief TIM4 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
    TIM_DeInit(TIM3);
    TIM_InternalClockConfig(TIM3);
    TIM_TimeBaseStructInit(&TIM_TimeBaseInitStructure);
    TIM_TimeBaseInitStructure.TIM_Period = 1000;
    TIM_TimeBaseInitStructure.TIM_Prescaler =4 ;
    TIM_TimeBaseInit( TIM3, &TIM_TimeBaseInitStructure);
    TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
    TIM_ITConfig(TIM3, TIM_IT_Update,ENABLE);
}



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
