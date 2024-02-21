/*
 * led.c
 *
 *  Created on: Nov 12, 2021
 *      Author: igor.dymov
 */
#include "led.h"
#include <math.h>
#include "init.h"

static uint8_t LED_ON[SPI_PACKET_SIZE] 			=         { 0x00 , 0x00 , 0x00 };
static uint8_t LED_BLINK[SPI_PACKET_SIZE]    	=         { 0x00 , 0x00 , 0x00 };

static uint16_t backligch_brigth		 = 0x1F;
static uint16_t led_brigth 				 = 0x3F;
static uint8_t color_div 				 = 1U;
static uint8_t brigth_color[SPI_PACKET_SIZE];
static uint16_t led_brigth_counter 		= 0;
static uint16_t led_blink_counter 		= 0;
static uint8_t BlinkON					= 1;
static uint8_t data[SPI_PACKET_SIZE];
/*
 * uint8_t data[SPI_PACKET_SIZE];
 * Защелка данных в SPI буферах
 * Поскольку длителность импульса мала, не целесобразно делать задержку через что-то, кроме пустого цикла
 */
static void vLatch( void )
{
    for (uint16_t i = 0U; i < 240; i++);
    GPIO_SetBits(CS_GPIO_Port, CS_Pin);
	for (uint8_t i = 0U; i < 240; i++);
	GPIO_ResetBits(CS_GPIO_Port, CS_Pin);
	return;
}
/*
 *
 */
static void vSPI_Transmit( )
{
  uint32_t tickstart = TIM_GetCounter(TIM1);
  DMA_SetCurrDataCounter(DMA1_Channel5,3);
  DMA_Cmd(DMA1_Channel5, ENABLE);
  while (DMA_GetFlagStatus(DMA1_FLAG_TC5)==RESET)
  {
	  if ( abs((TIM_GetCounter(TIM1) - tickstart)) >=  SPI_TIMEOUT)
	  {
	     break;
	  }
  }
  DMA_Cmd(DMA1_Channel5, DISABLE);
  DMA_ClearFlag(DMA1_FLAG_GL5);
  SPI_I2S_ClearFlag(SPI2,SPI_I2S_FLAG_OVR);
  return;
}

/*
 *
 */
static uint16_t calcBrigt(uint8_t pbr)
{
  return ( ( pbr > MAX_BRIGTH )?  MAX_BRIGTH_COUNTER :  (uint16_t) ( sin((double)pbr*(3.14/2.0)/MAX_BRIGTH )*(MAX_BRIGTH_COUNTER) ) );
}
/*
 *
 */
static uint8_t vSTPErrorDetection()
{
	/*Входим в режим Detectiom*/
     GPIO_SetBits(GPIOA, GPIO_Pin_0|GPIO_Pin_1); /*OE High*/
     GPIO_SetBits(GPIOB, GPIO_Pin_10);
     GPIO_ResetBits(CS_GPIO_Port, CS_Pin);  /*LE low*/
	 for (uint8_t i=0;i<(5+8*3);i++)
	 {

	     GPIO_SetBits(CS_GPIO_Port, CS_Pin);
		 vTaskDelay(1);
		 GPIO_ResetBits(CS_GPIO_Port, CS_Pin);
		 switch (i)
		 {
		    case 0:
		        GPIO_ResetBits(GPIOA, GPIO_Pin_0|GPIO_Pin_1); /*OE Low*/
		        GPIO_ResetBits(GPIOB, GPIO_Pin_10);
		    	break;
		    case 1:
		        GPIO_SetBits(GPIOA, GPIO_Pin_0|GPIO_Pin_1); /*OE High*/
		        GPIO_SetBits(GPIOB, GPIO_Pin_10);
		    	break;
		    case 2:
		        GPIO_SetBits(CS_GPIO_Port, CS_Pin);  /*LE High*/
		    	break;
		    case 3:
		        GPIO_ResetBits(CS_GPIO_Port, CS_Pin);  /*LE low*/
		    	break;
		    case 4:
		        GPIO_SetBits(GPIOA, GPIO_Pin_15);
		    	break;
		    default:
		    	break;
		 }
	 }
	 GPIO_ResetBits(GPIOA, GPIO_Pin_15);
	 vLatch();
	 GPIO_ResetBits(GPIOA, GPIO_Pin_0|GPIO_Pin_1); /*OE Low*/
	 GPIO_ResetBits(GPIOB, GPIO_Pin_10);
	 vTaskDelay(1);
	 GPIO_SetBits(CS_GPIO_Port, CS_Pin);
	 vTaskDelay(1);
	 GPIO_ResetBits(CS_GPIO_Port, CS_Pin);
	 GPIO_ResetBits(GPIOA, GPIO_Pin_0|GPIO_Pin_1); /*OE Low*/
	 GPIO_ResetBits(GPIOB, GPIO_Pin_10);
	 uint32_t buf=0;
	 for (uint8_t i =0;i<(3*8);i++)
	 {
	     GPIO_SetBits(CS_GPIO_Port, CS_Pin);
		 vTaskDelay(1);
		 if ( GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_14) == SET )
         {
			 buf=buf<<1;
			 buf |= 0x0001;
         }
		 GPIO_ResetBits(CS_GPIO_Port, CS_Pin);
		 vTaskDelay(1);
	 }
	 return ((buf == 0x0FFFFFF)? 0U : 1U );
}
/*
 *
 */
