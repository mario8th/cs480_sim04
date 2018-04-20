// Program Information ////////////////////////////////
/***
   * @file input_data_management.c
   *
   * @brief Parser program for config and meta-data
   *
   * @details Parses and stores all the information from config.cnf files
   *          and metadata.mdf files
   *
   * @version 1.0
   *          C.S Student (30 January 2017)
   *          Initial Development and testing of input_data_management class
   *
   * @Note Requires input_data_management.h,
   *       helper_lib.h, helper_lib.c, linked_list.h, linked_list.c
   *       Linked List class created for NAU ee222
   *       Boolean enum created by Dr. Micheal Leverington
*/
#include <stdio.h>
#include <stdlib.h>
#include "helper_lib.h"
#include "input_data_management.h"
#include "linked_list.h"
#include <string.h>

// Parses and stores the config file
ReturnMessage openConfig( char *configPath, ConfigStruct *configStruct )
{
    ReturnMessage returnMessage;
    FILE *configFile;
    configFile = fopen( configPath, "r" );
    if ( configFile == NULL)
    {
        fclose( configFile );
        return NO_FILE_FOUND;
    }

    returnMessage = checkStringStart( configFile );
    if ( returnMessage == FILE_ERROR )
    {
        fclose( configFile );
        return FILE_ERROR;
    }
    returnMessage = checkStringVersion( configFile, configStruct );
    if ( returnMessage == FILE_ERROR )
    {
        fclose( configFile );
        return FILE_ERROR;
    }
    returnMessage = checkStringFile( configFile, configStruct );
    if ( returnMessage == FILE_ERROR )
    {
        fclose( configFile );
        return FILE_ERROR;
    }
    returnMessage = checkStringCPU( configFile, configStruct );
    if ( returnMessage == FILE_ERROR )
    {
        fclose( configFile );
        return FILE_ERROR;
    }
    returnMessage = checkStringQuantum( configFile, configStruct );
    if ( returnMessage == FILE_ERROR )
    {
        fclose( configFile );
        return FILE_ERROR;
    }
    returnMessage = checkStringMemory( configFile, configStruct );
    if ( returnMessage == FILE_ERROR )
    {
        fclose( configFile );
        return FILE_ERROR;
    }
    returnMessage = checkStringProcessor( configFile, configStruct );
    if ( returnMessage == FILE_ERROR )
    {
        fclose( configFile );
        return FILE_ERROR;
    }
    returnMessage = checkStringIO( configFile, configStruct );
    if ( returnMessage == FILE_ERROR )
    {
        fclose( configFile );
        return FILE_ERROR;
    }
    returnMessage = checkStringLogTo( configFile, configStruct );
    if ( returnMessage == FILE_ERROR )
    {
        fclose( configFile );
        return FILE_ERROR;
    }
    returnMessage = checkStringLogFile( configFile, configStruct );
    if ( returnMessage == FILE_ERROR )
    {
        fclose( configFile );
        return FILE_ERROR;
    }
    returnMessage = checkStringEnd( configFile );
    if ( returnMessage == FILE_ERROR )
    {
        fclose( configFile );
        return FILE_ERROR;
    }

    fclose( configFile );
    return PASS;
}

// Parses and stores meta data file
ReturnMessage openMetaData( char *metaDataPath, List *metaDataList )
{
    int loop;
    FILE *metaFile = fopen( metaDataPath, "r" );
    if ( metaFile == NULL )
    {
        return NO_FILE_FOUND;
    }
    loop = parseMetaLines( ':', metaDataList, metaFile );
    while ( loop == 1 )
    {
        loop = parseMetaLines( ';', metaDataList, metaFile );
    }
    fclose( metaFile );
    if ( loop == 2 )
    {
        return FILE_ERROR;
    }
    return PASS;
}

// Checks config string variables are as expected
CompReturnType compConfigVars( char *word, ConfigType type )
{
    if ( type == CPUSCHED )
    {
        if ( stringComp( word, "NONE" ) )
        {
            return SPECIAL;
        }
        else if ( ( stringComp( word, "FCFS-N" ) )
        || ( stringComp( word, "SJF-N" ) )
        || ( stringComp( word, "SRTF-P" ) )
        || ( stringComp( word, "FCFS-P" ) )
        || ( stringComp( word, "RR-P" ) ) )
        {
            return GOOD;
        }
        return BAD;
    }
    else if ( type == LOGTO )
    {
        if ( ( stringComp( word, "Monitor" ) )
        || ( stringComp( word, "File" ) )
        || ( stringComp( word, "Both" ) ) )
        {
            return GOOD;
        }
        return BAD;
    }
    return BAD;
}

