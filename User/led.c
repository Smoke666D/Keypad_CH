/*
 * led.c
 *
 *  Created on: Nov 12, 2021
 *      Author: igor.dymov
 */
#include "led.h"
#include <math.h>
#include "init.h"
#include "device_config.h"
#include "hal_gpio.h"
#include "hal_timers.h"

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
	SPI_Cmd(SPI2, ENABLE);
	DMA_Tx_Init(DMA1_Channel5, (u32)&SPI2->DATAR, (u32)data,3);
	HAL_TiemrEneblae(TIMER3);
	HAL_TiemrEneblae(TIMER2);
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
	if (brigth <= MAX_BRIGTH)
	{
	    /*
	         * Яркость устанвл  ивается для каждого цвета отдельно. Возможно задавать индивидуалное соотношение яркостей цветов для получения
	           * дополнительных переходных цветов, например  AMBER и YELLOW_GREEN
	    */
	    uint16_t pulse;
	    pulse =  (uint16_t)( ( (float)(brigth)/MAX_BRIGTH )* PWM_TIM_PERIOD )+1;
	    HAL_TIMER_SetPWMPulse(TIMER2, TIM_CHANNEL_1 | TIM_CHANNEL_2 | TIM_CHANNEL_3 ,pulse);
	    HAL_TIMER_EnablePWMCH(TIMER2,0);
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


void TimersCallback()
{
      HAL_ResetBit(CS_Port, CSPin);
      vLedProcess();
      HAL_SetBit(CS_Port, CSPin);
}
