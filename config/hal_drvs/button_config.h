/***************************************************************************//**
  @file     button_config.h
  @brief    Button driver configuration file
  @author   Grupo 4
 ******************************************************************************/

#ifndef _BUTTON_CONFIG_H_
#define _BUTTON_CONFIG_H_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "board.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define BUTTON_DEVELOPMENT_MODE 0

// On Board User Switch
#if (BOARD == FRDM)
#define PIN_SW          PORTNUM2PIN(PA,4) // PTA4 SW3
#define SW_ACTIVE       LOW   // Physical Pull-Up Included
#define SW_INPUT_TYPE   INPUT
#elif (BOARD == DJ_BOARD)
#define PIN_SW          PORTNUM2PIN(PA,1) // PTA1 on FRDM Molex
#define SW_ACTIVE       LOW   // Physical Pull-Up Included
#define SW_INPUT_TYPE   INPUT
#endif

// Period for ISR
#define BUTTON_ISR_PERIOD      (10U*TIMER_SCALING)   // 10 ms - Software Debouncing

#define LKP_THRESHOLD   ((300U*TIMER_SCALING)/(BUTTON_ISR_PERIOD))   // 300 ms   - Begin LKP state
#define TM_THRESHOLD    ((3000U*TIMER_SCALING)/(BUTTON_ISR_PERIOD))  // 3000 ms  - Begin TypeMatic state
#define TM_REPEAT       ((1000U*TIMER_SCALING)/(BUTTON_ISR_PERIOD))  // 1000 ms  - Repeat TypeMatic state

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/


/*******************************************************************************
 ******************************************************************************/

#endif // _BUTTON_CONFIG_H_
