/***************************************************************************//**
  @file     button_drv.h
  @brief    Button driver header file
  @author   Grupo 4
 ******************************************************************************/

#ifndef _BUTTON_DRV_H_
#define _BUTTON_DRV_H_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "button_config.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/


/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

typedef enum
{
  BUTTON_noev           = 0x00,
	BUTTON_ePress         = 0x01,
	BUTTON_eRelease				= 0x02,
	BUTTON_eLKP      			= 0x03,
	BUTTON_eTypeMatic		  = 0x04
  
} ButtonEvent_t;

/*******************************************************************************
 * VARIABLE PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/**
 * @brief Initialize button_drv and corresponding peripheral
 * @return Initialization succeed
 */
bool buttonInit(void);

// Non-Blocking services ////////////////////////////////////////////////

/**
 * @brief Get button current position
 * @return True if pressed, false if not pressed
 */
bool button_isPressed(void);

/**
 * @brief Query event
 * @return True if event, false if not event
 */
bool button_hasEvent(void);

/**
 * @brief Get button event
 * @return NOEV, PRESS, RELEASE, LKP, TYPEMATIC
 */
ButtonEvent_t button_getEvent(void);

// Blocking services ////////////////////////////////////////////////

/*******************************************************************************
 ******************************************************************************/

#endif // _BUTTON_DRV_H_
