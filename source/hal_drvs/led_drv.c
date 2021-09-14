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
#define LED_CANT        3
#endif

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

typedef enum
{
  OFF               = 0x00,   // Continually OFF
	ON                = 0x01,   // Continually ON
	BLINK      			  = 0x02,   // Blinking
  TIMEOUT           = 0x03    // On with timer
  
} led_state_t;

typedef struct
{
  led_state_t   state;
  ttick_t       led_timer;
  ttick_t       count;
  bool          isOn;         // Flag for blinking only         

} led_t;


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

/**
 * @brief Cycle through LED focus (LED1, LED2, LED3, LED1, ...)
 */
static void cycle_focus(void);

#if (BOARD == DJ_BOARD)
/**
 * @brief Select LED - Changes pins instead of state (OFF, LED1, LED2, LED3)
 * @param led Chosen LED
 */
static void selectLed(led_label_t led);
#endif

/*******************************************************************************
 * ROM CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

static tim_id_t timerId;
static ttick_t timerTicks = LED_ISR_PERIOD;
static led_t leds[LED_CANT];
static led_label_t focus = LED_1;

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
    selectLed(LED_OFF);               // Set gpio connected to Status as LOW (All LEDs OFF)
    gpioMode(LED_STATUS_0, OUTPUT);   // Set gpio connected to Status0 as output
    gpioMode(LED_STATUS_1, OUTPUT);   // Set gpio connected to Status1 as output
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

void ledOn(led_label_t led)
{
  leds[led].state = ON;
}


void ledOff(led_label_t led)
{
  leds[led].state = OFF;
}


void ledToggle(led_label_t led)
{
  if (leds[led].state != BLINK)
    leds[led].state = !(leds[led].state); // OFF <-> ON
}


void ledBlink(led_label_t led, uint32_t period)
{
  leds[led].state = BLINK;
  leds[led].led_timer = (period*TIMER_SCALING)/(LED_ISR_PERIOD*LED_CANT);
  leds[led].count = leds[led].led_timer;
}

void ledOn_timeout(led_label_t led, uint32_t duration)
{
  leds[led].state = TIMEOUT;
  leds[led].led_timer = duration/(LED_ISR_PERIOD*LED_CANT);
  leds[led].count = leds[led].led_timer;
}

/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

static void led_isr(void)
{
  switch (leds[focus].state)
  {
  case OFF:
    selectLed(LED_OFF);    
    break;
  
  case ON:
    selectLed(focus);
    break;
  
  case BLINK:
    if(!(--(leds[focus].count)))                            // Decremento el contador de ticks, termino?
    {
      leds[focus].isOn = !(leds[focus].isOn);               // Toggle blink state
      leds[focus].count = leds[focus].led_timer;         // Reset blink timer
    }
    selectLed((leds[focus].isOn)? (focus) : (LED_OFF));     // LED ON or OFF
    break;
  
  case TIMEOUT:
    if(!(--(leds[focus].count)))                            // Decremento el contador de ticks, termino?
    {
      leds[focus].state = OFF;                              // End TIMEOUT state
      selectLed(LED_OFF);                                   // LED OFF
    }
    else
      selectLed(focus);                                     // LED ON
    break;

  default:
    break;
  }

  cycle_focus();
}

static void cycle_focus(void)
{
  focus = (led_label_t)((focus + 1) % LED_CANT);
}

#if (BOARD == DJ_BOARD)
static void selectLed(led_label_t led)
{
  switch (led)
  {
  case LED_OFF:
    gpioWrite(LED_STATUS_0, LOW);
    gpioWrite(LED_STATUS_1, LOW);
    break;
  case LED_1:
    gpioWrite(LED_STATUS_0, HIGH);
    gpioWrite(LED_STATUS_1, LOW);
    break;
  case LED_2:
    gpioWrite(LED_STATUS_0, LOW);
    gpioWrite(LED_STATUS_1, HIGH);
    break;
  case LED_3:
    gpioWrite(LED_STATUS_0, HIGH);
    gpioWrite(LED_STATUS_1, HIGH);
    break;
  
  default:
    break;
  }
}
#endif

/******************************************************************************/
