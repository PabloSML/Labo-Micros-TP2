/***************************************************************************//**
  @file     timer_config.h
  @brief    Timer driver configurations
  @author   Grupo 4
 ******************************************************************************/

#ifndef _TIMER_CONFIG_H_
#define _TIMER_CONFIG_H_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "SysTick_config.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define TIMER_DEVELOPMENT_MODE      0

#define TIMER_SCALING       ((SYSTICK_ISR_FREQUENCY_HZ)/(1000U))
#define TIMER_TICK_MS       ((1.0)/(TIMER_SCALING))
#define TIMER_MS2TICKS(ms)  ((ms)/TIMER_TICK_MS)

#define TIMERS_MAX_CANT     16
#define TIMER_INVALID_ID    255

/*******************************************************************************
 ******************************************************************************/

#endif // _TIMER_CONFIG_H_
