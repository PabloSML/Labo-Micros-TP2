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
#include "7seg_drv.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/
#define GUION       '='
#define CLEAN_ID    {GUION,GUION,GUION,GUION,GUION,GUION,GUION,GUION}
#define CLEAN_PIN   {GUION,GUION,GUION,GUION,GUION}
#define DEFAULT_INT {INTENSITY_MAXVALUE}

#define BLINK_SPEED  500

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

void change_number(EncoderEvent_t move);
void move_cursor(EncoderEvent_t move);
bool decoder_validateNumber(void);

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
static int8_t open[4] = {'O','P','E','N'};

static uint8_t intensityString[4] = {'L','I','=','3'};

static uint8_t char_number[ID_SIZE] = CLEAN_ID;

static int64_t fullnumber;

static uint8_t size = 0;
static int8_t *numberpos;
static uint8_t maxvalue;
static uint8_t minvalue;



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

void updateDisplay(void){
    dispCLR();
    uint8_t len = 0;
    if(type == DECODER_intensity){
        intensityString[3] = '0' + intensity[0];
        setBright(intensity[0]*2);
        dispMSG(intensityString, 4);
    }
    else if(type == DECODER_open){
        dispMSG(open, 4);
    }
    else{
        uint8_t len =size - (cursor/4)*4;
        if(len > 4){len =4;}
        dispMSG(&char_number[(cursor/4)*4],len );
        dispDP((cursor/4), true);
        switch(state){
            case DECODER_position:
                dispBlink(cursor%4,BLINK_SPEED);
                break;
            case DECODER_number:
                dispOn(cursor%4);
                break;
            default:
                break;
        }
    }
}

void change_number(EncoderEvent_t move){
    /*switch(type){
        case DECODER_intensity:
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
    }*/
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
    /*switch(type){
        case DECODER_intensity:
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
    }*/
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
    type = dtype;
    state = DECODER_position;
    cursor = 0;
    fullnumber = 0;
    switch(type){ //select size of number and array to modify
        case DECODER_intensity:
            size = INTENSITY_SIZE;
            maxvalue = INTENSITY_MAXVALUE;
            minvalue = INTENSITY_MINVALUE;
            numberpos = intensity;
            for(int8_t i = 0; i < size; i++)
            {
                numberpos[i] = INTENSITY_MAXVALUE;
                state = DECODER_number;
            }
            break;
        case DECODER_id:
            size = ID_SIZE;
            maxvalue = ID_MAXDIGITNUMBER;
            minvalue = ID_MINDIGITNUMBER;
            numberpos = id_number;
            break;
        case DECODER_pin:
            size = PIN_MAXSIZE;
            maxvalue = PIN_MAXDIGITNUMBER;
            minvalue = PIN_MINDIGITNUMBER;
            numberpos = pin_number;
            break;
        case DECODER_open:
            numberpos = open;
    }
    if((type == DECODER_id)|(type == DECODER_pin)){

        for(int8_t i = 0; i < size; i++)
        {
            numberpos[i] = GUION;
            char_number[i] = GUION;
        }
    }
    updateDisplay();
}

//ingresas en que estado estas (Intensity, pin o id) y devuelve evento (noev, inputnum o restart)

bool decoder_hasEvent(void){ 
    ev = DECODER_noev; 
    if(button_hasEvent()){
        newButtonEv = button_getEvent();
        switch (newButtonEv){
            case BUTTON_ePress:
                /* Act on release... */
                break;
            case BUTTON_eRelease:
                if(type == DECODER_open){
                    ev = DECODER_restart;
                }
                else if(prevButtonEv == BUTTON_ePress)
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
                    if(decoder_validateNumber){
                        ev = DECODER_inputnum;
                    }
                    else{
                        ev = DECODER_inputerror;
                    }
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
        updateDisplay();
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

//Call when decoder_getEvent returns inputnumber
//Get de value of number if valid, else get -1
int64_t decoder_getNumber(void){
    if(decoder_validateNumber()){  
        return fullnumber;
    }
    else{
        return -1;
    }
}

//Validate if number was fully inserted and within the admitted range. Guarda en fullnumber si fue valido.
//Probado que funciona
bool decoder_validateNumber(void){
    bool valid = true;
    switch(type){
        case DECODER_intensity:
            size = INTENSITY_SIZE;
            numberpos = intensity;
            maxvalue = INTENSITY_MAXVALUE;
            minvalue = INTENSITY_MINVALUE;
            break;
        case DECODER_id:
            size = ID_SIZE;
            numberpos = id_number;
            maxvalue = ID_MAXDIGITNUMBER;
            minvalue = ID_MINDIGITNUMBER;
            break;
        case DECODER_pin:
            size = PIN_MAXSIZE;
            numberpos = pin_number;
            maxvalue = PIN_MAXDIGITNUMBER;
            minvalue = PIN_MINDIGITNUMBER;
            break;
    }
    for(uint8_t i = 0; (i < size) & valid; i++){
        if(!((type == DECODER_pin)&(i == size-1)&(numberpos[i] == GUION))){
            if(!((numberpos[i] <= maxvalue) & (numberpos[i] >= minvalue))){ //If number not in valid range
                valid = false;
                fullnumber = -1;
            }
            else{
                if(i == 0){
                    fullnumber = numberpos[i];
                }
                else{
                    fullnumber = fullnumber*10+ numberpos[i];
                }
            }
        }
    }
    return valid;
}

void number2char(void){
    for(uint8_t i = 0; i < size; i++)
    {
        if(numberpos[i] == GUION){
            char_number[i] = GUION;
        }
        else{
            if(type == DECODER_pin){
                char_number[i] = 'H';
            }
            else{
                char_number[i] = '0' + numberpos[i];
            }
        }
    }
}


/******************************************************************************/
