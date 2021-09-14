/***************************************************************************//**
  @file     board_config.h
  @brief    Board management configuration
  @author   Grupo 4
 ******************************************************************************/

#ifndef _BOARD_CONFIG_H_
#define _BOARD_CONFIG_H_

// /*******************************************************************************
//  * INCLUDE HEADER FILES
//  ******************************************************************************/



// /*******************************************************************************
//  * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
//  ******************************************************************************/

// Choose Board for Pin config
#define FRDM            0
#define DJ_BOARD        1

#define BOARD           DJ_BOARD

// Convert port and number into pin ID
// Ex: PTB5  -> PORTNUM2PIN(PB,5)  -> 0x25
//     PTC22 -> PORTNUM2PIN(PC,22) -> 0x56
#define PORTNUM2PIN(p,n)    (((p)<<5) + (n))
#define PIN2PORT(p)         (((p)>>5) & 0x07)
#define PIN2NUM(p)          ((p) & 0x1F)

// Connection between FRDM and DJ_Board
// D = Digital, I = Input, O = Output, A = Active, H = High, L = Low, SIG = Signal

#if (BOARD == DJ_BOARD)
#define PIN_CSEGA         PORTNUM2PIN(PC,5)   // D.O - AH
#define PIN_CSEGB         PORTNUM2PIN(PC,7)   // D.O - AH
#define PIN_CSEGC         PORTNUM2PIN(PC,0)   // D.O - AH
#define PIN_CSEGD         PORTNUM2PIN(PC,9)   // D.O - AH
#define PIN_CSEGE         PORTNUM2PIN(PC,8)   // D.O - AH
#define PIN_CSEGF         PORTNUM2PIN(PC,1)   // D.O - AH
#define PIN_CSEGG         PORTNUM2PIN(PB,19)  // D.O - AH
#define PIN_CSEGDP        PORTNUM2PIN(PB,18)  // D.O - AH

#define PIN_SEL0          PORTNUM2PIN(PC,3)   // D.O - AH
#define PIN_SEL1          PORTNUM2PIN(PC,2)   // D.O - AH
#define PIN_RCHA          PORTNUM2PIN(PA,2)   // D.I - AL
#define PIN_RCHB          PORTNUM2PIN(PB,23)  // D.I - AL
#define PIN_RSWITCH       PORTNUM2PIN(PA,1)   // D.I - AL
#define PIN_STATUS0       PORTNUM2PIN(PB,9)   // D.O - AH
#define PIN_STATUS1       PORTNUM2PIN(PC,17)  // D.O - AH

#define PIN_MAG_EN        PORTNUM2PIN(PB,2)   // D.I - AL 
#define PIN_MAG_DT 	      PORTNUM2PIN(PB,3)   // D.I - AL
#define PIN_MAG_CLK	      PORTNUM2PIN(PB,10)  // D.I - AL

#define PIN_ISR_TEST      PORTNUM2PIN(PC,10)  // D.O - AH

#elif (BOARD == FRDM)
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
#endif

/*******************************************************************************
 ******************************************************************************/

#endif // _BOARD_CONFIG_H_
