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
#include "magnetic_reader_drv.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define PIN_MagEnable   PIN_MAG_EN 	//Enable (Low while card is sliding)
#define PIN_MagData     PIN_MAG_DT 	//Data
#define PIN_MagClk      PIN_MAG_CLK	//Clock

#define MAX_CHAR_LEN 			40
#define BITS_PER_CHAR	  	5
#define TRACK2_BITLEN			MAX_CHAR_LEN * BITS_PER_CHAR

#define SS                (uint8_t)0x0B
#define FS                (uint8_t)0x0D
#define ES                (uint8_t)0x1F
#define CHAR_NOT_FOUND    TRACK2_BITLEN
//#define LS2MS(data)     (uint8_t)((data[4]<<4) + (data[3]<<3) + (data[2]<<2) + (data[1]<<1) + data[0])
//#define NUMBITS(data)   (uint8_t)(((data&16)>>4) + (data&8)>>3) + ((data&4)>>2) + ((data&2)>>1) + (data&1))
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
static void     uploadCardData(void);
static void     clrRawData(void);
static void     clrCardData(void);
static uint8_t  reverseChar(bool data[]);
static uint8_t  numBits(uint8_t data);

/*******************************************************************************
 * ROM CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

static uint32_t          bitCounter = 0;
static bool              rawCardData[TRACK2_BITLEN + 10*BITS_PER_CHAR]; //Le agrego 10 extra por si acaso
static uint8_t           parsedRawData[MAX_CHAR_LEN];
static bool              validCardData = false;
static card_data_format  cardData;
static MagReaderEvent_t  ev = MAGREADER_noev;

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
  irq_id_t idE = irqGetId(PIN_MagEnable);
  irq_id_t idClk = irqGetId(PIN_MagClk);
  gpioIRQ(PIN_MagEnable, PORT_eInterruptEither,  idE, magReaderHandler);
  gpioIRQ(PIN_MagClk   , PORT_eInterruptFalling, idClk, readData); //Los datos cambian en posedge
}

bool magreader_hasEvent(void)
{
  return ev;
}

MagReaderEvent_t magreader_getEvent(void)
{
  MagReaderEvent_t  temp_ev = ev;
  ev = MAGREADER_noev;
  return temp_ev;
}

card_data_format getFullData(void)
{
  return cardData;
}

void dataTimeOut(void)
{
  bitCounter = 0; //Reinicio mi contador
  validCardData = false;
  clrRawData();
  clrCardData();
}

uint8_t * getPAN(void)
{
  return cardData.PAN;
}

uint8_t getPANlen(void)
{
  return cardData.PANLength;
}

uint8_t* magreader_getRawData(void)
{
  return parsedRawData;
}
/*******************************************************************************
*                     LOCAL FUNCTION DEFINITIONS
*******************************************************************************/

static void magReaderHandler(void)
{
  if(gpioRead(PIN_MagEnable))
  {
    bitCounter = 0; //Reinicio mi contador

    bool isdataOK = parseData();

    if(isdataOK)
    {
      ev = MAGREADER_cardUpload;
      // uploadCardData();
      // clrRawData();
    }
    else
    {
      ev = MAGREADER_carderror;
    }
  }
  else //Flanco descendente -> esta pasando
  {
    ev = MAGREADER_cardsliding;
  }
}

static void readData(void)
{
  if(!gpioRead(PIN_MagEnable))
  {
    rawCardData[bitCounter] = !gpioRead(PIN_MagData); // /DATA
    bitCounter++;
  }
}

static void uploadCardData(void)
{
  validCardData = true;
  uint8_t i = 0;
  //Cargo PAN
  for(i = 0; parsedRawData[i] != FS; ++i)
    cardData.PAN[i] = (parsedRawData[1 + i] & 0x0F);
  //Cargo largo PAN
  cardData.PANLength = i;
  //Cargo Additional Data
  for(i = 0; i < 7; ++i)
    cardData.additionalData[i] = parsedRawData[cardData.PANLength + 2 + i];
  //Cargo Discretionary Data
  for(i = 0; i < 8; ++i)
    cardData.discretionaryData[i] = parsedRawData[cardData.PANLength + 9 + i];
}

