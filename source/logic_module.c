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
#include "gpio_pdrv.h"
#include "timer_drv.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/


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
 bool check_ID();
 bool check_PIN();
 void convert_ID();
 void convert_PIN();
 void upload_valid_credentials();

/*******************************************************************************
 * ROM CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/
static uint8_t ID_array[8] ={-1};
static uint8_t PIN_array[5] = {-1};
static char posc_ID = -1;
static char posc_ptr = -1;
static uint64_t ID=0x00;
static uint64_t PIN=0x00;
static credentials_format valid_credentials;
static LM_event_t LM_ev = LM_No_Event;
static bool waiting_for_PIN = false;

static DecoderType_t estado = DECODER_intensity;

//static unsigned int valid_IDs[5];
//static unsigned int valid_PINs[5];
/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

bool logic_module_init(void){
	
	bool proper_initialization =false;
	
	upload_valid_credentials();

	//initialize the drivers
	ledInit(); 
	ledOn(LED_1);
	buttonInit();
	encoderInit();
	//start_decoder();  //No es necesario la inicializacion
	magneticReaderInit();
}


void run_logic_module(void){

	int8_t event = 0x00;

	if(decoder_hasEvent())
	{
		event = decoder_getEvent(); //fetch event
		switch(event){

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
					//*******Prender 1 led
					decoder(estado);
					break;
				case DECODER_id:
					ID = decoder_getNumber();
					//No es necesario chequear que el ID este dentro de los usuarios, se chequea con el PIN
					estado = DECODER_pin;
					//*******Prender 2 leds
					waiting_for_PIN = true; //No es necesario si vamos a usar estados
					decoder(estado);
					break;
				case DECODER_pin:
					PIN = decoder_getNumber();
					if(check_ID()&check_PIN()){
						estado = DECODER_open;
						///*******Prender los 3 leds
						decoder(estado);
					}
					else{
						estado = DECODER_id;
						//*******Prender 1 led
						decoder(estado);
					}
					waiting_for_PIN = false; //No es necesario si vamos a usar estados
					break;
				default:
					break;
			}
			break;
		
		case DECODER_restart:
			waiting_for_PIN=false;
			ID = 0;
			PIN = 0;
			LM_ev=LM_RESET;
			decoder(DECODER_intensity); //Lo mando setear la intensidad
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
		event=magreader_getEvent();  //fetch event
		
		switch(event){

			case MAGREADER_cardUpload: //From magnetic card
	    		if(getPANlen()==8){
					uint8_t * p = getPAN();
					for(uint8_t i = 0; i < 8; i++){
						ID_array[i] = p[i];
					}
	    			//ID_array = (uint8_t *)getPAN();
	    			convert_ID();
					if(check_ID())
					{
						LM_ev=LM_VALID_ID;
						decoder(DECODER_pin);
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
bool check_ID(){

	unsigned char posc=0;
	bool valid_ID=false;
	size_t n = sizeof(valid_credentials.valid_IDs)/sizeof(valid_credentials.valid_IDs[0]);

	while(posc<n)
	{
		if (valid_credentials.valid_IDs[posc]==ID)
			valid_ID=true;
		else
			posc++;
	}
	posc_ID=posc;

	return valid_ID;
}

void upload_valid_credentials(void){
	uint8_t validIDs[5] = {1234567891,1111122222,9988776622,3434567845,5544367812};
	uint8_t validPINs[5] = {1234,12345,11111,23452,1122};
	for(uint8_t i = 0; i < 5; i++){
		valid_credentials.valid_IDs[i] = validIDs[i];
		valid_credentials.valid_PINs[i] = validPINs[i];
	}
}


bool check_PIN(){
	unsigned char posc=0;
	bool valid_PIN=false;

	if(valid_credentials.valid_PINs[posc_ID]==PIN)
		valid_PIN=true;

	return valid_PIN;
}

void convert_ID(){
	int i, k = 0;
	size_t n = sizeof(ID_array)/sizeof(ID_array[0]);

	for (i = 0; i < n; i++)
	    k = 10 * k + ID_array[i] + 1;

	k=ID;
}




/******************************************************************************/
