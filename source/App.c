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


// APP DEVELOPMENT MODE
#define APP_DEVELOPMENT_MODE 0



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

typedef enum {WAIT_ID, WAIT_PIN, BLOCK, UNLOCK, INTENSITY} state_t;

typedef enum {STAY, INPUT_ID, VALID_PIN, INVALID_PIN} event_t;

static state_t state;
static event_t event;

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

  //Enable clocking for port B,A,E
  SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK;
  SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK;
  SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK;
  SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK;

  // Start 7-seg in OFF
  gpioWrite(PIN_CSEGA, LOW);
  gpioWrite(PIN_CSEGB, LOW);
  gpioWrite(PIN_CSEGC, LOW);
  gpioWrite(PIN_CSEGD, LOW);
  gpioWrite(PIN_CSEGE, LOW);
  gpioWrite(PIN_CSEGF, LOW);
  gpioWrite(PIN_CSEGG, LOW);
  gpioWrite(PIN_CSEGDP, LOW);

  gpioWrite(PIN_SEL0, LOW);
  gpioWrite(PIN_SEL1, LOW);

  // Set 7-seg pins as Out
  gpioMode(PIN_CSEGA, OUTPUT);
  gpioMode(PIN_CSEGB, OUTPUT);
  gpioMode(PIN_CSEGC, OUTPUT);
  gpioMode(PIN_CSEGD, OUTPUT);
  gpioMode(PIN_CSEGE, OUTPUT);
  gpioMode(PIN_CSEGF, OUTPUT);
  gpioMode(PIN_CSEGG, OUTPUT);
  gpioMode(PIN_CSEGDP, OUTPUT);

  gpioMode(PIN_SEL0, OUTPUT);
  gpioMode(PIN_SEL1, OUTPUT);

  // // Start LEDs OFF
  // gpioWrite(PIN_STATUS0, LOW);
  // gpioWrite(PIN_STATUS1, LOW);

  // // Set LED pins as Out
  // gpioMode(PIN_STATUS0, OUTPUT);
  // gpioMode(PIN_STATUS1, OUTPUT);

  // Set Encoder Inputs
  // gpioMode(PIN_RCHA, INPUT);
  // gpioMode(PIN_RCHB, INPUT);
  // gpioMode(PIN_RSWITCH, INPUT);

  // PORT_Type * portpointer[] = PORT_BASE_PTRS;
  // portpointer[PA]->ISFR |= 0xFFFFU;

  // NVIC_EnableIRQ(PORTA_IRQn);

  // Inits for FRDM
  // ledInit(LED_RED);
  // ledInit(LED_GREEN);
  // ledInit(LED_BLUE);
  // switchInit(SW3);

  // Inits for DJ_BOARD
  ledInit();
  ledOn(LED_1);
  buttonInit();
  encoderInit();


  // irq_id_t id = irqGetId(SW3);
  // gpioIRQ(SW3, PORT_eInterruptFalling, id, &ledToggle);

  hw_EnableInterrupts();


}

/* Función que se llama constantemente en un ciclo infinito */
void App_Run (void)
{

	if(!APP_DEVELOPMENT_MODE)
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
	else
	{
		 switch(state){
		    case WAIT_ID:
		        //event = ;//fsm del input id (debe devolver INPUT_ID si se ingreso ID, sino STAY)
		        if(event == INPUT_ID){
		            state = WAIT_PIN;
		        }
		        break;
		    case WAIT_PIN:
		        //event = ;//fsm del input id (debe devolver VALID_PIN o INVALID_PIN si se ingreso PIN, sino STAY)
		        if(event == VALID_PIN){
		            state = UNLOCK;
		        }
		        else if (event == INVALID_PIN){
		            state = BLOCK;
		        }
		        break;
		    case UNLOCK:
		        //show leds
		        //wait 5 sec
		        //state = WAIT_ID; //Back to start
		        break;
		    case BLOCK:
		        //block for 5 sec - No lo veo necesario para ahora
		        break;
		    case INTENSITY:
		        //if(intensity set){ state = WAIT_ID}
		        break;

		 }
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
