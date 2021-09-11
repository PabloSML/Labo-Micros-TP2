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
 bool check_ID(unsigned int ID);
 bool check_PIN(unsigned int PIN);
 void convert_ID();
 void convert_PIN();
 void upload_valid_credentials();

/*******************************************************************************
 * ROM CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/
static unsigned char ID_array[8] ={-1};
static unsigned char PIN_array[5] = {-1};
static char posc_ID = -1;
static char posc_ptr = -1;
static unsigned int ID;
static unsigned int PIN;
static credentials_format valid_credentials;
static LM_event_t LM_ev = LM_No_Event;

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
}


void run_logic_module(void){

	if(encoder_hasEvent())
		event=encoder_getEvent();
	else if(magnetic_reader_hasEvent())

	//fetch event
	 switch(event){

	    case ID_CARD: //From magnetic card
	    	if(getPANlen()==8)
	    		ID_array=(unsigned int)getPAN();
	    		convert_ID();
				if(check_ID())
					LM_ev=LM_VALID_ID;
				else
					LM_ev=LM_INVALID_ID;
			break;

	    case INPUT_NUMBER: //From encoder
			    ID=getNumber()
				if(check_ID())
					LM_ev=LM_VALID_ID;
				else
					LM_ev=LM_INVALID_ID;
			break;
		
		case RESTART:
			//borrar display
	    	//
	        break;


	    default:
	        //if(intensity set){ state = WAIT_ID}
	        break;

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
bool check_ID(unsigned int ID){

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


bool check_PIN(unsigned int PIN){
	unsigned char posc=0;
	bool valid_PIN=false;

	if(valid_credentials.valid_PINs[posc_ID]==PIN)
		valid_PIN=true;

	retutn valid_PIN;
}

void convert_ID(){
	int i, k = 0;
	size_t n = sizeof(ID_array)/sizeof(ID_array[0]);

	for (i = 0; i < n; i++)
	    k = 10 * k + ID_array[i] + 1;
}

void convert_PIN(){

	int i, k = 0;
	size_t n = sizeof(PIN_array)/sizeof(PIN_array[0]);

	for (i = 0; i < n; i++)
	    k = 10 * k + PIN_array[i] + 1;
}

}
/******************************************************************************/
