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
#include "led_config.h"


/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/


/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

#if (BOARD == DJ_BOARD)
typedef enum
{
	LED_1                 = 0x00,
	LED_2				          = 0x01,
	LED_3      			      = 0x02,
  LED_OFF               = 0x03
  
} led_label_t;
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
 * @param led Chosen LED
 */
void ledOn(led_label_t led);

/**
 * @brief Turn Off chosen LED
 * @param led Chosen LED
 */
void ledOff(led_label_t led);

/**
 * @brief Toggle chosen LED
 * @param led Chosen LED
 */
void ledToggle(led_label_t led);

/**
 * @brief Blink chosen LED
 * @param led Chosen LED
 * @param period Blinking period (ms - Multiple of LED_ISR_PERIOD*LED_CANT)
 */
void ledBlink(led_label_t led, uint32_t period);

/**
 * @brief Turn on chosen LED for duration
 * @param led Chosen LED
 * @param duration Duration of ON Time (ms - Multiple of LED_ISR_PERIOD*LED_CANT)
 */
void ledOn_timeout(led_label_t led, uint32_t duration);

/*******************************************************************************
 ******************************************************************************/

#endif // _LED_DRV_H_
