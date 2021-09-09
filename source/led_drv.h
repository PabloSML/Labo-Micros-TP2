/***************************************************************************//**
  @file     led_drv.h
  @brief    LED Driver Header File
  @author   Grupo 4
 ******************************************************************************/

#ifndef _LED_DRV_H_
#define _LED_DRV_H_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>


/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

// Choose Board for Pin config
#define FRDM            0
#define DJ_BOARD        1

#define BOARD           DJ_BOARD

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

#if (BOARD == DJ_BOARD)
typedef enum
{
  LED_OFF               = 0x00,
	LED_1                 = 0x01,
	LED_2				          = 0x02,
	LED_3      			      = 0x03
  
} Led_t;
#endif

/*******************************************************************************
 * VARIABLE PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/**
 * @brief Initialize led_drv and corresponding peripheral
 * @return Initialization succeed
 */
bool ledInit(void);

// Non-Blocking services ////////////////////////////////////////////////

/**
 * @brief Turn On chosen LED
 * @return True if pressed, false if not pressed
 */
void ledOn(void);

/**
 * @brief Get button current position
 * @return True if pressed, false if not pressed
 */
void ledOff(void);

/**
 * @brief Get button current position
 * @return True if pressed, false if not pressed
 */
void ledToggle(void);

/**
 * @brief Get button current position
 * @return True if pressed, false if not pressed
 */
void ledBlink(void);

/*******************************************************************************
 ******************************************************************************/

#endif // _LED_DRV_H_
