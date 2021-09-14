/***************************************************************************//**
  @file     encoder_drv.h
  @brief    Encoder Driver Header File
  @author   Grupo 4
 ******************************************************************************/

#ifndef _ENCODER_DRV_H_
#define _ENCODER_DRV_H_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "encoder_config.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/


/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

typedef enum
{
  ENCODER_noev            = 0x00,
	ENCODER_eRightTurn      = 0x01,
	ENCODER_eLeftTurn				= 0x02
  
} EncoderEvent_t;

/*******************************************************************************
 * VARIABLE PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/**
 * @brief Initialize encoder_drv and corresponding peripheral
 * @return Initialization succeed
 */
bool encoderInit(void);

// Non-Blocking services ////////////////////////////////////////////////

/**
 * @brief Query event
 * @return True if event, false if not event
 */
bool encoder_hasEvent(void);

/**
 * @brief Get encoder event
 * @return Right Turn, Left Turn
 */
EncoderEvent_t encoder_getEvent(void);

/*******************************************************************************
 ******************************************************************************/

#endif // _ENCODER_DRV_H_
