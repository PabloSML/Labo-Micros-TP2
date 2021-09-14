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




/*******************************************************************************
 ******************************************************************************/

#endif // _LED_DRV_H_