// Check meta data values are as expected
ReturnMessage compMetaVars( char *word, char type )
{
    if ( ( type == 'S' ) || ( type == 'A') )
    {
        if ( ( stringComp( word, "start" ) ) || ( stringComp( word, "end" ) ) )
        {
            return PASS;
        }
    }
    else if ( type == 'P' )
    {
        if ( stringComp( word, "run" ) )
        {
            return PASS;
        }
    }
    else if ( type == 'I' )
    {
        if ( ( stringComp( word, "hard drive" ) )
            || ( stringComp ( word, "keyboard" ) ) )
        {
            return PASS;
        }
    }
    else if ( type == 'O' )
    {
        if ( ( stringComp( word, "hard drive" ) )
            || ( stringComp( word, "printer" ) )
            || ( stringComp( word, "monitor" ) ) )
        {
            return PASS;
        }
    }
    else if ( type == 'M' )
    {
        if ( ( stringComp( word, "allocate" ) )
            || ( stringComp( word, "access" ) ) )
        {
            return PASS;
        }
    }
    return FAIL;
}

// Parse each meta-data file line for info
int parseMetaLines( char expectedChar, List *metaList, FILE *metaFile )
{
    char line[ 50 ];
    char character;
    int count = 0;
    MetaDataStruct *metaStruct;
    ReturnMessage message;
    // Check for beggining line
    if ( expectedChar == ':' )
    {
        count = getNextLine( metaFile, line, expectedChar );
        if ( stringComp( line, "Start Program Meta-Data Code" ) )
        {
            return 1;
        }
        return 2;
    }

    if ( expectedChar == ';' )
    {
        skipWS( metaFile );
        line[0] = '\0';
        character = fgetc( metaFile );
        // Check for end of line
        if ( character == 'E' )
        {
            count = getNextLine( metaFile, line, '.' );
            if ( stringComp( line, "nd Program Meta-Data Code" ) )
            {
                return 0;
            }
            return 2;
        }
        // Check beginning comp letter
        if ( ( character != 'S' ) && ( character != 'A' )
        && ( character != 'P' ) && ( character != 'M' )
        && ( character != 'I' ) && ( character != 'O' ) )
        {
            return 2;
        }
        metaStruct = createMetaStruct();
        metaStruct->compLetter = character;
        // Check for operation
        if ( fgetc( metaFile ) == '(' )
        {
            count = getNextLine( metaFile, line, ')' );
            if ( count == 2 )
            {
                return 2;
            }
            message = compMetaVars( line, character );
            if ( message == FAIL )
            {
                return 2;
            }
            stringCopy( line, metaStruct->opString );
            // check for cycle time
            if ( fscanf( metaFile, "%d", &count ) != 1 )
            {
                return 2;
            }
            metaStruct->cycleTime = count;
            list_insert_after( metaList, list_create_node( metaStruct ), NULL );
        }

    }
    fgetc( metaFile );
    return 1;
}

// skips white space and new lines
void skipWS( FILE* fileHandle )
{
  char character = fgetc( fileHandle );
  while( 1 )
  {
      if ( ( character != ' ' ) && ( character != '\n' ) )
      {
          break;
      }
      character = fgetc( fileHandle );
  }
  ungetc( character, fileHandle );
}

// creates and mallocs a config struct
ConfigStruct *createConfigStruct( void )
{
    ConfigStruct *configStruct = malloc( sizeof( ConfigStruct ) );
    configStruct->version = 0;
    configStruct->fileName = malloc( sizeof( char ) * 20 );
    configStruct->cpuScheduleCode = malloc( sizeof( char ) * 20 );
    configStruct->quantumTime = 0;
    configStruct->memorySize = 0;
    configStruct->processCycle = 0;
    configStruct->ioCycle = 0;
    configStruct->logToCode = malloc( sizeof( char ) * 20 );
    configStruct->logFileName = malloc( sizeof( char ) * 20 );
    return configStruct;
}

// creates and mallocs a meta data struct
MetaDataStruct *createMetaStruct( void )
{
    MetaDataStruct *metaStruct = malloc( sizeof( MetaDataStruct ) );
    metaStruct->opString = malloc( sizeof( char ) * 20 );
    metaStruct->cycleTime = 0;
    return metaStruct;
}

// frees a config struct
void destroyConfigStruct( ConfigStruct *configStruct )
{

    free( configStruct->fileName );
    free( configStruct->cpuScheduleCode );
    free( configStruct->logToCode );
    free( configStruct->logFileName );
    free( configStruct );
}

