/***************************************************************************//**
  @file     magnetic_reader.h
  @brief    Magnetic Reader Driver
  @author   Grupo 4
 ******************************************************************************/

#ifndef MAGNETIC__READER_H_
#define MAGNETIC__READER_H_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include <stdint.h>
#include <stdbool.h>

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

//Estructura de datos del TRACK2
typedef struct {
  uint8_t PAN[19]; //Primary Account No.
  uint8_t PANLength;
  uint8_t additionalData[7]; //4: Date(YYMM) + 3: Service Code
  uint8_t discretionaryData[8]; //?
} card_data_format;

//Eventos del lector de tarjeta
typedef enum
{
  MAGREADER_noev            = 0x00,
	MAGREADER_cardsliding     = 0x01, //La tarjeta esta pasando
	MAGREADER_carderror				= 0x02, //Hubo un error en la lectura
	MAGREADER_cardUpload 			= 0x03 //La lectura fue un exito!!

} MagReaderEvent_t;
/*******************************************************************************
 * VARIABLE PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

 /**
 * @brief Initialize magnetic reader
 */
void magneticReaderInit(void);
/**
* @brief Returns de last saved Data Track
*/
card_data_format getFullData(void);
/**
* @brief Returns de PAN Data
*/
uint8_t * getPAN(void);
/**
* @brief Returns de PAN Data len
*/
uint8_t getPANlen(void);
/**
* @brief Borra el todo lo guardado por timeout
*/
void dataTimeOut(void);
/**
 * @brief Query event
 * @return True if event, false if not event
 */
bool magreader_hasEvent(void);
/**
 * @brief Get magreader event
 * @return NOEV, CARDSLIDING, CARDERROR, CARDUPLOAD
 */
MagReaderEvent_t magreader_getEvent(void);

#endif // MAGNETIC__READER_H_
