/***************************************************************************//**
  @file     led_drv.c
  @brief    Dedcoder Source File
  @author   Grupo 4
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "decoder.h"
#include "button_drv.h"
#include "encoder_drv.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/
#define GUION       '='
#define CLEAN_ID    {GUION,GUION,GUION,GUION,GUION,GUION,GUION,GUION}
#define CLEAN_PIN   {GUION,GUION,GUION,GUION,GUION}
#define DEFAULT_INT {INTENSITY_MAXVALUE}

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/
typedef enum {
    DECODER_position        = 0x00,
    DECODER_number          = 0x01
} DecoderState_t;

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

//apunta a la posicion del numero que se quiere modificar
static uint8_t cursor;

//Valores iniciales de los numeros -------
static int8_t id_number[ID_SIZE] = CLEAN_ID;
static int8_t pin_number[PIN_MAXSIZE] = CLEAN_PIN;
static int8_t intensity[INTENSITY_SIZE] = DEFAULT_INT;

//Si estoy modificando el numero o moviendome de posicion
static DecoderState_t state;
static DecoderEvent_t ev = DECODER_noev;

static ButtonEvent_t prevButtonEv = BUTTON_noev;
static ButtonEvent_t newButtonEv = BUTTON_noev;

static DecoderType_t type = -1;

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/
void start_decoder(void){
    for(int8_t i = 0; i < ID_SIZE; i++)
    {
        id_number[i] = GUION;
    }
    for(int8_t i = 0; i < PIN_MAXSIZE; i++)
    {
        pin_number[i] = GUION;
    }

    cursor = 0;

}


void change_number(EncoderEvent_t move){

    uint8_t maxvalue;
    uint8_t minvalue;
    int8_t *numberpos;
    switch(type){
        case DECODER_intesity:
            maxvalue = INTENSITY_MAXVALUE;
            minvalue = INTENSITY_MINVALUE;
            numberpos = intensity;
            break;
        case DECODER_id:
            maxvalue = ID_MAXDIGITNUMBER;
            minvalue = ID_MINDIGITNUMBER;
            numberpos = id_number;
            break;
        case DECODER_pin:
            maxvalue = PIN_MAXDIGITNUMBER;
            minvalue = PIN_MINDIGITNUMBER;
            numberpos = pin_number;
            break;
        default:
            break;
    }
    switch(move){
        case ENCODER_eLeftTurn:
            if(numberpos[cursor] == GUION ){    //Para cuando esta vacio (-) -> (9)
                numberpos[cursor] = maxvalue;
            }
            else if((type == DECODER_pin) & (numberpos[cursor] == PIN_MINDIGITNUMBER) & (cursor == PIN_MAXSIZE)){ //Solo para el ultimo valor del PIN que es opcional (0) -> (-)
                numberpos[cursor] = GUION;
            }
            else if(numberpos[cursor] == minvalue ){   //(0) -> (9)
                numberpos[cursor] = maxvalue;
            }
            else{
                numberpos[cursor]--;
            }
            break;
        case ENCODER_eRightTurn:
            if(numberpos[cursor] == GUION ){    // (-) -> (0)
                numberpos[cursor] = minvalue;
            }
            else if((type == DECODER_pin) & (numberpos[cursor] == PIN_MAXDIGITNUMBER) & (cursor == PIN_MAXSIZE)){ //Solo para el ultimo valor del PIN que es opcional (9) -> (-)
                numberpos[cursor] = GUION;
            }
            else if(numberpos[cursor] == maxvalue ){   // (9) -> (0)
                numberpos[cursor] = minvalue;
            }
            else{
                numberpos[cursor]++;
            }
            break;
        default:
            break;
    }
}

void move_cursor(EncoderEvent_t move){
    int8_t size;
    switch(type){
        case DECODER_intesity:
            size = INTENSITY_SIZE;
            break;
        case DECODER_id:
            size = ID_SIZE;
            break;
        case DECODER_pin:
            size = PIN_MAXSIZE;
            break;
        default:
            break;
    }
    switch(move){
        case ENCODER_eLeftTurn:
            if(cursor > 0){
                cursor --;
            }
            break;
        case ENCODER_eRightTurn:
            if(cursor < (size - 1)){
                cursor ++;
            }
        default:
            break;
    }
}

//start decoder
void decoder(DecoderType_t dtype){

}


//ingresas en que estado estas (Intensity, pin o id) y devuelve evento (noev, inputnum o restart)

bool decoder_hasEvent(void){  
    if(button_hasEvent()){
        newButtonEv = button_getEvent();
        switch (newButtonEv){
            case BUTTON_ePress:
                /* Act on release... */
                break;
            case BUTTON_eRelease:
                if(prevButtonEv == BUTTON_ePress)
                {
                    if(state == DECODER_position){
                        state = DECODER_number;
                    }
                    else{
                        state = DECODER_position;
                    }
                }
                else if (prevButtonEv == BUTTON_eLKP)
                {
                    ev = DECODER_inputnum;
                }
                else if (prevButtonEv == BUTTON_eTypeMatic)
                {
                    ev = DECODER_restart;
                }
                break;
            case BUTTON_eLKP:
                /* Act on release... */
                break;
            case BUTTON_eTypeMatic:
                /* Act on release... */
                break;
            default:
                break;
        }
        prevButtonEv = newButtonEv;
    }
    else if(encoder_hasEvent()){
        if(state == DECODER_position){
            move_cursor(encoder_getEvent());
        }
        else{
            change_number(encoder_getEvent());
        }
    }
    if(ev == DECODER_noev){
        return false;
    }
    else{
        return true;
    }
}

DecoderEvent_t decoder_getEvent(void){
    return ev;
}

uint64_t decoder_getNumber(void){
    
}

bool decoder_validateNumber(void){
    uint8_t size = 0;
    switch(type){
        case DECODER_intesity:
            size = INTENSITY_SIZE;
            break;
        case DECODER_id:
            size = ID_SIZE;
            break;
        case DECODER_pin:
            size = PIN_MAXSIZE;
            break;
    }
}




/******************************************************************************/
