/***************************************************************************//**
  @file     encoder_drv.c
  @brief    Encoder Driver Source File
  @author   Grupo 4
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "encoder_drv.h"
#include "gpio_pdrv.h"
#include "timer_drv.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/



/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

typedef enum
{
	ENCODER_sXX  				= 0x00,         // Both Channels Inactive
  ENCODER_sXB  				= 0x01,         // Only Channel B Active
  ENCODER_sAX  				= 0x02,         // Only Channel A Active
  ENCODER_sAB  				= 0x03         // Both Channels Active

} EncoderState_t;


/*******************************************************************************
 * VARIABLES WITH GLOBAL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

static void encoder_isr(void);
static EncoderState_t getState(void);

/*******************************************************************************
 * ROM CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

static tim_id_t timerId;
static ttick_t timerTicks = ENCODER_ISR_PERIOD;
static uint8_t rcha_active = !CHANNEL_ACTIVE;
static uint8_t rchb_active = !CHANNEL_ACTIVE;
static EncoderState_t oldState = ENCODER_sXX;
static EncoderState_t newState = ENCODER_sXX;
static EncoderEvent_t ev = ENCODER_noev;

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

bool encoderInit(void)
{
  static bool yaInit = false;
    
  if (!yaInit) // init peripheral
  {
    gpioMode(PIN_RCHA, CH_INPUT_TYPE);  // Set gpio connected to RCHA as input
    gpioMode(PIN_RCHB, CH_INPUT_TYPE);  // Set gpio connected to RCHB as input
    timerInit();
    timerId = timerGetId();
    if(timerId != TIMER_INVALID_ID)
    {
      timerStart(timerId, timerTicks, TIM_MODE_PERIODIC, &encoder_isr);
      yaInit = true;
    }
  }

  return yaInit;
}

bool encoder_hasEvent(void)
{
  return ev;
}

EncoderEvent_t encoder_getEvent(void)
{
  EncoderEvent_t temp = ev;
  ev = ENCODER_noev;
  return temp;
}

/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

static void encoder_isr(void)
{
  newState = getState();

  switch (newState)
  {
  case ENCODER_sXX:
    /* code */
    break;

  case ENCODER_sXB:
    if(oldState == ENCODER_sXX)
      ev = ENCODER_eRightTurn;
    break;
  
  case ENCODER_sAX:
    if(oldState == ENCODER_sXX)
      ev = ENCODER_eLeftTurn;
    break;
  
  case ENCODER_sAB:
    /* code */
    break;

  default:
    break;
  }

  oldState = newState;
}

static EncoderState_t getState(void)
{
  rcha_active = (gpioRead(PIN_RCHA) == CHANNEL_ACTIVE);       // Get Channel States
  rchb_active = (gpioRead(PIN_RCHB) == CHANNEL_ACTIVE);

  return (EncoderState_t)((rcha_active << 1) + rchb_active);
}

/******************************************************************************/
