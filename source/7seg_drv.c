/***************************************************************************//**
  @file     7seg_drv.c
  @brief    Seven Segment Driver Source File
  @author   Grupo 4
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "7seg_drv.h"
#include "gpio_pdrv.h"
#include "timer_drv.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define SEVEN_SEG_DEVELOPMENT_MODE 1

#define CSEGA   PORTNUM2PIN(PC,16)  //PTC16 
#define CSEGB   PORTNUM2PIN(PC,17)  //PTC17
#define CSEGC   PORTNUM2PIN(PB,9)   //PTB9
#define CSEGD   PORTNUM2PIN(PA,1)   //PTA1
#define CSEGE   PORTNUM2PIN(PB,23)  //PTB23
#define CSEGF   PORTNUM2PIN(PA,2)   //PTA2
#define CSEGG   PORTNUM2PIN(PC,2)   //PTC2
#define CSEGDP  PORTNUM2PIN(PC,3)   //PTA0
#define SEL0    PORTNUM2PIN(PA,0)   //PTC4
#define SEL1    PORTNUM2PIN(PC,4)   //PTD0


#define NUMBER_OF_SEGMENTS 6
#define MAX_LENGTH_MESSAGE 20


/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

/*******************************************************************************
 * VARIABLES WITH GLOBAL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/
/**
 * @brief character ASCII to 7Segment
 * @param receive ch return 7Seg format (gfedcba)
 */
static uint8_t char2SSeg(char ch);

/**
 * @brief write on pins DPgfedcba
 * @param receive ch and dp state
 */
static void putCharacter(uint8_t ch, bool dp);

/**
 * @brief set SEL0,SEL1
 * @param receive id 7Seg
 */
static void selSSeg(seven_seg_t id);

/**
 * @brief write character on 7 segment display
 * @param uint8_t character to write
 * @param bool dpState true:on false:off
 * @param segment id
 */
static void sevenSegWrite(uint8_t character,bool dpState, seven_seg_t id);

/**
 * @brief Periodic service to refresh display
 */
static void display_refresh_isr(void);

/*******************************************************************************
 * ROM CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

//https://en.wikichip.org/wiki/seven-segment_display/representing_letters

static const uint8_t seven_seg_digits_decode_gfedcba[75]= {
/*  0     1     2     3     4     5     6     7     8     9     :     ;     */
    0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F, 0x00, 0x00, 
/*  <     =     >     ?     @     A     B     C     D     E     F     G     */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71, 0x3D, 
/*  H     I     J     K     L     M     N     O     P     Q     R     S     */
    0x76, 0x30, 0x1E, 0x75, 0x38, 0x55, 0x54, 0x5C, 0x73, 0x67, 0x50, 0x6D, 
/*  T     U     V     W     X     Y     Z     [     \     ]     ^     _     */
    0x78, 0x3E, 0x1C, 0x1D, 0x64, 0x6E, 0x5B, 0x00, 0x00, 0x00, 0x00, 0x00, 
/*  `     a     b     c     d     e     f     g     h     i     j     k     */
    0x00, 0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71, 0x3D, 0x76, 0x30, 0x1E, 0x75, 
/*  l     m     n     o     p     q     r     s     t     u     v     w     */
    0x38, 0x55, 0x54, 0x5C, 0x73, 0x67, 0x50, 0x6D, 0x78, 0x3E, 0x1C, 0x1D, 
/*  x     y     z     */
    0x64, 0x6E, 0x5B
};

static const uint8_t gfedcbaPins[]= {CSEGA,CSEGB,CSEGC,CSEGD,CSEGE,CSEGF,CSEGG};

static const uint8_t selPins[] = {SEL0, SEL1};

//Refresh display variables..
static tim_id_t timerRefreshId;
static ttick_t timerTicks = DISPLAY_ISR_PERIOD;

//Message variable
static uint8_t msg[MAX_LENGTH_MESSAGE];
static uint8_t actual_size_msg;

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

