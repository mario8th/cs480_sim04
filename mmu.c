#include "mmu.h"

MMUHelper *mmuAllocate( List *mmu, MetaDataStruct *currMetaData,
                        ConfigStruct *conStruct, OutputStruct *currOutput,
                        Boolean togle, int process, char* timerString )
{
   int currBase;
   int currEnd;
   int newBase;
   int newEnd;
   char *message  = "Allocated";
   ListNode *currNode;
   MMUStruct *currMMU;
   MMUStruct *mmuStruct = createMMUStruct( currMetaData->cycleTime );

   currOutput = MMUStart( currOutput, mmuStruct, togle, message, process, timerString );

   if( mmuStruct->memBase + mmuStruct->memAllocated > conStruct->memorySize)
   {
      destroyMMUStruct( mmuStruct );
      return MMUError( currOutput, togle, message, process, timerString );
   }
   currNode = mmu->first;
   while( 1 )
   {
      if( currNode == NULL)
      {
         break;
      }
      else
      {
         currMMU = currNode->value;
      }
      if( currMMU->segment == mmuStruct->segment)
      {
         currBase = currMMU->memBase;
         currEnd = ( currMMU->memBase + currMMU->memAllocated );
         newBase =  mmuStruct->memBase;
         newEnd = ( mmuStruct->memBase + mmuStruct->memAllocated );
         if( ( ( currBase <= newBase ) && ( newBase <= currEnd ) ) ||
             ( ( currBase <= newEnd ) && ( newEnd <= currEnd ) ) ||
             ( ( newBase <= currBase ) && ( currEnd <= newEnd ) ) )
         {
            destroyMMUStruct( mmuStruct );
            return MMUError( currOutput, togle, message, process, timerString );
         }
      }
      currNode = currNode->next;
   }
   list_insert_after( mmu, list_create_node( mmuStruct ), NULL );
   return MMUSuccess( currOutput, togle, message, process, timerString );
}

MMUHelper *mmuAccess( List *mmu, MetaDataStruct *currMetaData,
                      ConfigStruct *conStruct, OutputStruct *currOutput,
                      Boolean togle, int process, char* timerString )
{
   int currBase;
   int currEnd;
   int newBase;
   int newEnd;
   char *message  = "Accessing";
   ListNode *currNode;
   MMUStruct *currMMU;
   MMUStruct *mmuStruct = createMMUStruct( currMetaData->cycleTime );

   currOutput = MMUStart( currOutput, mmuStruct, togle, message, process, timerString );

   if( ( mmuStruct->memBase + mmuStruct->memAllocated > conStruct->memorySize ) ||
         mmu->count == 0)
   {
      destroyMMUStruct( mmuStruct );
      return MMUError( currOutput, togle, message, process, timerString );
   }
   currNode = mmu->first;
   while( 1 )
   {
      if( currNode == NULL)
      {
         destroyMMUStruct( mmuStruct );
         return MMUError( currOutput, togle, message, process, timerString );
      }
      else
      {
         currMMU = currNode->value;
      }
      if( currMMU->segment == mmuStruct->segment)
      {
         currBase = currMMU->memBase;
         currEnd = ( currMMU->memBase + currMMU->memAllocated );
         newBase =  mmuStruct->memBase;
         newEnd = ( mmuStruct->memBase + mmuStruct->memAllocated );
         if( ( currBase <= newBase ) && ( newEnd <= currEnd ) )
         {
            break;
         }
      }
      currNode = currNode->next;
   }
   destroyMMUStruct( mmuStruct );
   return MMUSuccess( currOutput, togle, message, process, timerString );
}

MMUHelper *createMMUHelper( void )
{
   MMUHelper *mmuHelper = malloc( sizeof( MMUHelper ) );
   mmuHelper->returnMessage = 0;
   mmuHelper->output = NULL;
   return mmuHelper;
}

void destroyMMUHelper( MMUHelper *mmuHelper )
{
   free( mmuHelper );
}

MMUStruct *createMMUStruct( int memInfo )
{
   MMUStruct *mmuStruct = malloc( sizeof( MMUStruct ) );
   mmuStruct->segment = memInfo / 1000000;
   mmuStruct->memBase = ( memInfo / 1000 ) % 1000;
   mmuStruct->memAllocated = memInfo % 1000;
   return mmuStruct;
}

void destroyMMUStruct( MMUStruct *mmuStruct )
{
   free( mmuStruct );
}

void destroyMMUList( List *mmu )
{
   ListNode *currNode = mmu->first;
   while( 1 )
   {
      if( currNode == NULL)
      {
         break;
      }
      destroyMMUStruct( currNode->value );
      currNode = currNode->next;
   }
   list_destroy( mmu );
}

OutputStruct *MMUStart( OutputStruct *currOutput, MMUStruct* mmuStruct,
                        Boolean togle, char *message, int process, char *timerString )
{
   char outputString[80];
   currOutput->next = createOutputStruct();
   currOutput = currOutput->next;
   accessTimer( 1, timerString );
   snprintf( outputString, 80, "Time:%s, Process %d, MMU %s: %d/%d/%d\n",
             timerString, process, message,
             mmuStruct->segment, mmuStruct->memBase, mmuStruct->memAllocated );
   stringCopy( outputString, currOutput->value );
   print( outputString, togle );
   return currOutput;
}

MMUHelper* MMUSuccess( OutputStruct *currOutput, Boolean togle, char *message,
                       int process, char *timerString )
{
   char outputString[80];
   MMUHelper *mmuHelper = createMMUHelper();
   currOutput->next = createOutputStruct();
   currOutput = currOutput->next;
   accessTimer( 1, timerString );
   snprintf( outputString, 80, "Time:%s, Process %d, MMU %s: Success\n",
             timerString, process, message );
   stringCopy( outputString, currOutput->value );
   print( outputString, togle );
   mmuHelper->output = currOutput;
   mmuHelper->returnMessage = PASS;
   return mmuHelper;
}

MMUHelper* MMUError( OutputStruct *currOutput, Boolean togle, char *message,
                     int process, char *timerString )
{
   char outputString[80];
   MMUHelper *mmuHelper = createMMUHelper();
   currOutput->next = createOutputStruct();
   currOutput = currOutput->next;
   accessTimer( 1, timerString );
   snprintf( outputString, 80, "Time:%s, Process %d, MMU %s: Failed\n",
             timerString, process, message );
   stringCopy( outputString, currOutput->value );
   print( outputString, togle );
   mmuHelper->output = currOutput;
   mmuHelper->returnMessage = SEGFAULT;
   return mmuHelper;
}
