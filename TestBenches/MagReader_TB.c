/***************************************************************************//**
  @file     App.c template
  @brief    Application functions
  @author   Grupo 4
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/
// #include "board.h"
#include "button_drv.h"
#include "led_drv.h"
#include "encoder_drv.h"
#include "gpio_pdrv.h"
#include "hardware.h"
#include "MK64F12.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

// Connection between FRDM and DJ_Board (Here just for developement)
// D = Digital, I = Input, O = Output, A = Active, H = High, L = Low, SIG = Signal

#define PIN_CSEGA         PORTNUM2PIN(PC,15)  // D.O - AH
#define PIN_CSEGB         PORTNUM2PIN(PC,7)   // D.O - AH
#define PIN_CSEGC         PORTNUM2PIN(PC,0)   // D.O - AH
#define PIN_CSEGD         PORTNUM2PIN(PC,9)   // D.O - AH
#define PIN_CSEGE         PORTNUM2PIN(PC,8)   // D.O - AH
#define PIN_CSEGF         PORTNUM2PIN(PC,1)   // D.O - AH
#define PIN_CSEGG         PORTNUM2PIN(PB,19)  // D.O - AH
#define PIN_CSEGDP        PORTNUM2PIN(PB,18)  // D.O - AH

#define PIN_SEL0          PORTNUM2PIN(PC,3)   // D.O - AH
#define PIN_SEL1          PORTNUM2PIN(PC,2)   // D.O - AH
#define PIN_RCHA          PORTNUM2PIN(PA,2)   // D.I - SIG
#define PIN_RCHB          PORTNUM2PIN(PB,23)  // D.I - SIG
#define PIN_RSWITCH       PORTNUM2PIN(PA,1)   // D.I - AL
#define PIN_STATUS0       PORTNUM2PIN(PB,9)   // D.O - AH
#define PIN_STATUS1       PORTNUM2PIN(PC,17)  // D.O - AH

#define PIN_MAG_EN        PORTNUM2PIN(PB,2) //
#define PIN_MAG_DT 	      PORTNUM2PIN(PB,3) //
#define PIN_MAG_CLK	      PORTNUM2PIN(PB,10) //
/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

// static void cycle_led_color(void);

/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

static ButtonEvent_t prevButtonEv = BUTTON_noev;
static ButtonEvent_t newButtonEv = BUTTON_noev;
static EncoderEvent_t encoderEv = ENCODER_noev;
static led_label_t oldFocus = LED_1;
static led_label_t newFocus = LED_1;
// static bool led_on = false;
// static enum led_color_t led_color = RED;

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

/* Función que se llama 1 vez, al comienzo del programa */
void App_Init (void)
{
  hw_DisableInterrupts();

  magneticReaderInit();
  gpioMode(PIN_LED_GREEN, OUTPUT);
  gpioToggle(PIN_LED_RED);
  gpioToggle(PIN_LED_GREEN);
  gpioMode(PIN_LED_RED, OUTPUT);

  hw_EnableInterrupts();


}

/* Función que se llama constantemente en un ciclo infinito */
void App_Run (void)
{
  if (encoder_hasEvent())
  {

    encoderEv = encoder_getEvent();

    switch (encoderEv)
    {
    case BUTTON_ePress:
      /* Act on release... */
      break;

    case BUTTON_eRelease:
      if(prevButtonEv == BUTTON_ePress)
      {
        ledToggle(LED_1);
        // led_on = !led_on;
      }
      else if (prevButtonEv == BUTTON_eLKP)
      {
        // if(led_on)
        //   cycle_led_color();
        ledBlink(LED_1, 500U);
        ledBlink(LED_2, 1000U);
        ledBlink(LED_3, 100U);
      }
      break;

    case ENCODER_eRightTurn:
      newFocus = (led_label_t)((oldFocus + 2) % 3);
      break;

    case ENCODER_eLeftTurn:
      newFocus = (led_label_t)((oldFocus + 1) % 3);
      break;

    default:
      break;
    }

    prevButtonEv = newButtonEv;

  }

}


/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

// static void cycle_led_color(void)
// {
//   switch (led_color)
//   {
//   case RED:
//     if(led_on)
//     {
//       ledOff(LED_RED);
//       ledOn(LED_GREEN);
//     }
//     led_color = GREEN;
//     break;
//   case GREEN:
//     if(led_on)
//     {
//       ledOff(LED_GREEN);
//       ledOn(LED_BLUE);
//     }
//     led_color = BLUE;
//     break;
//   case BLUE:
//     if(led_on)
//     {
//       ledOff(LED_BLUE);
//       ledOn(LED_RED);
//     }
//     led_color = RED;
//     break;
//   default:
//     break;
//   }
// }

/*******************************************************************************
 ******************************************************************************/
