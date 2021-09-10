/***************************************************************************//**
  @file     App.c template
  @brief    Application functions
  @author   Grupo 4
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "led_drv.h"
#include "hardware.h"
#include "7seg_drv.h"
#include "MK64F12.h"
#include "gpio_pdrv.h"
#include "button_drv.h"
#include <time.h>

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/


/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

static void nextChar(void);
static void delay(uint32_t veces);

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

/* Función que se llama 1 vez, al comienzo del programa */
void App_Init (void)
{
  hw_DisableInterrupts();

  sevenSegInit();

  //Enable clocking for port B,A,E
  SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK;
  SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK;
  SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK;

  PORT_Type * portpointer[] = PORT_BASE_PTRS;
  portpointer[PA]->ISFR |= 0xFFFFU;

  NVIC_EnableIRQ(PORTA_IRQn);

  //ledInit(LED_RED);
  buttonInit();
  //switchInit(SW3);

  //irq_id_t id = irqGetId(SW3);
  //gpioIRQ(SW3, PORT_eInterruptFalling, id, &nextChar);
  hw_EnableInterrupts();

}

/* Función que se llama constantemente en un ciclo infinito */
void App_Run (void)
{
	static const uint8_t *msg[] = {'H','O','L','A'};
	static uint8_t i = 0;
	if(button_hasEvent())
	{
		button_getEvent();
		if(i>4)
			i=0;

		sevenSegWrite(msg[i++],false,SSEG_1);
		delay(10000000UL);
	}
}


/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

static void nextChar(void)
{
	static const uint8_t *msg[] = {'H','O','L','A'};
	static uint8_t i = 0;

	if(i>4)
		i=0;

	sevenSegWrite(msg[i],false,SSEG_1);
}

static void delay(uint32_t veces)
{
	while(veces--);
}


/*******************************************************************************
 ******************************************************************************/
