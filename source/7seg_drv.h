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

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

typedef enum
{
  SSEG_1    = 0x00,
  SSEG_2    = 0x01,
  SSEG_3    = 0x02,
  SSEG_4    = 0x03,
  SSEG_MAX  = 0x04
} seven_seg_t;


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
 * @brief write character on 7 segment display
 * @param character to write, bool dpState true:on false:off, segment id
 */
void sevenSegWrite(uint8_t character,bool dpState, seven_seg_t id);



/*******************************************************************************
 ******************************************************************************/

#endif // _7SEG_DRV_H_
