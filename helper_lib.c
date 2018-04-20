// Program Information ////////////////////////////////
/***
   * @file helper_lib.c
   *
   * @brief simple helper methods for other files
   *
   * @details simple helper methods such as string checking
   *
   * @version 1.0
   *          C.S Student (30 January 2017)
   *          Initial Development and testing of helper_lib
   *
   * @Note Requires helper_lib.h
   *       Boolean enum created by Dr. Micheal Leverington
*/
#include <stdio.h>
#include <stdlib.h>
#include "helper_lib.h"

// checks if both strings are equal
Boolean stringComp( char *string1, char *string2 )
{
    int string1Len = stringLen( string1 );
    int string2Len = stringLen( string2 );

    if ( string1Len == string2Len )
    {
        for ( int count = 0; count < string1Len; count++ )
        {
            if ( string1[ count ] != string2[ count ] )
            {
                return False;
            }
        }
        return True;
    }
    return False;
}

// finds the length of a string
int stringLen( char *string )
{
    char currentChar = string[0];
    int counter = 0;
    while( currentChar != '\0' )
    {
        counter += 1;
        currentChar = string[ counter ];
    }
    return counter;
}

// copies string 1 into string 2
void stringCopy( char *string1, char *string2 )
{
    int length = stringLen( string1 );
    for ( int count = 0; count <= length; count++ )
    {
        string2[ count ] = string1[ count ];
    }
}

// creates output struct
OutputStruct *createOutputStruct( void )
{
   OutputStruct *output = malloc( sizeof( output ) );
   output->value = malloc( sizeof( char ) * 80 );
   return output;
}

// destroys output struct
void destroyOutputStruct( OutputStruct *output )
{
   if( output->next != NULL )
   {
      destroyOutputStruct( output->next );
   }
   free( output->value );
   free( output );
   return;
}

// prints if logto set to monitor or both
void print( char *string, Boolean togle )
{
   if( togle == True )
   {
      printf( "%s", string );
   }
}
