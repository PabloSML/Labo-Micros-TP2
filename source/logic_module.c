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
static unsigned char ID_array[8] ={-1};
static unsigned char PIN_array[5] = {-1};
static char posc_ID = -1;
static char posc_ptr = -1;
static unsigned int ID;
static unsigned int PIN;
static unsigned int valid_IDs[5]= {1234567891,1111122222,9988776622,3434567845,5544367812};
static unsigned int valid_PINs[5]={1234,12345,11111,23452,1122};
/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

void logic_module_init(void){


}


void logic_module(void){

	//fetch event
	 switch(event){
	    case ID_CARD:
	    	if(getPANlen()==8)
	    		ID_array=(unsigned int)getPAN();
	    		convert_ID();
				if(check_ID())
					int x1;
				//push VALID_ID event
				else
					int x2;
				//push INVALID_ID event
			break;
	    case INPUT_NUMBER:
	    	//igual al resto, devuelve numero entero
	        break;

	    case RESET:
	        posc_ID=-1;

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
	size_t n = sizeof(valid_IDs)/sizeof(valid_IDs[0]);

	while(posc<n)
	{
		if (valid_IDs[posc]==ID)
			valid_ID=true;
		else
			posc++;
	}
	posc_ID=posc;

	return valid_ID;
}


bool check_PIN(unsigned int PIN){
	unsigned char posc=0;
	bool valid_PIN=false;

	if(valid_PINs[posc_ID]==PIN)
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
