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

#define PIN_CSEGA         PORTNUM2PIN(PC,5)  // D.O - AH
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

#define PIN_MAG_EN        PORTNUM2PIN(PB,2) //  DEBUGGER
#define PIN_MAG_DT 	      PORTNUM2PIN(PB,3) //  DEBUGGER
#define PIN_MAG_CLK	      PORTNUM2PIN(PB,10) // DEBUGGER
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
static uint8_t len;
static uint8_t* pan;
static uint8_t ID_array[8];
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

   PORT_Type * portpointer[] = PORT_BASE_PTRS;
   portpointer[PB]->ISFR |= 0xFFFFU;

   NVIC_EnableIRQ(PORTB_IRQn);

   // Inits for DJ_BOARD
   gpioMode(LED_RED,output);
   gpioMode(LED_BLUE,output);
   gpioMode(LED_GREEN,output);
   gpioWrite(LED_RED,HIGH);
   gpioWrite(LED_BLUE,HIGH);
   gpioWrite(LED_GREEN,HIGH);
   magneticReaderInit();
   hw_EnableInterrupts();
}

/* Función que se llama constantemente en un ciclo infinito */
void App_Run (void)
{
  if (magreader_hasEvent())
  {

    magreaderEv = magreader_getEvent();

    switch (magreaderEv)
    {
      case MAGREADER_noev:
        break;
      case MAGREADER_cardsliding:
        gpioWrite(LED_BLUE,LOW);
        gpioWrite(LED_RED,HIGH);
        gpioWrite(LED_GREEN,HIGH);
        break;
      case MAGREADER_carderror:
        gpioWrite(LED_BLUE,HIGH);
        gpioWrite(LED_RED,LOW);
        gpioWrite(LED_GREEN,HIGH);
        break;
      case MAGREADER_cardUpload:
        gpioWrite(LED_BLUE,HIGH);
        gpioWrite(LED_RED,HIGH);
        gpioWrite(LED_GREEN,LOW);
        uint8_t * pan = getPAN();
        for(uint8_t i = 0; i < 8; i++){
          ID_array[i] = pan[i];
        break;
    }
  }

}

/*******************************************************************************
 ******************************************************************************/
