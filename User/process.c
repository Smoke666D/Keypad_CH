/*
 * process.c
 *
 *  Created on: Nov 12, 2021
 *      Author: igor.dymov
 */

#include "process.h"
#include "system_init.h"
#include "hw_data_model.h"


static TaskHandle_t  pProcessTaskHandle    __SECTION(RAM_SECTION_CCMRAM);
static uint8_t key_mask;

static QueueHandle_t     pKeyboard        = NULL;
static KeyEvent          TempEvent        = { 0U };

TaskHandle_t * xProcessTaskHandle ()
{
    return  &pProcessTaskHandle ;
}

static ODR_t OD_writeLed(OD_stream_t *stream,const  void *buf, OD_size_t count, OD_size_t *countWritten);
static ODR_t OD_readLed(OD_stream_t *stream, void *buf, OD_size_t count, OD_size_t *countRead);
static ODR_t OD_writeTest(OD_stream_t *stream,const  void *buf, OD_size_t count, OD_size_t *countWritten);
static ODR_t OD_readTest(OD_stream_t *stream, void *buf, OD_size_t count, OD_size_t *countRead);
static ODR_t OD_writeBlink(OD_stream_t *stream,const void *buf, OD_size_t count, OD_size_t *countWritten);
static ODR_t OD_writeBRIGTH(OD_stream_t *stream,const void *buf,OD_size_t count, OD_size_t *countWritten);
static ODR_t OD_readBRIGTH(OD_stream_t *stream, void *buf, OD_size_t count, OD_size_t *countRead);
static ODR_t OD_writeNode(OD_stream_t *stream,const void *buf, OD_size_t count, OD_size_t *countWritten);
static ODR_t OD_readNode(OD_stream_t *stream, void *buf, OD_size_t count, OD_size_t *countRead);
static ODR_t OD_writeBITRATE(OD_stream_t *stream,const void *buf, OD_size_t count, OD_size_t *countWritten);
static ODR_t OD_readBITRATE(OD_stream_t *stream, void *buf, OD_size_t count, OD_size_t *countRead);
static ODR_t OD_writeNMT(OD_stream_t *stream, const void *buf, OD_size_t count, OD_size_t *countWritten);
static ODR_t OD_writePWM(OD_stream_t *stream, const void *buf, OD_size_t count, OD_size_t *countWritten);
static ODR_t OD_writeKeyboardParametr(OD_stream_t *stream, const void *buf, OD_size_t count, OD_size_t *countWritten);
static ODR_t OD_readKeyboardParametr(OD_stream_t *stream, void *buf, OD_size_t count, OD_size_t *countRead);

/* Variables used for triggering TPDO, see simulation in app_programRt(). */
OD_extension_t OD_LED_data_extension = {
    .object = NULL,
    .read =  OD_readLed,
    .write = OD_writeLed
};

OD_extension_t OD_BLINK_data_extension = {
    .object = NULL,
    .read =  OD_readOriginal,
    .write = OD_writeBlink
};

OD_extension_t OD_BRIGTH_data_extension = {
    .object = NULL,
    .read =   OD_readBRIGTH,
    .write =  OD_writeBRIGTH
};

/* Variables used for triggering TPDO, see simulation in app_programRt(). */
OD_extension_t OD_KEY_extension = {
    .object = NULL,
    .read =  OD_readOriginal,
    .write = NULL
};

OD_extension_t OD_NODE_data_extension = {
    .object = NULL,
    .read =   OD_readNode,
    .write =  OD_writeNode
};

OD_extension_t OD_BITRATE_data_extension = {
    .object = NULL,
    .read   = OD_readBITRATE,
    .write  = OD_writeBITRATE
};

OD_extension_t OD_NMT_data_extension = {
    .object = NULL,
    .read =   NULL,
    .write = OD_writeNMT
};


OD_extension_t OD_keyBoardParametr_extension = {
    .object = NULL,
    .read =  OD_readKeyboardParametr,
    .write = OD_writeKeyboardParametr
};

OD_extension_t OD_PWM_extension = {
    .object = NULL,
    .read =  OD_readOriginal,
    .write = OD_writePWM
};

OD_extension_t OD_Test_extension = {
    .object = NULL,
    .read =  OD_readTest,
    .write = OD_writeTest
};

uint8_t *OD_KEY_flagsPDO = NULL;