static void vSTPNormalMode()
{
     GPIO_SetBits(GPIOA, GPIO_Pin_0|GPIO_Pin_1); /*OE High*/
     GPIO_SetBits(GPIOB, GPIO_Pin_10);
     GPIO_ResetBits(CS_GPIO_Port, CS_Pin);  /*LE low*/
	 for (uint8_t i=0;i<5;i++)
	 {

	    GPIO_SetBits(CS_GPIO_Port, CS_Pin);
	    vTaskDelay(1);
	    GPIO_ResetBits(CS_GPIO_Port, CS_Pin);
		switch (i)
		{
		    case 1:
		        GPIO_ResetBits(GPIOA, GPIO_Pin_0|GPIO_Pin_1); /*OE Low*/
		        GPIO_ResetBits(GPIOB, GPIO_Pin_10);
		    	break;
		    case 2:
		        GPIO_SetBits(GPIOA, GPIO_Pin_0|GPIO_Pin_1); /*OE High*/
		        GPIO_SetBits(GPIOB, GPIO_Pin_10);
		    	break;
		    default:
		    	break;
		 }
	 }
	 return;
}
/*
 *
 */
void vLedInit()
{
	vSetBackLigthColor(vFDGetRegState(DEF_BL_COLOR_ADR));
	vSetLedBrigth(vFDGetRegState(DEF_LED_BRIGTH_ADR));
	vSetBackLigth(vFDGetRegState(DEF_BL_BRIGTH_ADR));
    vSetBrigth(MAX_BRIGTH);
    return;
}


/*
 *
 */
void vLedDriverStart(void)
{
    GPIO_InitTypeDef      GPIO_InitSturcture = {0};
    GPIO_InitSturcture.GPIO_Pin =  GPIO_Pin_0|GPIO_Pin_1 |GPIO_Pin_15;
    GPIO_InitSturcture.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitSturcture.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitSturcture);

    GPIO_InitSturcture.GPIO_Pin =  GPIO_Pin_10;
    GPIO_Init(GPIOB, &GPIO_InitSturcture);

    GPIO_InitSturcture.GPIO_Pin =  GPIO_Pin_14;
    GPIO_InitSturcture.GPIO_Mode = GPIO_Mode_IN_FLOATING ;
    GPIO_Init(GPIOA, &GPIO_InitSturcture);


	if (vSTPErrorDetection() == 0)
	{
	    printf("NormalMode\r\n");
		vSTPNormalMode();
	}
	GPIO_Init(GPIOB, &GPIO_InitSturcture);
	GPIO_InitSturcture.GPIO_Mode = GPIO_Mode_AF_PP ;
	GPIO_InitSturcture.GPIO_Pin =  GPIO_Pin_10 | GPIO_Pin_13 | GPIO_Pin_15 ;
	GPIO_Init(GPIOB, &GPIO_InitSturcture);
	GPIO_InitSturcture.GPIO_Pin =  GPIO_Pin_0|GPIO_Pin_1 |GPIO_Pin_15;
	GPIO_Init(GPIOA, &GPIO_InitSturcture);


	SPI_Cmd(SPI2, ENABLE);
	DMA_Tx_Init(DMA1_Channel5, (u32)&SPI2->DATAR, (u32)data,3);
	TIM_Cmd( TIM3, ENABLE );
	TIM_Cmd( TIM2, ENABLE );

	return;
}
/*
 * Функция включения светодиодоы.
 * Корректность аргументов проверятся при вызове
 */
void vSetLedOn(uint8_t Color,uint8_t State)
{
	LED_ON[Color-1] = State;
	return;
}
/*
 *
 */
void vSetLedBlink(uint8_t Color, uint8_t State)
{
	LED_BLINK[Color-1] = State;
	return;
}

