/***************************************************************************//**
  @file     gpio_pdrv.c
  @brief    GPIO driver.
  @author   Grupo 4
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "gpio.h"
#include "hardware.h"
#include "MK64F12.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define GPIO_DEVELOPMENT_MODE 1

#define MUX_DEFAULT       	1
//#define IRQC_DEFAULT	  	0
#define PCR_PE_ENABLE	  	1
#define PORT_PCR_MUX_CLR  	~PORT_PCR_MUX_MASK
#define PORT_PCR_IRQC_CLR	~PORT_PCR_IRQC_MASK

#define PORT_CANT			5
#define PORT_WIDTH			32

#define BITCLR(x,pos) ((x) &= ((uint32_t)(~(((uint32_t)1) << (pos)))))
#define BITSET(x,pos) ((x) |= ((uint32_t)((((uint32_t)1) << (pos)))))

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

/*******************************************************************************
 * VARIABLES WITH GLOBAL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * ROM CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

static pinIrqFun_t pinIrq_callbacks[PORT_CANT][PORT_WIDTH];
static PORT_Type * portpointer[] = PORT_BASE_PTRS;
static GPIO_Type * gpiopointer[] = GPIO_BASE_PTRS;

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

/**
 * @brief Configures the specified pin to behave either as an input or an output
 * @param pin the pin whose mode you wish to set (according PORTNUM2PIN)
 * @param mode INPUT, OUTPUT, INPUT_PULLUP or INPUT_PULLDOWN.
 */
/*******************************************************/
void gpioMode (pin_t pin, uint8_t mode)
{
	uint32_t port = PIN2PORT(pin);
	uint32_t numpin = PIN2NUM(pin);

	uint32_t PCR_temp = portpointer[port]->PCR[numpin]; //RMW
	PCR_temp &= PORT_PCR_MUX_CLR; //Reinicio el MUX
	PCR_temp |= PORT_PCR_MUX(MUX_DEFAULT); //Cargo el MUX Default
	PCR_temp &= PORT_PCR_IRQC_CLR; //Reinicio IRQC
	PCR_temp |= PORT_PCR_IRQC(PORT_eDisabled);
	pinIrq_callbacks[port][numpin] = NULL;	// Elimino un callback si existia
	portpointer[port]->PCR[numpin] = PCR_temp; //Cargamos PCR temporal

	switch(mode)
	{
		case INPUT: //pin en 0 cofigura como input 
			BITCLR(gpiopointer[port]->PDDR,numpin);
			break;
		case OUTPUT://pin en 1 cofigura como output
			BITSET(gpiopointer[port]->PDDR,numpin);
			break;
		case INPUT_PULLUP: //pin PS de PCR en 1
			BITCLR(gpiopointer[port]->PDDR,numpin);//No deja de ser un INPUT
			portpointer[port]->PCR[numpin] |= PORT_PCR_PE(PCR_PE_ENABLE); //Habilitamos Pull Up
			portpointer[port]->PCR[numpin] |= PORT_PCR_PS(1);
			break;
		case INPUT_PULLDOWN: //pin PS de PCR en 0
			BITCLR(gpiopointer[port]->PDDR,numpin);//No deja de ser un INPUT
			portpointer[port]->PCR[numpin] |= PORT_PCR_PE(PCR_PE_ENABLE); //Habilitamos Pull Down
			portpointer[port]->PCR[numpin] &= ~PORT_PCR_PS(1);
			break;
	}
}

/**
 * @brief Configures how the pin reacts when an IRQ event ocurrs
 * @param pin the pin whose IRQ mode you wish to set (according PORTNUM2PIN)
 * @param irqMode disable, risingEdge, fallingEdge or bothEdges
 * @param irqFun function to call on pin event
 * @return Registration succeed
 */
bool gpioIRQ (pin_t pin, uint8_t irqMode, pinIrqFun_t irqFun)
{
	bool success = false;

#ifdef GPIO_DEVELOPMENT_MODE
    if ((irqFun) && (irqMode < GPIO_IRQ_CANT_MODES))
#endif // GPIO_DEVELOPMENT_MODE
	{
		uint32_t port = PIN2PORT(pin);
		uint32_t numpin = PIN2NUM(pin);

		pinIrq_callbacks[port][numpin] = irqFun;	// Asigno el callback

		uint32_t PCR_temp = portpointer[port]->PCR[numpin]; //RMW
		PCR_temp &= PORT_PCR_IRQC_CLR; //Reinicio IRQC
		PCR_temp |= PORT_PCR_IRQC(irqMode);
		portpointer[port]->PCR[numpin] = PCR_temp; //Cargamos PCR temporal

		success = true;
	}

	return success;
}

/**
 * @brief Write a HIGH or a LOW value to a digital pin
 * @param pin the pin to write (according PORTNUM2PIN)
 * @param val Desired value (HIGH or LOW)
 */
void gpioWrite (pin_t pin, bool value)
{
	uint32_t port = PIN2PORT(pin);
	uint32_t numpin = PIN2NUM(pin);
	// uint32_t pinmask = (uint32_t)(1 << numpin);

	if(value)
		BITSET(gpiopointer[port]->PDOR,numpin);
	else
		BITCLR(gpiopointer[port]->PDOR,numpin);
}

/**
 * @brief Toggle the value of a digital pin (HIGH<->LOW)
 * @param pin the pin to toggle (according PORTNUM2PIN)
 */
void gpioToggle (pin_t pin)
{
	uint32_t port = PIN2PORT(pin);
	uint32_t numpin = PIN2NUM(pin);
	uint32_t pinmask = (uint32_t)(1 << numpin);

	if(gpiopointer[port]->PDDR & pinmask) //Verifico que sea output
		gpiopointer[port]->PDOR ^= pinmask;
}

/**
 * @brief Reads the value from a specified digital pin, either HIGH or LOW.
 * @param pin the pin to read (according PORTNUM2PIN)
 * @return HIGH or LOW
 */
bool gpioRead (pin_t pin)
{
	uint32_t port = PIN2PORT(pin);
	uint32_t numpin = PIN2NUM(pin);
	uint32_t pinmask = (uint32_t)(1 << numpin);

	return (gpiopointer[port]->PDIR & pinmask);
}

/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/
/*
__ISR__ PORTA_IRQHandler (void)
{

	uint32_t ISFR_temp = portpointer[PA]->ISFR;



	pinIrqFun_t callback[PA];
#ifdef GPIO_DEVELOPMENT_MODE
    if (callback)
#endif // DEVELOPMENT_MODE
    {
        callback();
    }
}*/


/******************************************************************************/