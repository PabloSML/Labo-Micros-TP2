/***************************************************************************//**
  @file     7seg_drv.c
  @brief    Seven Segment Driver Source File
  @author   Grupo 4
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES *
 ******************************************************************************/

#include "7seg_drv.h"
#include "gpio_pdrv.h"
#include "timer_drv.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define SEVEN_SEG_DEVELOPMENT_MODE 1

#define NUMBER_OF_SEGMENTS 6

#define SEGMENT_ACTIVE   HIGH
#define SEGMENTS_OFF  0x00

#define DP_ACTIVE  HIGH

#define MAX_LENGTH_MESSAGE 20

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

typedef enum
{
  DISABLE               = 0x00,   // Continually OFF
  ENABLE                = 0x01,   // Continually ON
  BLINK                 = 0x02,   // Blinking
  TIMEOUT               = 0x03    // On with timer
} seven_seg_state_t;

typedef struct
{
  seven_seg_state_t   state;
  bool                dp_state;
  ttick_t             disp_timer;
  ttick_t             count;
  bool                isOn;         // Flag for blinking only         

} seven_seg_t;



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
static void selSSeg(seven_seg_label_t id);

/**
 * @brief write character on 7 segment display
 * @param uint8_t character to write
 * @param bool dpState true:on false:off
 * @param segment id
 */
static void sevenSegWrite(uint8_t character,bool dpState, seven_seg_label_t id);

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
    0x00, 0x40, 0x00, 0x00, 0x00, 0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71, 0x3D, 
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


//Refresh display variables..
static tim_id_t timerId;
static ttick_t timerTicks = DISPLAY_ISR_PERIOD;
static seven_seg_t display[DISP_CANT];

static brightness_label_t actual_bright = MAX;
static ttick_t bright_timer = MAX*DISPLAY_ISR_PERIOD;
static ttick_t bright_count = MAX*DISPLAY_ISR_PERIOD;

static uint8_t next_disp = NEXT_DISPLAY_REFRESH_TICKS;



//Message variable
static uint8_t msg[MAX_LENGTH_MESSAGE];
static uint8_t actual_size_msg;
static uint8_t offset;

// Currently focused display unit
static seven_seg_label_t actual7SegDisp = DISP_1;

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

    gpioWrite(PIN_CSEGDP,!DP_ACTIVE);

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

    for(uint8_t i=0; i < DISP_CANT; i++) //initial state of display
    {
      display[i].state = ENABLE;
      display[i].dp_state = !DP_ACTIVE;
    }

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


void scrollRightMsg(void)
{
  if(offset + DISP_CANT < actual_size_msg)
    offset++;
}


void scrollLeftMsg(void)
{
  if(offset)
    offset--;
}

void dispMSG(const char * new_msg, uint8_t size_msg)
{
  actual_size_msg = (size_msg > MAX_LENGTH_MESSAGE) ? MAX_LENGTH_MESSAGE:size_msg;

  uint8_t offset_char = 0;
  if(actual_size_msg < DISP_CANT)
  {
    offset_char = DISP_CANT-actual_size_msg;
    dispCLR();
  }

  for(uint8_t i = offset_char; i < (actual_size_msg + offset_char); i++)
  {
    msg[i] = char2SSeg(*new_msg);
    new_msg++;
  }
}

void dispDP(seven_seg_label_t disp, bool state)
{
  display[disp].dp_state = state;
}


void dispCLR(void)
{
  for(uint8_t i = 0; i < MAX_LENGTH_MESSAGE; i++)
  {
    msg[i] = SEGMENTS_OFF;
  }
  for(uint8_t i=0; i < DISP_CANT; i++) //initial state of display
  {
    display[i].state = ENABLE;
    display[i].dp_state = !DP_ACTIVE;
  }
}


void dispOn(seven_seg_label_t disp)
{
  display[disp].state = ENABLE;
}

void dispOff(seven_seg_label_t disp)
{
  display[disp].state = DISABLE;
}

void dispToggle(seven_seg_label_t disp)
{
  if(display[disp].state < BLINK)
    display[disp].state = (seven_seg_state_t)(!(display[disp].state));
}

void dispBlink(seven_seg_label_t disp, uint32_t period)
{
  display[disp].state = BLINK;
  display[disp].disp_timer = (period*TIMER_SCALING)/(NEXT_DISPLAY_REFRESH_TICKS*DISPLAY_ISR_PERIOD*DISP_CANT);
  display[disp].count = display[disp].disp_timer;
}

void setBright(brightness_label_t level)
{
  actual_bright = level;
  bright_timer = actual_bright*DISPLAY_ISR_PERIOD;
  bright_count = bright_timer;

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


static void selSSeg(seven_seg_label_t id)
{

  gpioWrite(PIN_SEL0, (uint8_t)id&(uint8_t)1); 
  gpioWrite(PIN_SEL1, (uint8_t)id&(uint8_t)2);

}

static void sevenSegWrite(uint8_t character, bool dpState, seven_seg_label_t id)
{
  selSSeg(id);        //update select 7SEG to change
  putCharacter(character, dpState); //update actual character
}


static void display_refresh_isr(void)
{

  if(!(--next_disp))
  {
    switch(display[actual7SegDisp].state)
    {
      case DISABLE:
          sevenSegWrite(SEGMENTS_OFF,!DP_ACTIVE,actual7SegDisp);
          break;
      case ENABLE:
          sevenSegWrite(msg[actual7SegDisp + offset], display[actual7SegDisp].dp_state, actual7SegDisp);  //write character
          break;
      case BLINK:
          if(!(--(display[actual7SegDisp].count)))   // Decremento el contador de ticks, termino?
          {
            display[actual7SegDisp].isOn = !(display[actual7SegDisp].isOn); // Toggle blink state
            display[actual7SegDisp].count = display[actual7SegDisp].disp_timer;         // Reset blink timer
          }
          uint8_t ch = (display[actual7SegDisp].isOn)? (msg[actual7SegDisp + offset]) : (SEGMENTS_OFF);
          bool dp = (display[actual7SegDisp].isOn)? (display[actual7SegDisp].dp_state) : (!DP_ACTIVE);
          sevenSegWrite(ch, dp, actual7SegDisp);
          break;
      default:
        break;
    }
    cycle_focus();
    next_disp = NEXT_DISPLAY_REFRESH_TICKS;
    bright_count = bright_timer;
  }
  else
  {
    if(!(--bright_count))
    {
      sevenSegWrite(SEGMENTS_OFF,!DP_ACTIVE,actual7SegDisp);
      bright_count = bright_timer;
    }
  }

}


static void cycle_focus(void)
{
  actual7SegDisp = (seven_seg_label_t)((actual7SegDisp + 1) % DISP_CANT);
}


/******************************************************************************/
