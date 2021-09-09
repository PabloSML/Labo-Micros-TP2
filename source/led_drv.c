/***************************************************************************//**
  @file     led_drv.c
  @brief    LED Driver Source File
  @author   Grupo 4
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "led_drv.h"
#include "gpio_pdrv.h"
#include "timer_drv.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define LED_DEVELOPMENT_MODE 1

// On Board User LEDs
#if (BOARD == FRDM)
#define PIN_LED_RED     PORTNUM2PIN(PB,22) // PTB22
#define PIN_LED_GREEN   PORTNUM2PIN(PE,26) // PTE26
#define PIN_LED_BLUE    PORTNUM2PIN(PB,21) // PTB21
#define LED_ACTIVE      LOW
#elif (BOARD == DJ_BOARD)
#define LED_STATUS_0    PORTNUM2PIN(PB,9) // PTB9 on FRDM Molex
#define LED_STATUS_1    PORTNUM2PIN(PC,17) // PTC17 on FRDM Molex
#define LED_ACTIVE      LOW
#endif

// Period for ISR
#define ISR_PERIOD      100U   // 100 ms

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/



/*******************************************************************************
 * VARIABLES WITH GLOBAL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

/**
 * @brief Periodic service
 */
static void led_isr(void);

/*******************************************************************************
 * ROM CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

static tim_id_t timerId;
static ttick_t timerTicks = ISR_PERIOD;

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

bool ledInit(void)
{
  static bool yaInit = false;
    
  if (!yaInit) // init peripheral
  {
    gpioMode(LED_STATUS_0, OUTPUT);  // Set gpio connected to Status0 as output
    gpioMode(LED_STATUS_1, OUTPUT);  // Set gpio connected to Status1 as output
    timerInit();
    timerId = timerGetId();
#ifdef LED_DEVELOPMENT_MODE
    if(timerId != TIMER_INVALID_ID)
#endif
    {
      timerStart(timerId, timerTicks, TIM_MODE_PERIODIC, &led_isr);
      yaInit = true;
    }
  }

  return yaInit;
}


/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

static void led_isr(void)
{

}

/******************************************************************************/