// frees a meta data struct
void destroyMetaDataStruct( MetaDataStruct *metaStruct )
{
    free( metaStruct->opString );
    free( metaStruct );
}

void destroyMetaDataList( List *metaList )
{
  ListNode *tempNode = metaList->first;
  int listSize = metaList->count;
  int count;

  for( count = 0; count < listSize; count++ )
  {
    destroyMetaDataStruct( tempNode->value );
    tempNode = tempNode->next;
  }
}

// checks config start line
ReturnMessage checkStringStart( FILE *configFile )
{
    char word[20];
    char word2[20];
    char word3[20];
    if ( fscanf( configFile, "%s", word ) == 1 )
    {
        if ( stringComp( word, "Start" ) )
        {
            if ( fscanf( configFile, "%s %s %s", word, word2, word3 ) != 3 )
            {
                return FILE_ERROR;
            }
        }
    }
    return PASS;
}

// checks config version line
ReturnMessage checkStringVersion( FILE *configFile, ConfigStruct *configStruct )
{
    char word[20];
    float floatNum;
    int intNum;
    if ( fscanf( configFile, "%s", word ) == 1 )
    {
        if ( stringComp( word, "Version/Phase:" ) )
        {
            if ( fscanf( configFile, "%f", &floatNum ) != 1 )
            {
                return FILE_ERROR;
            }
            intNum = ( int )floatNum;
            if ( intNum >= 0 && intNum <= 10 )
            {
                configStruct->version = ( int )floatNum;
            }
            else
            {
                return FILE_ERROR;
            }
        }
    }
    return PASS;
}

// checks config file line
ReturnMessage checkStringFile( FILE *configFile, ConfigStruct *configStruct )
{
    char word[20];
    char word2[20];
    if ( fscanf( configFile, "%s", word ) == 1 )
    {
        if ( stringComp( word, "File" ) )
        {
            if ( ( fscanf( configFile, "%s %s", word, word2 ) != 2 ) || ( stringComp( word, "Path:" ) != True ) )
            {
                return FILE_ERROR;
            }
            stringCopy( word2, configStruct->fileName );
        }
    }
    return PASS;
}

// checks config CPU line
ReturnMessage checkStringCPU( FILE *configFile, ConfigStruct *configStruct )
{
    char word[20];
    char word2[20];
    char word3[20];
    CompReturnType returnType;
    if ( fscanf( configFile, "%s", word ) == 1 )
    {
        if ( stringComp( word, "CPU" ) )
        {
            if ( ( fscanf( configFile, "%s %s %s", word, word2, word3 ) != 3 )
                || ( stringComp( word, "Scheduling" ) != True ) || ( stringComp( word2, "Code:") != True ) )
            {
                return FILE_ERROR;
            }
            returnType = compConfigVars( word3, CPUSCHED );
            if ( returnType == GOOD )
            {
                stringCopy( word3, configStruct->cpuScheduleCode );
            }
            else if ( returnType == SPECIAL )
            {
                stringCopy( "FCFS-N", configStruct->cpuScheduleCode );
            }
            else
            {
                return FILE_ERROR;
            }
        }
    }
    return PASS;
}

// checks config Quantum line
ReturnMessage checkStringQuantum( FILE *configFile, ConfigStruct *configStruct )
{
    char word[20];
    char word2[20];
    int intNum;
    if ( fscanf( configFile, "%s", word ) == 1 )
    {
        if ( stringComp( word, "Quantum" ) )
        {
            if ( ( fscanf( configFile, "%s %s %d", word, word2, &intNum ) != 3 )
                || ( stringComp( word, "Time" ) != True ) || ( stringComp( word2, "(cycles):" ) != True ) )
                {
                    return FILE_ERROR;
                }
            if ( intNum >= 0 && intNum <= 100 )
            {
                configStruct->quantumTime = intNum;
            }
            else
            {
                return FILE_ERROR;
            }
        }
    }
    return PASS;
}

// checks config Memory line
ReturnMessage checkStringMemory( FILE *configFile, ConfigStruct *configStruct )
{
    char word[20];
    char word2[20];
    int intNum;
    if ( fscanf( configFile, "%s", word ) == 1 )
    {
        if ( stringComp( word, "Memory" ) )
        {
            if ( ( fscanf( configFile, "%s %s %d", word, word2, &intNum) != 3 )
                || ( stringComp( word, "Available" ) != True ) || ( stringComp( word2, "(KB):" ) != True ) )
                {
                    return FILE_ERROR;
                }
            if ( intNum >= 0 && intNum <= 1048576 )
            {
                configStruct->memorySize = intNum;
            }
            else
            {
                return FILE_ERROR;
            }
        }
    }
    return PASS;
}

