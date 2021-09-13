/***************************************************************************//**
  @file     magnetic_reader.h
  @brief    Decoder
  @author   Grupo 4
 ******************************************************************************/

#ifndef DECODER_H_
#define DECODER_H_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include <stdint.h>
#include <stdbool.h>

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define ID_SIZE             8
#define PIN_MAXSIZE         5
#define INTENSITY_SIZE      1


#define INTENSITY_MINVALUE      1
#define INTENSITY_MAXVALUE      3
#define ID_MINDIGITNUMBER       0
#define ID_MAXDIGITNUMBER       9
#define PIN_MINDIGITNUMBER      0
#define PIN_MAXDIGITNUMBER      9



/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/
typedef enum
{
    DECODER_noev            = 0x00,
	DECODER_inputnum        = 0x01,
    DECODER_restart         = 0x02,
    DECODER_inputerror      = 0X03,
    DECODER_inputclick      = 0x04
} DecoderEvent_t;

typedef enum 
{
    DECODER_intensity       = 0x00,
    DECODER_id              = 0x01,
    DECODER_pin             = 0x02,
    DECODER_invalid         = 0x03,
    DECODER_open            = 0x04
} DecoderType_t;

/*******************************************************************************
 * VARIABLE PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/**
 * @brief Initialize decoder
 */
bool decoderInit(void);

//ingresas en que estado estas (Intensity, pin o id) y devuelve evento (noev, inputnum o restart)

/**
 * @brief Constructor, input a DecoderType to get events
 * @param dtype DecoderType_t (Intesity, ID, PIN, ...)
 */
void decoder(DecoderType_t dtype);

/**
 * @brief Check events from drivers
 * @return true if there's an event, otherwise return false
 */
bool decoder_hasEvent(void);

/**
 * @brief Get decoder event DecoderEvent_t
 * @return Return a DecoderEvent_t to process
 */
DecoderEvent_t decoder_getEvent(void);

/**
 * @brief Get number (ID, PIN)
 * @return return whole number
 */
int64_t decoder_getNumber(void);




#endif // DECODER