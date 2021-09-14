/***************************************************************************//**
  @file     encoder_drv.h
  @brief    Encoder Driver Header File
  @author   Grupo 4
 ******************************************************************************/

#ifndef _ENCODER_CONFIG_H_
#define _ENCODER_CONFIG_H_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "board.h"
#include "timer_config.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define ENCODER_DEVELOPMENT_MODE 0

#if (BOARD == DJ_BOARD)
#define PIN_RCHA            PORTNUM2PIN(PA,2)   // D.I - SIG
#define PIN_RCHB            PORTNUM2PIN(PB,23)  // D.I - SIG
#define CHANNEL_ACTIVE      LOW
#define CH_INPUT_TYPE       INPUT               // Physical Pull-Up Included
#endif

// Period for ISR
#define ENCODER_ISR_PERIOD     (5U*TIMER_SCALING) // 5 ms - Software Debouncing

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/


/*******************************************************************************
 ******************************************************************************/

#endif // _ENCODER_CONFIG_H_
