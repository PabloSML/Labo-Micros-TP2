/***************************************************************************//**
  @file     board.h
  @brief    Board management
  @author   Nicolás Magliola
 ******************************************************************************/

#ifndef _BOARD_H_
#define _BOARD_H_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "gpio_pdrv.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

/***** BOARD defines **********************************************************/

// On Board User LEDs
#define PIN_LED_RED     PORTNUM2PIN(PB,22) // PTB22
#define PIN_LED_GREEN   PORTNUM2PIN(PE,26) // PTE26
#define PIN_LED_BLUE    PORTNUM2PIN(PB,21) // PTB21

// Definitions for app
#define LED_RED			PIN_LED_RED
#define LED_GREEN		PIN_LED_GREEN
#define LED_BLUE		PIN_LED_BLUE

#define LED_ACTIVE      LOW

// On Board User Switches
#define PIN_SW2         PORTNUM2PIN(PC,6) // PTC6
#define PIN_SW3         PORTNUM2PIN(PA,4) // PTA4

#define SW2				PIN_SW2
#define	SW3				PIN_SW3

#define SW_ACTIVE       LOW
#define SW_INPUT_TYPE   INPUT


/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

enum led_color_t {RED = LED_RED, GREEN = LED_GREEN, BLUE = LED_BLUE};

typedef pin_t led_t;
typedef pin_t sw_t;

/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/*********** LED init & services ****************/
/**
 * @brief Initialize LED
 */
void ledInit(led_t led_color);
/**
 * @brief LED Off
 */
void ledOff(led_t led_color);
/**
 * @brief LED Off
 */
void ledOn(led_t led_color);
/**
 * @brief LED Off
 */
void ledToggle(led_t led_color);


/*********** Switch init & services ****************/
/**
 * @brief Initialize switch
 */
void switchInit(sw_t sw_number);

/*******************************************************************************
 ******************************************************************************/

#endif // _BOARD_H_
