/***************************************************************************//**
  @file     App.c template
  @brief    Application functions
  @author   Grupo 4
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/
#include "board.h"
#include "button_drv.h"
#include "hardware.h"
#include "MK64F12.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/


/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

static void cycle_led_color(void);

/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

static ButtonEvent_t prevButtonEv = BUTTON_noev;
static ButtonEvent_t newButtonEv = BUTTON_noev;
static bool led_on = false;
static enum led_color_t led_color = RED;

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
  SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK;
  SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK;
  SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK;

  // PORT_Type * portpointer[] = PORT_BASE_PTRS;
  // portpointer[PA]->ISFR |= 0xFFFFU;
  
  // NVIC_EnableIRQ(PORTA_IRQn);

  ledInit(LED_RED);
  ledInit(LED_GREEN);
  ledInit(LED_BLUE);
  // switchInit(SW3);
  buttonInit();

  // irq_id_t id = irqGetId(SW3);
  // gpioIRQ(SW3, PORT_eInterruptFalling, id, &ledToggle);

  hw_EnableInterrupts();

}

/* Función que se llama constantemente en un ciclo infinito */
void App_Run (void)
{
  if(button_hasEvent())
  {
    newButtonEv = button_getEvent();

    switch (newButtonEv)
    {
    case BUTTON_ePress:
      /* Act on release... */
      break;
    
    case BUTTON_eRelease:
      if(prevButtonEv == BUTTON_ePress)
      {
        ledToggle(led_color);
        led_on = !led_on;
      }
      else if (prevButtonEv == BUTTON_eLKP)
      {
        if(led_on)
          cycle_led_color();
      }
      break;

    case BUTTON_eLKP:
      /* Act on release... */
      break;

    case BUTTON_eTypeMatic:
      {
        if(led_on)
          cycle_led_color();
      }
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

static void cycle_led_color(void)
{
  switch (led_color) 
  {
  case RED:
    if(led_on)
    {
      ledOff(LED_RED);
      ledOn(LED_GREEN);
    }
    led_color = GREEN;
    break;
  case GREEN:
    if(led_on)
    {
      ledOff(LED_GREEN);
      ledOn(LED_BLUE);
    }
    led_color = BLUE;
    break;
  case BLUE:
    if(led_on)
    {
      ledOff(LED_BLUE);
      ledOn(LED_RED);
    }
    led_color = RED;
    break;
  default:
    break;
  }
}

/*******************************************************************************
 ******************************************************************************/
