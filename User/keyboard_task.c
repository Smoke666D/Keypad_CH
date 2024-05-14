/*
 * keyboard_task.c
 *
 *  Created on: May 14, 2024
 *      Author: i.dymov
 */
#include "keyboard_task.h"
#include "hw_lib_keyboard.h"

static TaskHandle_t  pKeybardTaskHandle    __SECTION(RAM_SECTION_CCMRAM);
static uint8_t  STATUS[KEY_COUNT];
static uint8_t  COUNTERS[KEY_COUNT];

TaskHandle_t * xKeyboardProcessTaskHandle ()
{
    return  &pKeybardTaskHandle ;
}


BitState_t fPortState (uint8_t i)
{
    switch (i)
    {
        case 0:
            return HAL_GetBit( KL1_Port, KL1Pin  );
        case 1:
            return HAL_GetBit( KL2_8_Port, KL2Pin  );
        case 2:
            return HAL_GetBit( KL2_8_Port, KL3Pin  );
        case 3:
            return HAL_GetBit( KL2_8_Port, KL4Pin  );
        case 4:
            return HAL_GetBit( KL2_8_Port, KL5Pin  );
        case 5:
            return HAL_GetBit( KL2_8_Port, KL6Pin  );
        case 6:
            return HAL_GetBit( KL2_8_Port, KL7Pin  );
        default:
            return 0;
    }
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


void vKeyboardTask( void * argument )
{
  vInitKeybord();
  for(;;)
  {
    vTaskDelay( HW_LIB_GetKeyboardPeriod() );
    HW_LIB_KeyboradFSM();
  }
}
