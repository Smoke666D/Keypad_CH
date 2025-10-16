/*
 * flash_data.c
 *
 *  Created on: Dec 1, 2021
 *      Author: igor.dymov
 */

#include "flash_data.h"
#include "CO_NMT_Heartbeat.h"
#include "led.h"
#include "CANopen.h"
#include "CO_ODinterface.h"
#include "OD.h"
#include "hal_flash.h"
#include "hal_wdt.h"
#include "hw_data_model.h"

static uint8_t *MEM_If_Read_FS(uint8_t *src, uint8_t *dest, uint32_t Len);
static void vFDWtiteReg(void);


static void vFDWtiteReg(void)
{
	uint8_t * src = (uint8_t *) FLASH_DATA_ADR;
	HAL_FLASH_ErasePage(FLASH_DATA_ADR);
	HAL_FLASH_WriteByWord(GetDataRegister(), src, REG_SIZE );
	HAL_FLASH_Lock();
}

void vFDInit( void )
{
	uint8_t * src =  (uint8_t *) FLASH_DATA_ADR;
	MEM_If_Read_FS(src, GetDataRegister(), REG_SIZE );
	if (getReg8(CODE_ADR)!= VALID_CODE)
	{
	    vSetNodeID( 0x15);
	    vSetBitrate(2);
	    setReg8( NMT_STATE_ADR, 1 );
	    setReg8( DEF_LED_BRIGTH_ADR , 0x3F);
	    setReg8( DEF_BL_BRIGTH_ADR  , 0 );
	    setReg8( DEF_BL_COLOR_ADR,  WHITE);
        setReg8( NMT_START_MESSAGE , 1 );
        setReg8( KEYBOARD_PERIOD_ADRRES ,2 );
        setReg8( KEYDOWN_DELAY_ADRRES , 2 );
        setReg8( KEYDOWN_HOLD_ADDRESS  , 4 );
        setReg8( REPEAT_TIME_ADDRESS, 3 );
        setReg16( PWM_PERIOD_ADDRESS , 0 );
        setReg16( PWM_DUTY_ADDRESS, 44 );
        setReg8( TEST_START, 1 );
        vFDWtiteReg();
        MEM_If_Read_FS(src, GetDataRegister(), REG_SIZE );
	}
	if (( vGetBitrate()==0x00) || ( vGetBitrate()==0xFF)) vSetBitrate(2);
	if (( vGetNodeId() ==0x00)|| ( vGetNodeId() ==0xFF) ) vSetNodeID( 0x20);
    OD_set_value(OD_ENTRY_H2004_keyBoardParametr,0x01,GetRegisterAddr(KEYBOARD_PERIOD_ADRRES ), 1, true);
	OD_set_value(OD_ENTRY_H2004_keyBoardParametr,0x02,GetRegisterAddr(KEYBOARD_PERIOD_ADRRES +1) , 1, true);
	OD_set_value(OD_ENTRY_H2004_keyBoardParametr,0x03,GetRegisterAddr(KEYBOARD_PERIOD_ADRRES +2 ), 1, true);
	OD_set_value(OD_ENTRY_H2004_keyBoardParametr,0x04,GetRegisterAddr(KEYBOARD_PERIOD_ADRRES +3 ), 1, true);
	OD_set_value(OD_ENTRY_H2005_PWM_Parametr,0x01,GetRegisterAddr(KEYBOARD_PERIOD_ADRRES +3 ), 1, true);
	OD_set_value(OD_ENTRY_H2005_PWM_Parametr,0x02,GetRegisterAddr(KEYBOARD_PERIOD_ADRRES +3 ), 1, true);
}
/*
 *
 */
void vFDSetRegState(uint8_t adr, uint8_t state)
{
	setReg8(adr,state);
	vFDWtiteReg();
}

/*
 *
 */
void vFDSetRegState16(uint8_t adr, uint16_t state)
{
	setReg16(adr,state);
	vFDWtiteReg();
}

/*
* 圾抉戒志把忘投忘快技 戒扶忘折快扶我快 扼抗抉把抉扼找我 CAN 我戒 EEPROM
*/
uint16_t vGetBitrate()
{
       return (OB->Data0 & 0xF);
}

void vSetBitrate( uint8_t data)
{
    uint8_t temp = OB->Data0 & 0x80;
    ProgramOptionByteData(0,temp | (data & 0x7F )) ;
}

uint16_t vFDGetNMTState( void )
{
	 uint16_t res = 0;
	 if (getReg8(NMT_STATE_ADR) == 0x01) {
		 res = CO_NMT_STARTUP_TO_OPERATIONAL;
	 }
	 return res;
}

uint8_t vFDGetStartMessage( void )
{
	  return getReg8(NMT_START_MESSAGE);
}

uint8_t vGetNodeId( void )
{
  return ( OB->Data1 & 0xFF );
}

void vSetNodeID( uint8_t data)
{
    ProgramOptionByteData(1,data) ;
}




/**
  * @brief  Memory read routine.
  * @param  src: Pointer to the source buffer. Address to be written to.
  * @param  dest: Pointer to the destination buffer.
  * @param  Len: Number of data to be read (in bytes).
  * @retval Pointer to the physical address where data should be read.
  */
uint8_t *MEM_If_Read_FS(uint8_t *src, uint8_t *dest, uint32_t Len)
{
  /* Return a valid address to avoid HardFault */
  /* USER CODE BEGIN 4 */

    uint32_t i    = 0U;
    uint8_t *psrc = src;

    for ( i=0U; i<Len; i++ )
    {
      dest[i] = *psrc++;
    }
    return ( uint8_t* )( dest );

  /* USER CODE END 4 */
}



