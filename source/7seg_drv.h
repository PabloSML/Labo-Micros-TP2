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
#define DISPLAY_ISR_PERIOD      2U   


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
 * @brief print message on display. Maximo size de msg MAX_LENGTH_MESSAGE
 * @param char pointer to string
 * @param uint8_t size of msg
 */
void dispMSG(const char * new_msg, uint8_t size_msg);

/**
 * @brief set/clr decimal point
 * @param seven_seg_t id of display position
 * @param bool state of DP, true:ON, false:OFF
 */
void dispDP(seven_seg_t disp, bool state);

/**
 * @brief clear display
 */
void dispCLR(void);

/**
 * @brief move msg to right 
 */
void scrollRightMsg(void);

/**
 * @brief move msg to left 
 */
void scrollLeftMsg(void);


/*******************************************************************************
 ******************************************************************************/

#endif // _7SEG_DRV_H_