static bool parseData(void)
{
  bool okstruct = true;
  uint8_t SSindex = findSSIndex();
  // uint8_t FSindex = findFSIndex();
  // uint8_t ESindex = findESIndex();
  if(SSindex == CHAR_NOT_FOUND /*|| FSindex == CHAR_NOT_FOUND || ESindex == CHAR_NOT_FOUND*/)
  {
    okstruct = false;
  }
  else
  {
    // uint8_t PANlen = (FSindex - SSindex)/BITS_PER_CHAR - 1; //Resto SS
    // uint8_t realTracklen = MAX_CHAR_LEN - 19 + PANlen; //el largo de PAN es variable

    for(uint8_t char_num = 0; char_num < MAX_CHAR_LEN; ++char_num)
    {
      parsedRawData[char_num] = reverseChar(&rawCardData[SSindex + char_num*BITS_PER_CHAR + 1]);
    }
    // okstruct = chkParity(parsedRawData);
  }
  return okstruct;
}

static bool chkParity(uint8_t myData[])
{
  bool okparity = true; //Inocente hasta que se demuestre lo contrario
  for(uint8_t i = 0; myData[i]!=ES; ++i)
  {
    if(!(numBits(myData[i])%2)) //No puede haber cant par de bits
      okparity = false;
  }
  return okparity;
}

static uint8_t findSSIndex(void)
{
	uint8_t possibleSS;
  for(uint8_t i = 0; i < TRACK2_BITLEN; ++i)
  {
    possibleSS = reverseChar(&rawCardData[i]);
    if(possibleSS == SS)
    {
      return i;
    }
  }
  return CHAR_NOT_FOUND;
}

static uint8_t findFSIndex(void)
{
	uint8_t possibleFS;
  for(uint8_t i = 0; i < TRACK2_BITLEN; ++i)
  {
    possibleFS = reverseChar(&rawCardData[i]);
    if(possibleFS == FS)
    {
      return i;
    }
  }
  return CHAR_NOT_FOUND;
}

static uint8_t findESIndex(void)
{
	uint8_t possibleES;
  for(uint8_t i = 0; i < (TRACK2_BITLEN + 9*BITS_PER_CHAR); ++i)
  {
    possibleES = reverseChar(&rawCardData[i]);
    if(possibleES == ES)
    {
      return i;
    }
  }
  return CHAR_NOT_FOUND;
}

static void clrRawData(void)
{
  for (uint8_t i = 0; i < TRACK2_BITLEN + 10*BITS_PER_CHAR;++i)
    rawCardData[i] = false;
  for(uint8_t i = 0; i < MAX_CHAR_LEN; ++i)
    parsedRawData[i] = 0;
}

static void clrCardData(void)
{
	uint8_t i = 0;
	  //Borro PAN
	  for(i = 0; i < 19; ++i)
	    cardData.PAN[i] = 0;
	  //Borro largo PAN
	  cardData.PANLength = 0;
	  //Borro Additional Data
	  for(i = 0; i < 7; ++i)
	    cardData.additionalData[i] = 0;
	  //Borro Discretionary Data
	  for(i = 0; i < 8; ++i)
	    cardData.discretionaryData[i] = 0;
}

static uint8_t  reverseChar(bool data[])
{
  return((data[4]<<4) + (data[3]<<3) + (data[2]<<2) + (data[1]<<1) + data[0]);
}

static uint8_t  numBits(uint8_t data)
{
  return(((data&16)>>4) + ((data&8)>>3) + ((data&4)>>2) + ((data&2)>>1) + (data&1));
}