void vProceesInit( void)
{
	pKeyboard = *( xKeyboardQueue());
	OD_extension_init(OD_ENTRY_H2000_digitalInputModuleKeysStates, &OD_KEY_extension);
	OD_extension_init(OD_ENTRY_H2001_digitalOutputModuleLED_ON, &OD_LED_data_extension);
	OD_extension_init(OD_ENTRY_H2002_digitalOutputModuleLEDBlink, &OD_BLINK_data_extension);
	OD_extension_init(OD_ENTRY_H2003_digitalOutputModuleBrightnessLevel, &OD_BRIGTH_data_extension );
	OD_extension_init(OD_ENTRY_H2013_CANopenNodeID,   &OD_NODE_data_extension);
	OD_extension_init(OD_ENTRY_H2012_setDeviceActiveOnStartup, &OD_NMT_data_extension);
	OD_extension_init(OD_ENTRY_H2010_baudRateSetting, &OD_BITRATE_data_extension);
	OD_extension_init(OD_ENTRY_H2004_keyBoardParametr, &OD_keyBoardParametr_extension);
	OD_extension_init(OD_ENTRY_H2005_PWM_Parametr, &OD_PWM_extension);
	OD_extension_init(OD_ENTRY_H2014_testRegister, &OD_Test_extension);
	OD_KEY_flagsPDO = OD_getFlagsPDO(OD_ENTRY_H2000_digitalInputModuleKeysStates);
}



static ODR_t OD_writePWM(OD_stream_t *stream, const void *buf, OD_size_t count, OD_size_t *countWritten)
{
	if ( stream->subIndex == PWM_PERIOD_SUBINDEX)
	{
	    vFDSetRegState16(PWM_PERIOD_ADDRESS  , CO_getUint16(buf) );
	}
	else
	{
	   vFDSetRegState16( PWM_DUTY_ADDRESS  , CO_getUint16(buf) );

	}

	*countWritten = sizeof(uint16_t);
	return ( ODR_OK  );

}
static ODR_t OD_writeKeyboardParametr(OD_stream_t *stream, const void *buf, OD_size_t count, OD_size_t *countWritten)
{

	switch (stream->subIndex)
    {
	    case KEYBOARD_PERIOD_SUBINDEX:
	        vFDSetRegState( KEYBOARD_PERIOD_ADRRES , CO_getUint8(buf) );
	        break;
	    case KEYDOWN_DELAY_SUBINDEX:
	        vFDSetRegState( KEYDOWN_DELAY_ADRRES  , CO_getUint8(buf) );
	        break;
	    case KEYDOWN_HOLD_SUBINDEX :
	        vFDSetRegState(KEYDOWN_HOLD_ADDRESS  , CO_getUint8(buf) );
	        break;
	    case REPEAT_TIME_SUBINDEX :
	        default:
	        vFDSetRegState( REPEAT_TIME_ADDRESS  , CO_getUint8(buf) );
	        break;
	}
    *countWritten = sizeof(uint8_t);
	return ( ODR_OK );
}



static ODR_t OD_readKeyboardParametr(OD_stream_t *stream, void *buf, OD_size_t count, OD_size_t *countRead)
{
 	CO_setUint8(buf,getReg8(KEYBOARD_PERIOD_ADRRES));
    *countRead = sizeof(uint8_t);
    return (ODR_OK);
}



/*
 * Callback функция записи в oбъект 2012. Принимает 2 значения, в активном NTM состояние после стратна OPERATIONAL, в не активном PRE_OPERATIONAL
 */
ODR_t OD_writeNMT(OD_stream_t *stream,const void *buf,  OD_size_t count, OD_size_t *countWritten)
{
	ODR_t res = ODR_OK;
	if (stream == NULL || buf == NULL || countWritten == NULL)
	{
		res = ODR_DEV_INCOMPAT;
	}
	else
	{
		switch (CO_getUint8(buf))
		{
	    	case ACTIVE:
	    	case NOT_ACTIVE:
	    		vFDSetRegState( NMT_STATE_ADR , CO_getUint8(buf) );
	    		break;
	    	default:
	    		res = ODR_INVALID_VALUE;
	    		break;
		}
	}
	return ( res );
}
/*
 * 	Callback функция записи в oбъект 2013. Node Id. Принимает значения от 1 до 7F
 */