/*
 *
 */
void vSetLedBrigth(uint8_t brigth)
{
	led_brigth = calcBrigt(brigth);
	return;
}

void vSetBackLigthColor(uint8_t color)
{
	brigth_color[0]=MAX_DATA;
	brigth_color[1]=MAX_DATA;
	brigth_color[2]=MAX_DATA;
	color_div =2;
	switch (color)
	{
		case  RED:
			brigth_color[1]=0x00;
			brigth_color[2]=0x00;
			color_div =1;
			break;
		case GREEN:
			brigth_color[0]=0x00;
			brigth_color[2]=0x00;
			color_div =1;
			break;
		case BLUE:
			brigth_color[0]=0x00;
			brigth_color[1]=0x00;
			color_div =1;
			break;
		case YELLOW:
			brigth_color[2]=0x00;
			break;
		case YELLOW_GREEN:
			brigth_color[2]=0x00;
			break;
		case  AMBER:
			brigth_color[2]=0x00;
			break;
		case VIOLET:
			brigth_color[1]=0x00;
			break;
		case CYAN:
			brigth_color[0]=0x00;
			break;
		case WHITE:
		default:
			break;
	}
	return;
}

/*
 *
 */
void vSetBackLigth(uint8_t brigth)
{
	backligch_brigth =calcBrigt( brigth);
}
/*
 * Функция установки якрости.
 */
void vSetBrigth(uint8_t brigth)
{
    TIM_OCInitTypeDef TIM_OCInitStructure={0};

	if (brigth <= MAX_BRIGTH)
	{
	    /*
	         * Яркость устанвл  ивается для каждого цвета отдельно. Возможно задавать индивидуалное соотношение яркостей цветов для получения
	           * дополнительных переходных цветов, например  AMBER и YELLOW_GREEN
	    */
	    TIM_CtrlPWMOutputs(TIM2, DISABLE );
	    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	    TIM_OCInitStructure.TIM_Pulse = (uint16_t)( ( (float)(brigth)/MAX_BRIGTH )* PWM_TIM_PERIOD )+1;
	    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
	    TIM_OC1Init( TIM2, &TIM_OCInitStructure );
	    TIM_OC2Init( TIM2, &TIM_OCInitStructure );
	    TIM_OC3Init( TIM2, &TIM_OCInitStructure );
	    TIM_CtrlPWMOutputs(TIM2, ENABLE );
	    printf("BRIHTG %d\r\n",TIM_OCInitStructure.TIM_Pulse);
	  //  TIM_OC1PreloadConfig( TIM1, TIM_OCPreload_Disable );
	    //TIM_ARRPreloadConfig( TIM1, ENABLE );
	    TIM_Cmd( TIM2, ENABLE );
	}
}
/*
 *  Функция вывода данных в SPI, вызывается по прерыванию таймра №4
 */
void vLedProcess( void )
{
	/*Cбравысваем флаг тамера 4*/

	
	uint8_t temp_led;
	led_brigth_counter++;
    if (led_brigth_counter>(MAX_BRIGTH_COUNTER))
    {
    	led_brigth_counter = 0;
    }
    if (LED_BLINK[0] || LED_BLINK[1] || LED_BLINK[2])
    {

        if (++led_blink_counter > 22500U)
        {
        	led_blink_counter = 0;
        	BlinkON	 =  (BlinkON ==1) ? 0 : 1;
        }
    	if (BlinkON == 1)
    	{
    		LED_ON[0] |= LED_BLINK[0];
    		LED_ON[1] |= LED_BLINK[1];
    		LED_ON[2] |= LED_BLINK[2];
    	}
    	else
    	{
    		LED_ON[0] &= ~LED_BLINK[0];
    		LED_ON[1] &= ~LED_BLINK[1];
    		LED_ON[2] &= ~LED_BLINK[2];
    	}
    }
    else
    {
    	BlinkON = 1U;
    }
    data[0] =0;
    data[1] =0;
    data[2] =0;
    temp_led = ~(LED_ON[0]  | LED_ON[1] | LED_ON[2] );
	if (led_brigth_counter < backligch_brigth)
	{
		 data[2]=brigth_color[0] & temp_led;
	 	 data[1]=brigth_color[1] & temp_led;
	 	 data[0]=brigth_color[2] & temp_led;
 	}
	if (led_brigth_counter <= led_brigth)
	{
		data[2]|=LED_ON[0];
	 	data[1]|=LED_ON[1];
	 	data[0]|=LED_ON[2];
    }
    vSPI_Transmit();
    return;
}




