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