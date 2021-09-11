/***************************************************************************//**
  @file     logic_module.h
  @brief    Logic Module Header File
  @author   Grupo 4
 ******************************************************************************/

#ifndef _LOGIC_MODULE_H_
#define _LOGIC_MODULE_H_



/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "button_drv.h"
#include "led_drv.h"
#include "gpio_pdrv.h"
#include "magnetic_reader_drv.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/



/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

 typedef enum {
	LM_No_Event=0x00,
	LM_stay=0x01,
	LM_VALID_ID=0x02,
	LM_VALID_PIN=0X03,
	LM_INVALID_PIN=0X04,
	LM_INVALID_ID=0X05,
	LM_RESET=0X06
} LM_event_t;

/*******************************************************************************
 * VARIABLE PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/


/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

  /**
 * @brief Initialize the logic module and the drivers beneath 
 * @return bool, true if everything initialized correctly
 */
bool logic_module_init(void);


  /**
 * @brief Runs the logic module that talks to the drivers
 * @return void
 */
void run_logic_module(void);

 /**
 * @brief Query event
 * @return valid ID
 */
int get_ID(void);

/**
 * @brief Query event
 * @return valid PIN 
 */
int get_PIN(void);

/**
 * @brief Query event
 * @return True if event, false if not event
 */
bool logic_module_hasEvent(void);

/**
 * @brief Get encoder event
 * @return Return event of type LM_event_t 
 */
LM_event_t logic_module_getEvent(void);


/*******************************************************************************
 ******************************************************************************/

#endif // _LED_DRV_H_
