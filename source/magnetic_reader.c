/*******************************************************************************
  @file     magnetic_reader.c
  @brief    Magnetic Reader Driver
  @author   Grupo 4
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "gpio_pdrv.h"
#include "board.h"
#include "magnetic_reader.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define PIN_MagEnable   PIN_MAG_EN 	//Enable (Low while card is sliding)
#define PIN_MagData     PIN_MAG_DT 	//Data
#define PIN_MagClk      PIN_MAG_CLK	//Clock

#define MAX_CHAR_LEN 			40
#define BITS_PER_CHAR	  	5
#define TRACK2_BITLEN			MAX_CHAR_LEN * BITS_PER_CHAR

#define SS                (uint8_t)0xB
#define FS                (uint8_t)0xD
#define ES                (uint8_t)0xF
#define CHAR_NOT_FOUND    TRACK2_BITLEN
#define LS2MS(data)     (uint8_t)((data[4]<<4) + (data[3]<<3) + (data[2]<<2) + (data[1]<<1) + data[0])
#define NUMBITS(data)   (uint8_t)(((data&16)>>4) + (data&8)>>3) + ((data&4)>>2) + ((data&2)>>1) + (data&1))
/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/


/*******************************************************************************
 * VARIABLES WITH GLOBAL SCOPE
 ******************************************************************************/


/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

static void     magReaderHandler(void);
static void     readData(void);
static bool     parseData(void);
static uint8_t  findSSIndex(void);
static uint8_t  findFSIndex(void);
static uint8_t  findESIndex(void);
static bool     chkParity(uint8_t myData[]);

/*******************************************************************************
 * ROM CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

static bool     cardSwipped = false;
static uint32_t bitCounter = 0;
static bool     cardData[TRACK2_BITLEN + 10*BITS_PER_CHAR]; //Le agrego 10 extra por si acaso
static uint8_t  parsedData[MAX_CHAR_LEN];

/*******************************************************************************
*                     GLOBAL FUNCTION DEFINITIONS
*******************************************************************************/

void magneticReaderInit(void)
{
  //Configuro pins
  gpioMode(PIN_MagEnable, INPUT);
  gpioMode(PIN_MagData, INPUT);
  gpioMode(PIN_MagClk, INPUT);

  //Configuro IRQs
  gpioIRQ(PIN_MagEnable, PORT_eInterruptRising , , magReaderHandler);
  gpioIRQ(PIN_MagClk   , PORT_eInterruptFalling, , readData); //Los datos cambian en posedge
}

/*******************************************************************************
*                     LOCAL FUNCTION DEFINITIONS
*******************************************************************************/

static void magReaderHandler(void)
{
  bitCounter = 0; //Reinicio mi contador

  isdataOK = parseData();

  if(isdataOK)
  {

  }

}

static void readData(void)
{
  if(gpioRead(PIN_MagEnable))
  {
    cardData[bitCounter] = !gpioRead(PIN_MagData); // /DATA
    bitCounter++;
  }
}

static bool parseData(void)
{
  bool okstruct;
  SSindex = findSSIndex();
  FSindex = findFSIndex();
  ESindex = findESIndex();
  if(SSindex == CHAR_NOT_FOUND || FSindex == CHAR_NOT_FOUND || ESindex == CHAR_NOT_FOUND)
  {
    okstruct = false;
  }
  else
  {
    uint8_t PANlen = (FSindex - SSindex)/BITS_PER_CHAR - 1; //Resto SS
    uint8_t realTracklen = MAX_CHAR_LEN - 19 + PANlen; //el largo de PAN es variable

    for(uint8_t char_num = 0; char_num < realTracklen, ++char_num)
    {
      parsedData[char_num] = LS2MS(&cardData[SSindex + char_num*BITS_PER_CHAR]);
    }
    okstruct = chkParity(parseData);
  }
  return okstruct;
}

static bool chkParity(uint8_t myData[])
{
  bool okparity = true; //Inocente hasta que se demuestre lo contrario
  for(uint8_t i = 0; myData[i]!=ES; ++i)
  {
    if(!(NUMBITS(myData[i])%2)) //No puede haber cant par de bits
      okparity = false;
  }
}

static uint8_t findSSIndex(void)
{
  for(uint8_t i = 0; i < TRACK2_BITLEN; ++i)
  {
    possibleSS = LS2MS(&cardData[i]);
    if(possibleSS == SS)
    {
      return i;
    }
  }
  return CHAR_NOT_FOUND;
}

static uint8_t findFSIndex(void)
{
  for(uint8_t i = 0; i < TRACK2_BITLEN; ++i)
  {
    possibleFS = LS2MS(&cardData[i]);
    if(possibleFS == FS)
    {
      return i;
    }
  }
  return CHAR_NOT_FOUND;
}

static uint8_t findESIndex(void)
{
  for(uint8_t i = 0; i < (TRACK2_BITLEN + 9*BITS_PER_CHAR); ++i)
  {
    possibleES = LS2MS(&cardData[i]);
    if(possibleES == ES)
    {
      return i;
    }
  }
  return CHAR_NOT_FOUND;
}
