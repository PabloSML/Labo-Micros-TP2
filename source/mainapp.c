
//Estoy escribiendo aca para que no pise con las pruebas de los drivers

#include "button_drv.h"
#include "led_drv.h"
#include "gpio_pdrv.h"
#include "hardware.h"
#include "MK64F12.h"

typedef enum {WAIT_ID, WAIT_PIN, BLOCK, UNLOCK, INTENSITY} state_t;

typedef enum {STAY, INPUT_ID, VALID_PIN, INVALID_PIN} event_t;

static state_t state;
static event_t event;

void App_Run (void)
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