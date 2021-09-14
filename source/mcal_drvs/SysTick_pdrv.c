/***************************************************************************//**
  @file     SysTick.c
  @brief    SysTick driver
  @author   Nicolás Magliola
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "SysTick_pdrv.h"
#include "hardware.h"


/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define DEVELOPMENT_MODE    0

#define SYSTICK_LOAD_INIT   ((__CORE_CLOCK__/SYSTICK_ISR_FREQUENCY_HZ) - 1U)

#if SYSTICK_LOAD_INIT > (1<<24)
#error Overflow de SysTick! Ajustar  __CORE_CLOCK__ y SYSTICK_ISR_FREQUENCY_HZ!
#endif // SYSTICK_LOAD_INIT > (1<<24)


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

static systck_callback_t systick_callback;

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

bool SysTick_Init (systck_callback_t funcallback)
{
    static bool yaInit = false;
#if DEVELOPMENT_MODE
    if (!yaInit && funcallback)
#endif // DEVELOPMENT_MODE
    {
        SysTick->CTRL = 0x00;
        SysTick->LOAD = SYSTICK_LOAD_INIT;
        SysTick->VAL  = 0x00;
        SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk;

        systick_callback = funcallback;
        yaInit = true;
    }
    return yaInit;
}



/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

__ISR__ SysTick_Handler (void)
{
#if DEVELOPMENT_MODE
    if (systick_callback)
#endif // DEVELOPMENT_MODE
    {
        systick_callback();
    }
}

/******************************************************************************/
