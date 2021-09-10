/***************************************************************************//**
  @file     display_drv.h
  @brief    DISPLAY Driver Header File
  @author   Grupo 4
 ******************************************************************************/

#ifndef _DISPLAY_DRV_H_
#define _DISPLAY_DRV_H_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>


/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

// Period for ISR
#define DISPLAY_ISR_PERIOD      10U   // 20 ms

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
 * @brief Initialize display and corresponding peripheral
 * @return Initialization succeed
 */
bool displayInit(void);

// Non-Blocking services

/**
 * @brief print message on display
 * @param receive char pointer and bool scrolling
 */
void dispMSG(const char * string, bool scrolling);

/**
 * @brief clear display
 */
void dispCLR(void);


/*******************************************************************************
 ******************************************************************************/

#endif // _DISPLAY_DRV_H_