ODR_t OD_writeNode(OD_stream_t *stream,const  void *buf, OD_size_t count, OD_size_t *countWritten)
{
    uint8_t data =   CO_getUint8(buf);
	if ( ( data != 0 ) && ( data  == 0xFF) )
	{
	    vSetNodeID( data );
	    *countWritten = sizeof(uint8_t);
	}
	else
	{
	    return ( ODR_INVALID_VALUE );
	}
	return ( ODR_OK );
}

ODR_t OD_readNode(OD_stream_t *stream, void *buf, OD_size_t count, OD_size_t *countRead)
{
    CO_setUint8(buf,vGetNodeId());
    *countRead = sizeof(uint8_t);
    return (ODR_OK);
}



/*
 * 	Callback функция записи в oбъект 2010. Скорость CAN. Принимает значения от 0 до 7
 */
ODR_t OD_writeBITRATE(OD_stream_t *stream,const  void *buf,
                      OD_size_t count, OD_size_t *countWritten)
{
	if  ( CO_getUint8(buf) <= MAX_BITRATE )
	{
	    *countWritten = sizeof(uint8_t);
	    vSetBitrate( CO_getUint8(buf) );
	}
	else
	{
	    *countWritten = 0;
	     return ( ODR_INVALID_VALUE );
	}
	return ( ODR_OK );
}


ODR_t OD_readBITRATE(OD_stream_t *stream, void *buf, OD_size_t count, OD_size_t *countRead)
{
    CO_setUint8(buf,vGetBitrate());
    *countRead = sizeof(uint8_t);
    return (ODR_OK);
}
/*
 *
 */
ODR_t OD_writeLed(OD_stream_t *stream,const  void *buf, OD_size_t count, OD_size_t *countWritten)
{
    *countWritten = sizeof(uint8_t);
    switch (stream->subIndex)
	{
			case RED_COLOR:
			case GREEN_COLOR:
			case BLUE_COLOR:
				vSetLedOn(stream->subIndex,CO_getUint8(buf));
				break;
	}
	return ( ODR_OK );
}

ODR_t OD_readLed(OD_stream_t *stream, void *buf, OD_size_t count, OD_size_t *countRead)
{
    CO_setUint8(buf,uGetLedState(stream->subIndex));
    *countRead = sizeof(uint8_t);
    return (ODR_OK);
}


static ODR_t OD_writeTest(OD_stream_t *stream,const  void *buf, OD_size_t count, OD_size_t *countWritten)
{
    *countWritten = sizeof(uint8_t);
    vFDSetRegState(TEST_START,CO_getUint8(buf));
    return (ODR_OK);
}
static ODR_t OD_readTest(OD_stream_t *stream, void *buf, OD_size_t count, OD_size_t *countRead)
{
    CO_setUint8(buf,getReg8(TEST_START));
     *countRead = sizeof(uint8_t);
     return (ODR_OK);
}

ODR_t OD_writeBlink(OD_stream_t *stream,const  void *buf,
                      OD_size_t count, OD_size_t *countWritten)
{

	if (stream == NULL || buf == NULL || countWritten == NULL)
	{
	        return ( ODR_DEV_INCOMPAT );
	}
	if ((stream->subIndex >=RED_COLOR) && (stream->subIndex <=BLUE_COLOR))
	{
		vSetLedBlink(stream->subIndex ,CO_getUint8(buf));
	}
	else
	{
	    	return ( ODR_SUB_NOT_EXIST );
	}
	return ( OD_writeOriginal(stream, buf, count, countWritten) );
}

ODR_t OD_writeBRIGTH(OD_stream_t *stream,const void *buf,
                      OD_size_t count, OD_size_t *countWritten)
{



    if  ( ( stream->subIndex == 1U ) ||  ( stream->subIndex == 2U ) ||  ( stream->subIndex == 5U ) ||  ( stream->subIndex == 6U ) ) {
       	if ( ( CO_getUint8(buf) >=0 ) && ( CO_getUint8(buf) <= MAX_BRIGTH ) ) {
       		switch (stream->subIndex)
       		{
       		    case 1U:
       		    	vSetLedBrigth(CO_getUint8(buf));
       		    	break;
       		    case 2U:
       		    	vSetBackLigth(CO_getUint8(buf));
       		    	break;
       		    case 5U:
       		    	vFDSetRegState( DEF_LED_BRIGTH_ADR , CO_getUint8(buf) );
       		    	break;
       		    case 6U:
       		    	vFDSetRegState( DEF_BL_BRIGTH_ADR , CO_getUint8(buf) );
       		    	break;
       		}
       	}
       	else {
       		return ( ODR_INVALID_VALUE );
      	}
     }

     if ( (stream->subIndex == 3U ) ||   (stream->subIndex == 4U ) )   {
       	if  ( ( CO_getUint8(buf) !=0 ) && ( CO_getUint8(buf) <= MAX_COLOR ) )  {
       		switch (stream->subIndex)
       		{
       		    case 3U:
       		    	vSetBackLigthColor(CO_getUint8(buf));
       		    	break;
       		    case 4U:
       		    	vFDSetRegState( DEF_BL_COLOR_ADR , CO_getUint8(buf) );
       		    	break;
       		}
       	}
       	else {
       		return ( ODR_INVALID_VALUE );
       	}
     }
     else
     {
    	  return ( ODR_SUB_NOT_EXIST );
     }

     return ( ODR_OK );
}


