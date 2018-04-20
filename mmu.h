#ifndef MMU
#define MMU

#include "helper_lib.h"
#include "linked_list.h"
#include "input_data_management.h"
#include "simtimer.h"

typedef struct MMU
{
   int segment;
   int memBase;
   int memAllocated;
}MMUStruct;

typedef struct MMUHelper
{
   ReturnMessage returnMessage;
   OutputStruct *output;
}MMUHelper;

MMUHelper *mmuAllocate( List*, MetaDataStruct*, ConfigStruct*,
                        OutputStruct*, Boolean, int, char* );

MMUHelper *mmuAccess( List*, MetaDataStruct*, ConfigStruct*,
                      OutputStruct*, Boolean, int, char* );

MMUHelper *createMMUHelper( void );

void destroyMMUHelper( MMUHelper* );

MMUStruct *createMMUStruct( int );

void destroyMMUStruct( MMUStruct* );

void destroyMMUList( List* );

OutputStruct* MMUStart( OutputStruct*, MMUStruct*, Boolean, char*, int, char* );

MMUHelper* MMUSuccess( OutputStruct*, Boolean, char*, int, char* );

MMUHelper* MMUError( OutputStruct*, Boolean, char*, int, char* );

#endif