bool sevenSegInit(void)
{
  static bool yaInit = false;

  if (!yaInit) //init peripheral
  {
    gpioMode(CSEGA, OUTPUT);  //set gpio connected to 7seg as output
    gpioMode(CSEGB, OUTPUT);
    gpioMode(CSEGC, OUTPUT);
    gpioMode(CSEGD, OUTPUT);
    gpioMode(CSEGE, OUTPUT);
    gpioMode(CSEGF, OUTPUT);
    gpioMode(CSEGG, OUTPUT);
    gpioMode(CSEGDP, OUTPUT);
    gpioMode(SEL0, OUTPUT);   //set gpio connected to select as output
    gpioMode(SEL1, OUTPUT);

    for(uint8_t i = 0; i <= NUMBER_OF_SEGMENTS;i++)
    {
      gpioWrite(gfedcbaPins[i],LOW); // 7seg off
      gpioWrite(selPins[i], LOW)
    }

    timerInit();
    timerRefreshId = timerGetId();  //Agregar timer scrolling y timer brillo..
#ifdef DISPLAY_DEVELOPMENT_MODE
      if(timerId != TIMER_INVALID_ID)
#endif
      {
        timerStart(timerRefreshId, timerTicks, TIM_MODE_PERIODIC, &display_refresh_isr);
        yaInit = true;
      }
  }
  return yaInit;
}

//tenia la idea de q reciba puntero y size, seria mas rapido
// pero si le pasas mal el size puede terminar leyendo basura..
void dispMSG(const char newMsg[], bool scrolling) 
{
  uint8_t size_msg = sizeof(newMsg);
  actual_size_msg = size_msg > MAX_LENGTH_MESSAGE ? MAX_LENGTH_MESSAGE:size_msg;

  for(uint8_t i = 0; i < actual_size_msg; i++)
  {
    msg[i] = char2SSeg(newMsg[i]);
  }

}

void dispCLR(void)
{
  for(uint8_t i = 0; i < MAX_LENGTH_MESSAGE; i++)
  {
    msg[i] = 0x00;
  }
}



/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/
static uint8_t char2SSeg(char ch) 
{
  return (ch >= (uint8_t)'0' & ch <= (uint8_t)'z' ) ? 
    seven_seg_digits_decode_gfedcba[ch-(uint8_t)'0'] : (uint8_t)0x00;
}


static void putCharacter(uint8_t ch, bool dp)
{
  for(uint8_t i = 0; i <= NUMBER_OF_SEGMENTS;i++)   //update gfedcba pin
  {
    gpioWrite(gfedcbaPins[i],((ch>>i)&(uint8_t)1)); //shifting i times 7SEG word and mask 1
  }
  gpioWrite(CSEGDP,dp)      //update decimal point
}


static void selSSeg(seven_seg_t id)
{
/*
*   gpioWrite(SEL0,(uint8_t)id&(uint8_t)1);   \\probar si esto funciona...
*   gpioWrite(SEL1,(uint8_t)id&(uint8_t)2);
*/

  switch(id)
  {
    case CSEG_1:
      gpioWrite(SEL0, LOW);
      gpioWrite(SEL1, LOW);
      break;
    case CSEG_2:
      gpioWrite(SEL0, HIGH);
      gpioWrite(SEL1, LOW);
      break;
    case CSEG_3:
      gpioWrite(SEL0, LOW);
      gpioWrite(SEL1, HIGH);
      break;
    case CSEG_4:
      gpioWrite(SEL0, HIGH);
      gpioWrite(SEL1, HIGH);
      break;
  }
}

static void sevenSegWrite(uint8_t character,bool dpState, seven_seg_t id)
{
  if(id < SSEG_MAX)       //check valid id
  {
      selSSeg(id);        //update select 7SEG to change
      putCharacter(character,dpState); //update actual character
  }
}


static void display_refresh_isr(void) //Por ahora solo maneja 4 caracteres y no scrollea..
{
  static seven_seg_t actual7SegDisp = 0x00;

  if(actual_size_msg<SSEG_MAX)
  {
    actual7SegDisp = SSEG_MAX - actual_size_msg; //offset display [][][][1]
  }

  if(actual7SegDisp>=SSEG_MAX) //restart actual display
  {
    actual7SegDisp=0;    
  }

  sevenSegWrite(msg[actual7SegDisp],false,actual7SegDisp);  //write character
  actual7SegDisp++; //move next display
}



/******************************************************************************/