// checks config Processor line
ReturnMessage checkStringProcessor( FILE *configFile, ConfigStruct *configStruct )
{
    char word[20];
    char word2[20];
    char word3[20];
    int intNum;
    if ( fscanf( configFile, "%s", word ) == 1 )
    {
        if ( stringComp( word, "Processor" ) )
        {
            if ( ( fscanf( configFile, "%s %s %s %d", word, word2, word3, &intNum ) != 4 )
                || ( stringComp( word, "Cycle" ) != True ) || ( stringComp( word2, "Time" ) != True )
                || ( stringComp( word3, "(msec):" ) != True ) )
                {
                    return FILE_ERROR;
                }

            if ( intNum >= 1 && intNum <= 1000 )
            {
                configStruct->processCycle = intNum;
            }
            else
            {
                return FILE_ERROR;
            }
        }
    }
    return PASS;
}

// checks config IO line
ReturnMessage checkStringIO( FILE *configFile, ConfigStruct *configStruct )
{
    char word[20];
    char word2[20];
    char word3[20];
    int intNum;
    if ( fscanf( configFile, "%s", word ) == 1 )
    {
        if ( stringComp( word, "I/O") )
        {
            if ( ( fscanf( configFile, "%s %s %s %d", word, word2, word3, &intNum) != 4 )
                || ( stringComp( word, "Cycle" ) != True ) || ( stringComp( word2, "Time" ) != True )
                || ( stringComp( word3, "(msec):" ) != True ) )
                {
                    return FILE_ERROR;
                }

            if ( intNum >= 1 && intNum <= 10000 )
            {
                configStruct->ioCycle = intNum;
            }
            else
            {
                return FILE_ERROR;
            }
        }
    }
    return PASS;
}

// checks config Log To line
ReturnMessage checkStringLogTo( FILE *configFile, ConfigStruct *configStruct )
{
    char word[20];
    CompReturnType returnType;
    if ( fscanf( configFile, "%s", word ) == 1 )
    {
        if ( stringComp( word, "Log" ) )
        {
            if ( fscanf( configFile, "%s", word ) != 1 )
            {
                return FILE_ERROR;
            }
            if ( stringComp( word, "To:" ) )
            {
                if ( fscanf( configFile, "%s", word ) != 1 )
                {
                    return FILE_ERROR;
                }
                returnType = compConfigVars( word, LOGTO );
                if (returnType == BAD )
                {
                    return FILE_ERROR;
                }
                stringCopy( word, configStruct->logToCode );
            }
        }
    }
    return PASS;
}

// checks config Log File line
ReturnMessage checkStringLogFile( FILE *configFile, ConfigStruct *configStruct )
{
    char word[20];
    char word2[20];
    if ( fscanf( configFile, "%s", word ) == 1 )
    {
        if ( stringComp( word, "Log" ) )
        {
            if ( fscanf( configFile, "%s", word ) != 1 )
            {
                return FILE_ERROR;
            }
            if ( stringComp( word, "File" ) )
            {
                if ( ( fscanf( configFile, "%s %s", word, word2 ) != 2 )
                || ( stringComp( word, "Path:" ) != True ) )
                {
                    return FILE_ERROR;
                }
                if ( stringComp( "Monitor", configStruct->logToCode ) )
                {
                    stringCopy( "none", configStruct->logFileName );
                }
                else
                {
                    stringCopy( word2, configStruct->logFileName );
                }
            }
        }
    }
    return PASS;
}

// checks config End line
ReturnMessage checkStringEnd( FILE *configFile )
{
    char word[20];
    char word2[20];
    char word3[20];
    if ( fscanf( configFile, "%s", word ) == 1 )
    {
        if ( stringComp( word, "End") )
        {
            if ( ( fscanf( configFile, "%s %s %s", word, word2, word3 ) != 3 )
                || ( stringComp( word, "Simulator" ) != True )
                || ( stringComp( word2, "Configuration" ) != True )
                || ( stringComp( word3, "File." ) != True ) )
            {
                return FILE_ERROR;
            }
        }

        else
        {
            return FILE_ERROR;
        }
    }
    return PASS;
}

// Grabs a string from a file ending in the expectedChar
int getNextLine( FILE *fileHandle, char *line, char expectedChar )
{
    char currentChar;
    int count = 0;
    while ( ( currentChar != expectedChar ) )
    {
        currentChar = fgetc( fileHandle );
        line[ count ] = currentChar;
        count++;
        if( ( count == 49 ) )
        {
            return 2;
        }
    }
    line[--count] = '\0';
    return 1;
}
