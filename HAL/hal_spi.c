/*
 * HAL_SPI.c
 *
 *  Created on: 14 мая 2024 г.
 *      Author: i.dymov
 */


#include "hal_spi.h"

void     SPI1_IRQHandler(void)   __attribute__((interrupt("WCH-Interrupt-fast")));
void     SPI2_IRQHandler(void)   __attribute__((interrupt("WCH-Interrupt-fast")));
static void (* transmit_callback)(void);


void HAL_SPI_SetTXCallback( void (* f)(void) )
{
    transmit_callback = f;
}

void HAL_SPI_InitDMA(HAL_SPI_t spi )
{
#if MCU == CH32V2
	SPI_InitTypeDef  SPI_InitStructure = {0};
	if ( spi == SPI1 )  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
	if ( spi == SPI2 )  RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
	SPI_I2S_DeInit(spi);
	SPI_InitStructure.SPI_Direction = SPI_Direction_1Line_Tx;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB ;
    SPI_InitStructure.SPI_CRCPolynomial = 0;
    SPI_Init(spi, &SPI_InitStructure);
    SPI_I2S_DMACmd(spi, SPI_I2S_DMAReq_Tx, ENABLE);
    SPI_Cmd(spi, ENABLE);
#endif
}

void HAL_SPI_InitIT(HAL_SPI_t spi )
{
#if MCU == CH32V2
    SPI_InitTypeDef  SPI_InitStructure = {0};
    NVIC_InitTypeDef      NVIC_InitStructure = {0};
    if ( spi == SPI1 )  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
    if ( spi == SPI2 )  RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
    SPI_I2S_DeInit(spi);
    SPI_InitStructure.SPI_Direction = SPI_Direction_1Line_Tx;
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB ;
    SPI_InitStructure.SPI_CRCPolynomial = 0;
    SPI_Init(spi, &SPI_InitStructure);
    NVIC_InitStructure.NVIC_IRQChannel =  SPI2_IRQn;
         NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 5;
         NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
         NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
         NVIC_Init(&NVIC_InitStructure);
    SPI_Cmd(spi, ENABLE);
#endif
}

#if MCU == CH32V2

void SPI1_IRQHandler(void)
{

}

void SPI2_IRQHandler(void)
{
    if( SPI_I2S_GetITStatus( SPI2, SPI_I2S_IT_TXE ) != RESET )
    {
//        SPI_I2S_SendData( SPI1, TxData[Txval++] );
  //      if( Txval == 18 )
    //    {
    //        SPI_I2S_ITConfig( SPI1, SPI_I2S_IT_TXE , DISABLE );
      //  }
        transmit_callback();
    }
}
#endif