static ODR_t OD_readBRIGTH(OD_stream_t *stream, void *buf, OD_size_t count, OD_size_t *countRead)
{

    switch (stream->subIndex)
    {
            case 1:
                CO_setUint8(buf,vGetNodeId());
                break;
            case 2:
                CO_setUint8(buf,vGetNodeId());
                break;
            case 3:
                CO_setUint8(buf,vGetNodeId());
                break;
            case 4:
                CO_setUint8(buf,getReg8(DEF_BL_COLOR_ADR));
                break;
            case 5:
                CO_setUint8(buf,getReg8(DEF_LED_BRIGTH_ADR));
                break;
            case 6:
                CO_setUint8(buf,getReg8(DEF_BL_BRIGTH_ADR));
                break;


    }

       *countRead = sizeof(uint8_t);
       return (ODR_OK);
}

uint16_t test_fsm = 0;

void vTestFunction( uint16_t * test_counter, uint8_t  keymask )
{
    uint8_t  test_mask ;
    if (++(*test_counter) == 100)
    {
        *test_counter = 0;
        test_fsm++;
        vSetLedOn(0x01,0);
        vSetLedOn(0x02,0);
        vSetLedOn(0x03,0);
        if (test_fsm <= 8 )
        {
            test_mask = 0x01<<test_fsm;
            vSetLedOn(0x01,test_mask );
        }
        else
            if (test_fsm <= 16 )
            {
                test_mask = 0x01<<(test_fsm -8);
                vSetLedOn(0x02,test_mask );
            }
            else
                if (test_fsm <= 24)
                {
                    test_mask = 0x01<<(test_fsm -16);
                    vSetLedOn(0x03,test_mask );

                }
                else
                    test_fsm = 0;
    }
    if (keymask)
    {
        vSetLedOn(0x01,keymask);
        vSetLedOn(0x02,keymask);
        vSetLedOn(0x03,keymask);
    }
}
/*
 *
 */
void vProcessTask( void * argument )
{
    static uint8_t data;
    static uint16_t test_counter = 0;
	for(;;)
	{
		/*Обработка событий от клавиатуры*/
		vTaskDelay(1);
		if ( uxQueueMessagesWaiting(pKeyboard) != 0)
		{
			xQueueReceive( pKeyboard, &TempEvent,portMAX_DELAY );
			switch (TempEvent.KeyCode)
			{
				case kl1_key:
				   key_mask = K1;
				   break;
				case kl2_key:
				   key_mask = K2;
			   	   break;
				case kl3_key:
				   key_mask = K3;
			   	   break;
				case kl4_key:
				   key_mask = K4;
 			   	   break;
				case kl5_key:
				   key_mask = K5;
   			   	   break;
				case kl6_key:
				   key_mask = K6;
			  	   break;
				case kl7_key:
				   key_mask = K7;
			   	   break;
				case kl8_key:
				   key_mask = K8;
			   	   break;
				default:
				   key_mask = 0U;
				   break;
			}
			if ( TempEvent.Status == MAKECODE )
			{
				data |= key_mask;
			}
			else
			{
				data &= ~key_mask;
			}
			OD_set_value(OD_ENTRY_H2000_digitalInputModuleKeysStates,0x01,&data,1,true);
			OD_requestTPDO(OD_KEY_flagsPDO,1);
		}
		if(  getReg8(TEST_START)!=0 )
		{
		    vTestFunction(&test_counter,data);
		}
	}
}




