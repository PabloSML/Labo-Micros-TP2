/***************************************************************************//**
  @file     7seg_drv.h
  @brief    Seven Segment Driver Header File
  @author   Grupo 4
 ******************************************************************************/

#ifndef _7SEG_DRV_H_
#define _7SEG_DRV_H_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "7seg_config.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/


/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

typedef enum
{
  DISP_1    = 0x00,
  DISP_2    = 0x01,
  DISP_3    = 0x02,
  DISP_4    = 0x03,
  DISP_CANT
} seven_seg_label_t;

typedef enum
{
  MIN    = 2,
  MID    = 4,
  MAX    = 6
} brightness_label_t;

/*******************************************************************************
 * VARIABLE PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/**
 * @brief Initialize 7seg_drv and corresponding peripheral
 * @return Initialization succeed
 */
bool sevenSegInit(void);

// Non-Blocking services

/**
 * @brief move msg to right 
 */
void scrollRightMsg(void);

/**
 * @brief move msg to left 
 */
void scrollLeftMsg(void);

/**
 * @brief print message on display. Maximo size de msg MAX_LENGTH_MESSAGE
 * @param new_msg char pointer to string
 * @param size_msg size of string
 */
void dispMSG(const char * new_msg, uint8_t size_msg);

/**
 * @brief set/clr decimal point
 * @param disp Chosen 7Segment display
 * @param state of DP, true:ON, false:OFF
 */
void dispDP(seven_seg_label_t disp, bool state);

/**
 * @brief clear display
 */
void dispCLR(void);

/**
 * @brief Turn On chosen 7Segment display
 * @param disp Chosen 7Segment display
 */
void dispOn(seven_seg_label_t disp);

/**
 * @brief Turn Off chosen 7Segment display
 * @param disp Chosen 7Segment display
 */
void dispOff(seven_seg_label_t disp);

/**
 * @brief Blink chosen 7Segment display
 * @param disp Chosen 7Segment display
 * @param period Blinking period (ms - Multiple of DISPLAY_ISR_PERIOD*DISP_CANT)
 */
void dispBlink(seven_seg_label_t disp, uint32_t period);

/**
 * @brief Toggle chosen 7Segment display
 * @param disp Chosen 7Segment display
 */
void dispToggle(seven_seg_label_t disp);

/**
 * @brief set bright for 7Segment display
 * @param level Chosen bright 7Segment display
 */
void setBright(brightness_label_t level);


/*******************************************************************************
 ******************************************************************************/

#endif // _7SEG_DRV_H_
