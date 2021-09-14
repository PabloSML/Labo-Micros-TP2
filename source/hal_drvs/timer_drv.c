/***************************************************************************//**
  @file     timer_drv.c
  @brief    Timer driver. Advanced implementation
  @author   Nicolás Magliola
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "board.h"
#include "timer_drv.h"
#include "SysTick_pdrv.h"
#include "gpio_pdrv.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define TIMER_ID_INTERNAL           0

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

typedef struct {
	ttick_t             period;
	ttick_t             cnt;
    tim_callback_t      callback;
    uint8_t             mode        : 1;
    uint8_t             running     : 1;
    uint8_t             expired     : 1;
    uint8_t             unused      : 5;
} timer_t;


/*******************************************************************************
 * VARIABLES WITH GLOBAL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

/**
 * @brief Periodic service
 */
static void timer_isr(void);
/**
 * @brief Reset timer count
 */
static void timerResetCnt(tim_id_t id);
/**
 * @brief Update timer status
 */
static void update(tim_id_t id);

/*******************************************************************************
 * ROM CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

static timer_t timers[TIMERS_MAX_CANT];
static tim_id_t timers_cant = TIMER_ID_INTERNAL+1;


/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

bool timerInit(void)
{
    static bool yaInit = false;
    if (!yaInit && SysTick_Init(timer_isr)) // init peripheral
    {
        gpioMode(PIN_ISR_TEST, OUTPUT);
        yaInit = true;
    }
    return yaInit;
}


tim_id_t timerGetId(void)
{
#if TIMER_DEVELOPMENT_MODE
    if (timers_cant >= TIMERS_MAX_CANT)
    {
        return TIMER_INVALID_ID;
    }
    else
#endif // TIMER_DEVELOPMENT_MODE
    {
        return timers_cant++;
    }
}


void timerStart(tim_id_t id, ttick_t ticks, uint8_t mode, tim_callback_t callback)
{
#if TIMER_DEVELOPMENT_MODE
    if ((id < timers_cant) && (mode < CANT_TIM_MODES))
#endif // TIMER_DEVELOPMENT_MODE
    {
        timers[id].running = false; // disable timer
        timers[id].period = ticks;  // configure timer
        timers[id].cnt = ticks;
        timers[id].callback = callback;
        timers[id].mode = mode;
        timers[id].expired = false;
        timers[id].running = true;  // enable timer
    }
}


void timerStop(tim_id_t id)
{
    timers[id].running = false; // Apago el timer y bajo el flag
    timerResetCnt(id);  // Reseteo la cuenta para que empiece de vuelta luego
}


bool timerExpired(tim_id_t id)
{
    // Verifico si expiró el timer
    // y bajo el flag
    bool expired = timers[id].expired;
    timers[id].expired = false;
    return expired; 
}


void timerDelay(ttick_t ticks)
{
    timerStart(TIMER_ID_INTERNAL, ticks, TIM_MODE_SINGLESHOT, NULL);
    while (!timerExpired(TIMER_ID_INTERNAL))
    {
        // wait...
    }
}

/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

static void timer_isr(void)
{
    gpioWrite(PIN_ISR_TEST, HIGH);
    for(tim_id_t id = TIMER_ID_INTERNAL; id < timers_cant; id++)
    {
        if(timers[id].running)  // si el timer esta activo
        {
            if(!(--timers[id].cnt))     // decremento el contador de ticks, termino?
            {
                timers[id].expired = true;  // hubo timeout!
                if(timers[id].callback)     // si el callback no es NULL       
                    timers[id].callback();  // 1) execute action: callback
                update(id);             // 2) update state
            }         
        }
    }
    gpioWrite(PIN_ISR_TEST, LOW);
}

static void timerResetCnt(tim_id_t id)
{
    timers[id].cnt = timers[id].period;
}

static void update(tim_id_t id)
{ 
    if(timers[id].mode == TIM_MODE_SINGLESHOT)
        timerStop(id);
    else
    {
        if(timers[id].callback) // si no hay callback, se deja alto el flag
            timers[id].expired = false;
        timerResetCnt(id);
    }
    return;
}
/******************************************************************************/
