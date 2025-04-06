/*
 * flash_data.h
 *
 *  Created on: Dec 1, 2021
 *      Author: igor.dymov
 */

#include "main.h"


#define FLASH_DATA_ADR             0x0800F000 //0x000fC00
#define CODE_ADR       		    0x00
#define NMT_STATE_ADR  	    	   ( CODE_ADR + 1 )
#define DEF_LED_BRIGTH_ADR  	   ( NMT_STATE_ADR + 1 )
#define DEF_BL_BRIGTH_ADR   	   ( DEF_LED_BRIGTH_ADR + 1 )
#define DEF_BL_COLOR_ADR    	   ( DEF_BL_BRIGTH_ADR  +1 )
#define NMT_START_MESSAGE   	   ( DEF_BL_COLOR_ADR + 1 )
#define KEYBOARD_PERIOD_ADRRES     ( NMT_START_MESSAGE  + 1 )
#define KEYDOWN_DELAY_ADRRES       ( KEYBOARD_PERIOD_ADRRES + 1 )
#define KEYDOWN_HOLD_ADDRESS       ( KEYDOWN_DELAY_ADRRES + 1 )
#define REPEAT_TIME_ADDRESS        ( KEYDOWN_HOLD_ADDRESS + 1 )
#define PWM_PERIOD_ADDRESS		   ( REPEAT_TIME_ADDRESS + 1 )
#define PWM_DUTY_ADDRESS		   (PWM_PERIOD_ADDRESS + 2)
#define TEST_START                 (PWM_DUTY_ADDRESS + 2 )


#define REG_SIZE      		TEST_START	+ 2

#define  TOTAL_REGISTER_COUNT REG_SIZE

#define VALID_CODE   	0x66
#define FLASH_SIZE  	0x00007FFFU
#define APP_ADDRESS    	0x00009000U

void vFDInit( void );
//void * cgetREGAdr(uint8_t adr);
uint16_t vGetBitrate( void );
void vSetBitrate( uint8_t data);
void vSetNodeID( uint8_t data);
uint8_t vGetNodeId( void );
uint16_t vFDGetNMTState( void );
void vFDSetRegState(uint8_t adr, uint8_t state);
uint8_t vFDGetStartMessage( void );
void vFDSetRegState16(uint8_t adr, uint16_t state);

