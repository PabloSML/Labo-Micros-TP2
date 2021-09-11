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

#define PIN_CSEGA         PORTNUM2PIN(PC,5)  // D.O - AH
#define PIN_CSEGB         PORTNUM2PIN(PC,7)   // D.O - AH
#define PIN_CSEGC         PORTNUM2PIN(PC,0)   // D.O - AH
#define PIN_CSEGD         PORTNUM2PIN(PC,9)   // D.O - AH
#define PIN_CSEGE         PORTNUM2PIN(PC,8)   // D.O - AH
#define PIN_CSEGF         PORTNUM2PIN(PC,1)   // D.O - AH
#define PIN_CSEGG         PORTNUM2PIN(PB,19)  // D.O - AH
#define PIN_CSEGDP        PORTNUM2PIN(PB,18)  // D.O - AH

#define PIN_SEL0          PORTNUM2PIN(PC,3)   // D.O - AH
#define PIN_SEL1          PORTNUM2PIN(PC,2)   // D.O - AH

#define NUMBER_OF_SEGMENTS 6
#define MAX_LENGTH_MESSAGE 20

#define SEGMENT_ACTIVE   HIGH 

#define DP_ACTIVE  HIGH

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

/**
 * @brief Cycle through DISP focus (DISP1, DISP2, DISP3, DISP4, DISP1, ...)
 */
static void cycle_focus(void);

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

static const uint8_t gfedcbaPins[]= {PIN_CSEGA,PIN_CSEGB,PIN_CSEGC,PIN_CSEGD,PIN_CSEGE,PIN_CSEGF,PIN_CSEGG};

static const uint8_t selPins[] = {PIN_SEL0, PIN_SEL1};

//Refresh display variables..
static tim_id_t timerId;
static ttick_t timerTicks = DISPLAY_ISR_PERIOD;

//Message variable
static uint8_t msg[MAX_LENGTH_MESSAGE];
static uint8_t actual_size_msg;

// Currently focused display unit
static seven_seg_t actual7SegDisp = DISP_1;

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
    
    gpioWrite(PIN_SEL0, LOW);
    gpioWrite(PIN_SEL1, LOW);

    for(uint8_t i = 0; i <= NUMBER_OF_SEGMENTS;i++)
    {
      gpioWrite(gfedcbaPins[i], !SEGMENT_ACTIVE); // 7seg off    
    }

    gpioMode(PIN_CSEGA, OUTPUT);  //set gpio connected to 7seg as output
    gpioMode(PIN_CSEGB, OUTPUT);
    gpioMode(PIN_CSEGC, OUTPUT);
    gpioMode(PIN_CSEGD, OUTPUT);
    gpioMode(PIN_CSEGE, OUTPUT);
    gpioMode(PIN_CSEGF, OUTPUT);
    gpioMode(PIN_CSEGG, OUTPUT);
    gpioMode(PIN_CSEGDP, OUTPUT);
    gpioMode(PIN_SEL0, OUTPUT);   //set gpio connected to select as output
    gpioMode(PIN_SEL1, OUTPUT);

    timerInit();
    timerId = timerGetId();  //Agregar sub-timer scrolling y sub-timer brillo..
#ifdef SEVEN_SEG_DEVELOPMENT_MODE
      if(timerId != TIMER_INVALID_ID)
#endif
      {
        timerStart(timerId, timerTicks, TIM_MODE_PERIODIC, &display_refresh_isr);
        yaInit = true;
      }
  }
  return yaInit;
}

//tenia la idea de q reciba puntero y size, seria mas rapido
// pero si le pasas mal el size puede terminar leyendo basura..
void dispMSG(const char newMsg[])
{
  // uint8_t size_msg = sizeof(newMsg);
  uint8_t size_msg = 4;

  actual_size_msg = (size_msg > MAX_LENGTH_MESSAGE) ? MAX_LENGTH_MESSAGE:size_msg;

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
  return (ch >= ((uint8_t)'0') && (ch <= ((uint8_t)'z') )) ?   
    seven_seg_digits_decode_gfedcba[ch-(uint8_t)'0'] : (uint8_t)0x00;
}



static void putCharacter(uint8_t ch, bool dp)
{
  for(uint8_t i = 0; i <= NUMBER_OF_SEGMENTS;i++)   //update gfedcba pin
  {
    gpioWrite(gfedcbaPins[i],((ch>>i)&(uint8_t)1)); //shifting i times 7SEG word and mask 1
  }
  gpioWrite(PIN_CSEGDP, dp);      //update decimal point
}


static void selSSeg(seven_seg_t id)
{
/*
*   gpioWrite(SEL0,(uint8_t)id&(uint8_t)1);   \\probar si esto funciona...
*   gpioWrite(SEL1,(uint8_t)id&(uint8_t)2);
*/

  switch(id)
  {
    case DISP_1:
      gpioWrite(PIN_SEL0, LOW);
      gpioWrite(PIN_SEL1, LOW);
      break;
    case DISP_2:
      gpioWrite(PIN_SEL0, HIGH);
      gpioWrite(PIN_SEL1, LOW);
      break;
    case DISP_3:
      gpioWrite(PIN_SEL0, LOW);
      gpioWrite(PIN_SEL1, HIGH);
      break;
    case DISP_4:
      gpioWrite(PIN_SEL0, HIGH);
      gpioWrite(PIN_SEL1, HIGH);
      break;
  }
}

static void sevenSegWrite(uint8_t character, bool dpState, seven_seg_t id)
{
  selSSeg(id);        //update select 7SEG to change
  putCharacter(character, dpState); //update actual character
}


static void display_refresh_isr(void) //Por ahora solo maneja 4 caracteres y no scrollea..
{
  // if(actual_size_msg<CSEG_CANT)
  // {
  //   actual7SegDisp = CSEG_CANT - actual_size_msg; //offset display [][][][1]
  // }

  sevenSegWrite(msg[actual7SegDisp], !DP_ACTIVE, actual7SegDisp);  //write character
  
  cycle_focus();
}


/*


*/

static void cycle_focus(void)
{
  actual7SegDisp = (seven_seg_t)((actual7SegDisp + 1) % DISP_CANT);
}


/******************************************************************************/
