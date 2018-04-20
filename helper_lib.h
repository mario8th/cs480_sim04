// Program Information ////////////////////////////////
/***
   * @file helper_lib.h
   *
   * @brief header file for helper_lib
   *
   * @details Specifies functions, structs and enums for helper_lib.c
   *
   * @version 2.0
   *          C.S Student (30 January 2017)
   *          Initial Development and testing of helper_lib
   *
   * @Note Requires helper_lib.c
   *       Boolean enum created by Dr. Micheal Leverington
*/

#ifndef HELPER_LIB_H
#define HELPER_LIB_H

// holds all the strings to write the output
struct OutputStruct;

typedef struct OutputStruct
{
   struct OutputStruct *next;
   char *value;
}OutputStruct;

typedef enum { False, True } Boolean;

// return messages/ error messages
typedef enum { PASS, FAIL, FILE_ERROR, NO_FILE_FOUND,
               PCB_CREATE_ERROR, PCB_ERROR, SEGFAULT } ReturnMessage;

Boolean stringComp( char *, char * );

int stringLen( char * );

void stringCopy( char *, char * );

OutputStruct *createOutputStruct( void );

void destroyOutputStruct( OutputStruct* );

void print( char*, Boolean );

#endif
