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

// Period for ISR
#define DISPLAY_ISR_PERIOD      2U   // 20 ms

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

typedef enum
{
  DISP_1    = 0x00,
  DISP_2    = 0x01,
  DISP_3    = 0x02,
  DISP_4    = 0x03,
  DISP_CANT  = 0x04 
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
 * @brief print message on display
 * @param receive char pointer
 */
void dispMSG(const char newMsg[]);

/**
 * @brief clear display
 */
void dispCLR(void);


/*******************************************************************************
 ******************************************************************************/

#endif // _7SEG_DRV_H_
