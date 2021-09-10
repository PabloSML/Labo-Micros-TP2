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

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define SEVEN_SEG_DEVELOPMENT_MODE 1

#define SSEGA   PORTNUM2PIN(PC,16)  //PTC16 
#define SSEGB   PORTNUM2PIN(PC,17)  //PTC17
#define SSEGC   PORTNUM2PIN(PB,9)   //PTB9
#define SSEGD   PORTNUM2PIN(PA,1)   //PTA1
#define SSEGE   PORTNUM2PIN(PB,23)  //PTB23
#define SSEGF   PORTNUM2PIN(PA,2)   //PTA2
#define SSEGG   PORTNUM2PIN(PC,2)   //PTC2
#define SSEGDP  PORTNUM2PIN(PC,3)   //PTA0
#define SEL0    PORTNUM2PIN(PA,0)   //PTC4
#define SEL1    PORTNUM2PIN(PC,4)   //PTD0


#define NUMBER_OF_SEGMENTS 6


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
static uint8_t char2SSeg(uint8_t ch);

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

static const uint8_t gfedcbaPins[]= {SSEGA,SSEGB,SSEGC,SSEGD,SSEGE,SSEGF,SSEGG};


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
    gpioMode(SSEGA, OUTPUT);  //set gpio connected to 7seg as output
    gpioMode(SSEGB, OUTPUT);
    gpioMode(SSEGC, OUTPUT);
    gpioMode(SSEGD, OUTPUT);
    gpioMode(SSEGE, OUTPUT);
    gpioMode(SSEGF, OUTPUT);
    gpioMode(SSEGG, OUTPUT);
    gpioMode(SSEGDP, OUTPUT);
    gpioMode(SEL0, OUTPUT);   //set gpio connected to select as output
    gpioMode(SEL1, OUTPUT);
    yaInit = true;
  }
  return yaInit;
}


void sevenSegWrite(uint8_t character,bool dpState, seven_seg_t id)
{
  if(id < SSEG_MAX)       //check valid id
  {
      selSSeg(id);        //update select 7SEG to change
      putCharacter(character,dpState); //update actual character
  }
}



/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/
static uint8_t char2SSeg(uint8_t ch) 
{
  return (ch >= (uint8_t)'0' & ch <= (uint8_t)'z' ) ? 
    seven_seg_digits_decode_gfedcba[ch-(uint8_t)'0'] : (uint8_t)0x00;
}

static void putCharacter(uint8_t ch, bool dp)
{
  uint8_t sseg_ch = char2SSeg(ch);  //ASCII -> 7SEG
  for(uint8_t i = 0; i <= NUMBER_OF_SEGMENTS;i++)        //update gfedcba pin
  {
    gpioWrite(gfedcbaPins[i],((sseg_ch>>i)&(uint8_t)1)); //shifting i times 7SEG word and mask 1
  }

  if(dp)                    //update decimal point
    gpioWrite(SSEGDP,HIGH);
  else
    gpioWrite(SSEGDP,LOW);
}

static void selSSeg(seven_seg_t id)
{
  switch(id)
  {
    case SSEG_1:
      gpioWrite(SEL0, LOW);
      gpioWrite(SEL1, LOW);
      break;
    case SSEG_2:
      gpioWrite(SEL0, HIGH);
      gpioWrite(SEL1, LOW);
      break;
    case SSEG_3:
      gpioWrite(SEL0, LOW);
      gpioWrite(SEL1, HIGH);
      break;
    case SSEG_4:
      gpioWrite(SEL0, HIGH);
      gpioWrite(SEL1, HIGH);
      break;
  }
}


/******************************************************************************/
