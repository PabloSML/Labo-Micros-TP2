/***************************************************************************//**
  @file     logic_module.c
  @brief    Logic Module Source File
  @author   Grupo 4
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "logic_module.h"
//#include "gpio_pdrv.h"
//#include "timer_drv.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/


/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/
 typedef struct {
	 unsigned int valid_IDs[5];
	 unsigned int valid_PINs[5];
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
//static unsigned char ID_array[8] ={-1};
//static unsigned char PIN_array[5] = {-1};
static char posc_ID = -1;
static char posc_ptr = -1;
static unsigned int ID=0x00;
static unsigned int PIN=0x00;
static credentials_format valid_credentials;
static LM_event_t LM_ev = LM_No_Event;
static bool waiting_for_PIN = false;

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
	start_decoder();
	magneticReaderInit();
}


void run_logic_module(void){

	int event = 0x00;

	if(decoder_hasEvent())
	{
		event=decoder_getEvent(); //fetch event
		switch(event){

	    case DECODER_inputnum: //From encoder
			    if(waiting_for_PIN)
				{
					PIN = getNumber();
					if(check_PIN())
					{
						LM_ev=LM_VALID_PIN;
						decoder(DECODER_id); //restart
					}
					else
						LM_ev=LM_INVALID_PIN;
				}
				else
				{
					ID=getNumber();
					if(check_ID())
					{
						LM_ev=LM_VALID_ID;
						waiting_for_PIN=true;
						decoder(DECODER_pin);
					}
					else
						LM_ev=LM_INVALID_ID;
				}
			break;
		
		case DECODER_restart:
			waiting_for_PIN=false;
			LM_ev=LM_RESET;
			decoder(DECODER_id);
			//borrar display
	    	//
	        break;


	    default:
	        //if(intensity set){ state = WAIT_ID}
	        break;
	}	

	else if(magreader_hasEvent()&& (!waiting_for_PIN))
	{
		event=magreader_getEvent();  //fetch event
		
		switch(event){

			case MAGREADER_cardUpload: //From magnetic card
	    		if(getPANlen()==8)
	    			ID_array=(unsigned int)getPAN();
	    			convert_ID();
					if(check_ID())
					{
						LM_ev=LM_VALID_ID;
						decoder(DECODER_pin);
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

	valid_credentials.valid_IDs={1234567891,1111122222,9988776622,3434567845,5544367812};
	valid_credentials.valid_PINs={1234,12345,11111,23452,1122};

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

		K=ID;
}




/******************************************************************************/
