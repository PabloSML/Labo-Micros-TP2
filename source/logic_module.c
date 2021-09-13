/***************************************************************************//**
  @file     logic_module.c
  @brief    Logic Module Source File
  @author   Grupo 4
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "logic_module.h"
#include "decoder.h"
#include "led_drv.h"
#include "gpio_pdrv.h"
#include "timer_drv.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define ID_LEN			8

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/


 typedef struct {
	uint64_t valid_IDs[5];
	uint64_t valid_PINs[5];
 } credentials_format;



/*******************************************************************************
 * VARIABLES WITH GLOBAL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/
 static bool check_ID();
 static bool check_PIN();
 static void convert_ID();
 static void upload_valid_credentials();

/*******************************************************************************
 * ROM CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/
static uint8_t ID_array[8];
static uint8_t PIN_array[5] = {-1};
static uint8_t posc_ID = 0;
static uint8_t posc_ptr = 0;
static uint64_t ID=0;
static uint64_t PIN=0x00;
static credentials_format valid_credentials;
static LM_event_t LM_ev = LM_No_Event;
static DecoderEvent_t decoderEv = DECODER_noev;
static MagReaderEvent_t magreaderEv = MAGREADER_noev;
static uint8_t panLen = 0;
static bool waiting_for_PIN = false;

static DecoderType_t estado = DECODER_intensity;

//static unsigned int valid_IDs[5];
//static unsigned int valid_PINs[5];
/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

bool logic_module_init(void)
{
  static bool yaInit = false;
    
  if (!yaInit) // init peripheral
  {
    upload_valid_credentials();
	ledInit();
	decoder(DECODER_intensity);
	yaInit = true;
	//initialize the drivers (quizás)
  }

  return yaInit;
}

void run_logic_module(void){

	// int8_t event = 0x00;

	if(decoder_hasEvent())
	{
		decoderEv = decoder_getEvent(); //fetch event
		switch(decoderEv)
		{

	    case DECODER_inputnum: //From encoder
			//*****Germo: comente esto porque al agregar el estado de la intensidas, se necesitaba mas que un flag para ver si esta en ID o PIN
			//***** Fijate si te convence

			/*if(waiting_for_PIN)
			{
				PIN = decoder_getNumber();
				if(check_PIN())
				{
					LM_ev=LM_VALID_PIN;
					decoder(DECODER_id); //restart
				}
				else
				{
					LM_ev=LM_INVALID_PIN;
				}
			}
			else
			{
				ID = decoder_getNumber();
				if(check_ID())
				{
					LM_ev=LM_VALID_ID;
					waiting_for_PIN=true;
					decoder(DECODER_pin);
				}
				else
				{
					LM_ev=LM_INVALID_ID;
				}
			}*/
			switch(estado){
				case DECODER_intensity:
					estado = DECODER_id;
					ledOn(LED_3);
					ledOff(LED_2);
					ledOff(LED_1);
					decoder(estado);
					break;
				case DECODER_id:
					ID = decoder_getNumber();
					if (check_ID())
					{
						estado = DECODER_pin;
						ledOn(LED_3);
						ledOn(LED_2);
						ledOff(LED_1);
						decoder(estado);
					}
					else{
						estado = DECODER_invalid;
						ledBlink(LED_1, 500U);
						ledBlink(LED_2, 500U);
						ledBlink(LED_3, 500U);
						decoder(estado);
					}
					break;
				case DECODER_pin:
					PIN = decoder_getNumber();
					if(check_PIN()){
						estado = DECODER_open;
						ledOn(LED_3);
						ledOn(LED_2);
						ledOn(LED_1);
						decoder(estado);
					}
					else{
						estado = DECODER_invalid;
						ledBlink(LED_1, 500U);
						ledBlink(LED_2, 500U);
						ledBlink(LED_3, 500U);
						decoder(estado);
					}
					break;
				default:
					break;
			}
			break;
		
		case DECODER_restart:
			ID = 0;
			PIN = 0;
			LM_ev=LM_RESET;
			if (estado == DECODER_invalid)
			{
				estado = DECODER_id;
				ledOn(LED_3);
				ledOff(LED_2);
				ledOff(LED_1);
			}
			else
			{
				estado = DECODER_intensity;
				ledOff(LED_3);
				ledOff(LED_2);
				ledOff(LED_1);
			}
			decoder(estado); //Lo mando setear la intensidad o id si fue inválido
			//borrar display
	    	//
	        break;

		case DECODER_inputerror:
			decoder(estado);	//Si le falto un numero en id, que siga en ID, lo mismo para PIN
			break;

	    default:
	        //if(intensity set){ state = WAIT_ID}
	        break;
		}
	}	

	else if(magreader_hasEvent()&& (!waiting_for_PIN))
	{
		magreaderEv = magreader_getEvent();  //fetch event
		
		switch(magreaderEv)
		{

			case MAGREADER_cardUpload: //From magnetic card
				panLen = getPANlen();
	    		if(panLen >= ID_LEN)
				{
					uint8_t * pan = getPAN();
					for(uint8_t i = 0; i < ID_LEN; i++){
						ID_array[i] = pan[i];
					}
	    			//ID_array = (uint8_t *)getPAN();
	    			convert_ID();
					if(check_ID())
					{
						estado = DECODER_pin;
						ledOn(LED_3);
						ledOn(LED_2);
						ledOff(LED_1);
						decoder(estado);
					}
					else
						LM_ev=LM_INVALID_ID;
				}
				else
					LM_ev=LM_INVALID_ID;
				break;

			case MAGREADER_carderror: 

				break;

			case MAGREADER_cardsliding: 

				break;
		
			default:
				//if(intensity set){ state = WAIT_ID}
				break;

		}
	}
}


bool logic_module_hasEvent(void){
	return LM_ev;
}


LM_event_t logic_module_getEvent(void){
	return LM_ev;
}


int get_ID(void){

	return ID;
}


int get_PIN(void){

	return PIN;
}
/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/
static bool check_ID(){

	uint8_t posc = 0;
	bool valid_ID=false;
	uint8_t n = (uint8_t)(sizeof(valid_credentials.valid_IDs)/sizeof(valid_credentials.valid_IDs[0]));

	while((posc < n) && (valid_ID == false))
	{
		if (valid_credentials.valid_IDs[posc]==ID)
			valid_ID=true;
		else
			posc++;
	}
	posc_ID = posc;

	return valid_ID;
}

void upload_valid_credentials(void){
	uint64_t validIDs[5] = {12345678UL,11112222UL,11111111UL,34345678UL,63913003UL};
	uint64_t validPINs[5] = {1234UL,12345UL,1111UL,23452UL,1111UL};
	for(uint8_t i = 0; i < 5; i++){
		valid_credentials.valid_IDs[i] = validIDs[i];
		valid_credentials.valid_PINs[i] = validPINs[i];
	}
}


static bool check_PIN(void)
{
	bool valid_PIN = false;

	if(valid_credentials.valid_PINs[posc_ID] == PIN)
		valid_PIN = true;

	return valid_PIN;
}

static void convert_ID(void)
{
	int i = 0;
	uint8_t n = (uint8_t)(sizeof(ID_array)/sizeof(ID_array[0]));
	ID = 0;

	for (i = 0; i < n; i++)
		ID = ID*10 + ID_array[i];

}




/******************************************************************************/
