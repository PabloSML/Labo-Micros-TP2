/***************************************************************************//**
  @file     button_drv.c
  @brief    Button driver source file.
  @author   Grupo 4
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "button_drv.h"
#include "gpio_pdrv.h"
#include "timer_drv.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define BUTTON_DEVELOPMENT_MODE 1

// Choose Board for Pin config
#define FRDM            0
#define DJ_BOARD        1

#define BOARD           DJ_BOARD

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
#define BUTTON_ISR_PERIOD      (10U*TIMER_SCALING)   // 10 ms    - Software Debouncing

#define LKP_THRESHOLD   ((300U*TIMER_SCALING)/(BUTTON_ISR_PERIOD))   // 300 ms   - Begin LKP state
#define TM_THRESHOLD    ((3000U*TIMER_SCALING)/(BUTTON_ISR_PERIOD))  // 3000 ms  - Begin TypeMatic state
#define TM_REPEAT       ((1000U*TIMER_SCALING)/(BUTTON_ISR_PERIOD))  // 1000 ms  - Repeat TypeMatic state


/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

typedef enum
{
	BUTTON_sReleased				= 0x00,
	BUTTON_sPressed         = 0x01
} ButtonState_t;

/*******************************************************************************
 * VARIABLES WITH GLOBAL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

/**
 * @brief Periodic service
 */
static void button_isr(void);

/*******************************************************************************
 * ROM CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

static tim_id_t timerId;
static ttick_t timerTicks = BUTTON_ISR_PERIOD;
static bool buttonPressed = false;
static ButtonState_t state = BUTTON_sReleased;
static ButtonEvent_t ev = BUTTON_noev;
static ttick_t counter = 0; // Counts succesive PRESSED states

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

bool buttonInit(void)
{
  static bool yaInit = false;
    
  if (!yaInit) // init peripheral
  {
    gpioMode(PIN_SW, SW_INPUT_TYPE);  // Set gpio connected to RSwitch as input
    timerInit();
    timerId = timerGetId();
    if(timerId != TIMER_INVALID_ID)
    {
      timerStart(timerId, timerTicks, TIM_MODE_PERIODIC, &button_isr);
      yaInit = true;
    }
  }

  return yaInit;
}

bool button_isPressed(void)
{
  return buttonPressed;
}

bool button_hasEvent(void)
{
  return ev;
}

ButtonEvent_t button_getEvent(void)
{
  ButtonEvent_t temp = ev;
  ev = BUTTON_noev;
  return temp;
}

/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

static void button_isr(void)
{
  buttonPressed = (gpioRead(PIN_SW) == SW_ACTIVE);

  switch (state)
  {
  case BUTTON_sPressed:
    if(!buttonPressed)
    {
      ev = BUTTON_eRelease;
      state = BUTTON_sReleased;
      counter = 0;
    }
    else
    {
      if(counter == LKP_THRESHOLD)
        ev = BUTTON_eLKP;
      else if (counter == TM_THRESHOLD)
        ev = BUTTON_eTypeMatic;
      else if (counter > TM_THRESHOLD && !((counter - TM_THRESHOLD) % TM_REPEAT))
        ev = BUTTON_eTypeMatic;
      counter++;
    }
    break;
  
  case BUTTON_sReleased:
    if(buttonPressed)
    {
      ev = BUTTON_ePress;
      state = BUTTON_sPressed;
    }
    break;

  default:
    break;
  }
}

/******************************************************************************/
