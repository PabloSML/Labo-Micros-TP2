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

#define ID_SIZE         8
#define PIN_MAXSIZE     5
#define INTENSITY_SIZE  1

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/
typedef enum
{
    DECODER_noev            = 0x00,
	DECODER_inputnum        = 0x01,
    DECODER_restart         = 0x02
  
} DecoderEvent_t;

typedef enum 
{
    DECODER_intesity        = 0x00,
    DECODER_pin             = 0x01,
    DECODER_id              = 0x02
} DecoderType_t;

/*******************************************************************************
 * VARIABLE PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/


//ingresas en que estado estas (Intensity, pin o id) y devuelve evento (noev, inputnum o restart)
DecoderEvent_t decoder(DecoderType_t dtype);





#endif // DECODER