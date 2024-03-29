/* Required Header Stuff
1) Creating my own waiting loop that does not depend on Dr. Leverington's allows
   Multithreading
2) Memory management Is included
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "helper_lib.h"
#include "linked_list.h"
#include "input_data_management.h"
#include "simtimer.h"
#include "simulator.h"

ReturnMessage errorHandling( ReturnMessage );

int main( int argc, char* argv[] )
{
    ReturnMessage returnMessage;
    char *fileHandle = argv[1];
    char *metaHandle;
    ConfigStruct *conStruct = createConfigStruct();
    List *metaList = list_create();

    // Check if all input parameters present
    if ( argc != 2 )
    {
        printf("Missing file(s)\n");
        return 1;
    }

    // Open and parse the config file
    // Store into config structure
    // Check if errors occured
    returnMessage = openConfig( fileHandle, conStruct );
    returnMessage = errorHandling( returnMessage );
    if ( returnMessage == FAIL )
    {
        return 1;
    }
    // Open and parse the meta data file
    // Store into linked list with nodes conatining meta data stuctures
    metaHandle = conStruct->fileName;
    returnMessage = openMetaData( metaHandle, metaList );
    returnMessage = errorHandling( returnMessage );
    if ( returnMessage == FAIL )
    {
        return 1;
    }

    // Run the simulator
    startSimulator( conStruct, metaList );

    destroyMetaDataList( metaList );
    list_destroy( metaList );
    destroyConfigStruct( conStruct );

    return 0;

}

// Return error message depending on error
ReturnMessage errorHandling( ReturnMessage message )
{
    if ( message == FAIL )
    {
        printf( "An unknown error occured\n" );
    }
    else if ( message == FILE_ERROR )
    {
        printf( "There was an error in the config or meta_data file(s)\n" );
    }
    else if ( message == NO_FILE_FOUND )
    {
        printf( "File Not Found\n" );
    }
    else
    {
        return PASS;
    }
    return FAIL;
}
