// Program Information ////////////////////////////////
/***
   * @file input_data_management.h
   *
   * @brief header file for input_data_management
   *
   * @details Specifies functions, structs and enums for input_data_management
   *
   * @version 2.0
   *          C.S Student (30 January 2017)
   *          Initial Development and testing of input_data_management header
   *
   * @Note Requires input_data_management.c
   *       helper_lib.h, helper_lib.c, linked_list.h, linked_list.c
   *       Linked List class created for NAU ee222
   *       Boolean enum created by Dr. Micheal Leverington
*/

#ifndef INPUT_DATA_H
#define INPUT_DATA_H

#include <stdio.h>
#include <stdlib.h>
#include "helper_lib.h"
#include "linked_list.h"

// enum to determine type of string being checked
typedef enum { CPUSCHED, LOGTO } ConfigType;

typedef enum { STARTEND, RUN, INPUT, OUTPUT } MetaType;

// enum to determine comparison return type
typedef enum { BAD, GOOD, SPECIAL } CompReturnType;

// structure to hold config info
typedef struct ConfigStruct
{
    int version;
    char *fileName;
    char *cpuScheduleCode;
    int quantumTime;
    int memorySize;
    int processCycle;
    int ioCycle;
    char *logToCode;
    char *logFileName;
} ConfigStruct;

// sturcture to hold meta data info
typedef struct MetaDataStruct
{
    char compLetter;
    char *opString;
    int cycleTime;
} MetaDataStruct;

ReturnMessage openConfig( char *, ConfigStruct * );

ReturnMessage openMetaData( char *, List * );

CompReturnType compConfigVars( char *, ConfigType );

ReturnMessage compMetaVars( char *, char );

int parseMetaLines( char, List *, FILE * );

void skipWS( FILE * );

ConfigStruct *createConfigStruct( void );

MetaDataStruct *createMetaStruct( void );

void destroyConfigStruct( ConfigStruct * );

void destroyMetaDataStruct( MetaDataStruct * );

void destroyMetaDataList( List * );

ReturnMessage checkStringStart( FILE * );

ReturnMessage checkStringVersion( FILE *, ConfigStruct * );

ReturnMessage checkStringFile( FILE *, ConfigStruct * );

ReturnMessage checkStringCPU( FILE *, ConfigStruct * );

ReturnMessage checkStringQuantum( FILE *, ConfigStruct * );

ReturnMessage checkStringMemory( FILE *, ConfigStruct * );

ReturnMessage checkStringProcessor( FILE *, ConfigStruct * );

ReturnMessage checkStringIO( FILE *, ConfigStruct * );

ReturnMessage checkStringLogTo( FILE *, ConfigStruct * );

ReturnMessage checkStringLogFile( FILE *, ConfigStruct * );

ReturnMessage checkStringEnd( FILE * );

int getNextLine( FILE *, char *, char );

#endif
