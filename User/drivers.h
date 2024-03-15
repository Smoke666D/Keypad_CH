/*
 * drivers.h
 *
 *  Created on: Mar 9, 2024
 *      Author: i.dymov
 */

#ifndef USER_DRIVERS_H_
#define USER_DRIVERS_H_



#define KEYBOARD

#ifdef KEYBOARD
#include "keyboard.h"



#define KEY_COUNT           8
#define PARAM_CHECK         0
#define KEY_ON_STATE       ( RESET )
#define KEY_OFF_STATE      ( SET )

#endif

#endif /* USER_DRIVERS_H_ */
