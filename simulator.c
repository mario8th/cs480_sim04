// Program Information ////////////////////////////////
/***
   * @file simulator.c
   *
   * @brief Runs a simulation of an OS
   *
   * @details Parses metaData files into a PCB list/struct
   *          Using a timer and pthreads simulates running each process
   *
   * @version 1.0
   *          C.S Student (26 February 2018)
   *          Initial Development and testing of simulator class
   *
   * @Note Requires simulator.h,
   *       helper_lib.h, helper_lib.c, linked_list.h, linked_list.c
   *       Linked List class created for NAU ee222
   *       Boolean enum created by Dr. Micheal Leverington
   *       simtimer created by Dr. Micheal Leverington
*/
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "simtimer.h"
#include "simulator.h"

// Starts running the simulator on the metaList data
ReturnMessage startSimulator( ConfigStruct *conStruct, List *metaList )
{
   List *pcb = list_create();
   List *mmu = list_create();
   OutputStruct *outputList = createOutputStruct();
   OutputStruct *currOutput = outputList;
   ReturnMessage rMessage;
   Boolean togle;
   char timerString[15];
   char outputString[80];

   if( stringComp( conStruct->logToCode, "File" ) )
   {
      togle = False;
   }
   else
   {
      togle = True;
   }

   accessTimer( 0, timerString );
   snprintf( outputString, 80, "Time:%s, System Start\n", timerString );
   stringCopy( outputString, currOutput->value );
   print( outputString, togle );

   currOutput = configureOutput( currOutput, timerString,
                                 "Time:%s, Begin PCB Creation\n", -1, NULL,
                                 NULL, -1, togle );

   // Creates PCB, checks for errors
   rMessage = createPCB( conStruct, metaList, pcb );
   if( rMessage != PASS )
   {
      return rMessage;
   }

   currOutput = configureOutput( currOutput, timerString,
                                 "Time:%s, All Processes Initiated in New State\n",
                                 -1, NULL, NULL, -1, togle );

   // Sets PCB states to Ready
   setPCBReady( pcb->first->value );

   currOutput = configureOutput( currOutput, timerString,
                                 "Time:%s, All Processes Initiated in Ready State\n",
                                 -1, NULL, NULL, -1, togle );

   // Runs simulator using PCB, checks for errors
   currOutput = runSimulator( pcb, currOutput, timerString, togle, conStruct, mmu );

   currOutput = configureOutput( currOutput, timerString,
                                 "Time:%s, System Stop\n",
                                 -1, NULL, NULL, -1, togle );

   // If File or both, write to file
   if( stringComp( conStruct->logToCode, "Monitor" ) != True )
   {
      writeOutput( outputList, conStruct );
   }

   destroyPCB( pcb->first->value );
   list_destroy( pcb );
   destroyMMUList( mmu );
   destroyOutputStruct( outputList );

   return PASS;
}

// runs the simulator
OutputStruct *runSimulator( List *pcb, OutputStruct *currOutput,
                           char *timerString, Boolean togle,
                           ConfigStruct *conStruct, List *mmu )
{
   List *interruptQueue = list_create();
   ListNode *currProcess;
   ProcessControlBlock *currOP;
   Semaphores *semaphores = createSemaphores();
   currProcess = chooseNextProcess( pcb, conStruct, interruptQueue );
   currOP = currProcess->value;
   while( 1 )
   {
      //Select Process
      if( currOP->processNum != -1 )
      {
         currOutput = configureOutput( currOutput, timerString,
                  "Time:%s, %s Strategy selects Process %d with time: %d mSec\n",
                  currOP->processNum, NULL, conStruct->cpuScheduleCode,
                  currOP->processTime, togle );

         currOutput = configureOutput( currOutput, timerString,
                                    "Time:%s, Process %d set in Running State\n",
                              currOP->processNum, NULL, NULL, -1, togle );
         currOutput = runProcess( currProcess, currOP, currOutput, timerString,
                                  semaphores, togle, conStruct, mmu, interruptQueue );
         currOutput = configureOutput( currOutput, timerString,
                                     "Time:%s, Process %d, set in Exit State\n",
                            currOP->processNum, NULL, NULL, -1, togle );
      }
      else
      {
         currOP->state = EXIT;
      }
      currProcess = chooseNextProcess( pcb, conStruct, interruptQueue );
      // Change this so it waits for interrupt if needed
      // But if it's there's no more processes then break
      if( currProcess == NULL && interruptQueue->count == 0 )
      {
         break;
      }
      currOP = currProcess->value;
   }
   return currOutput;
}

// runs a specific process
OutputStruct *runProcess( ListNode *currProcess, ProcessControlBlock *currOP,
                          OutputStruct *currOutput, char *timerString,
                          Semaphores *semaphores, Boolean togle,
                          ConfigStruct *conStruct, List *mmu, List *interruptQueue )
{
   pthread_t tid;
   pthread_attr_t attr;
   MMUHelper *mmuHelper;
   ReturnMessage returnMessage;
   IOHelper *ioHelper;
   ProcessControlBlock *cpValue = currProcess->value;
   int waitTime;

   while ( 1 )
   {
      while( currOP->state == EXIT )
      {
         currOP = currOP->next;
      }
      if( currOP->current->compLetter == 'I' )
      {
         ioHelper = createIOHelper( currOutput, timerString, currProcess,
                                    currOP, semaphores, interruptQueue, togle );
         setPCBState( cpValue, BLOCKED, currOP->processNum + 1 );
         pthread_attr_init(&attr);
         pthread_create( &tid, &attr, runIO, ioHelper );
         if( stringComp( conStruct->cpuScheduleCode, "SJF-N" ) == True ||
             stringComp( conStruct->cpuScheduleCode, "FCFS-N" ) == True )
         {
            pthread_join( tid, NULL );
         }
         else
         {
            return currOutput;
         }
      }
      else if( currOP->current->compLetter == 'O' )
      {
         ioHelper = createIOHelper( currOutput, timerString, currProcess,
                                    currOP, semaphores, interruptQueue, togle );
         setPCBState( cpValue, BLOCKED, currOP->processNum + 1 );
         pthread_attr_init(&attr);
         pthread_create( &tid, &attr, runIO, ioHelper );
         if( stringComp( conStruct->cpuScheduleCode, "SJF-N" ) == True ||
             stringComp( conStruct->cpuScheduleCode, "FCFS-N" ) == True )
         {
            pthread_join( tid, NULL );
         }
         else
         {
            return currOutput;
         }
      }
      else if( currOP->current->compLetter == 'M' )
      {
         if( stringComp(currOP->current->opString, "allocate") == True )
         {
            while( semaphores->memoryAllocate == 0 )
            {
               // wait
            }
            semaphores->memoryAllocate = 0;
            mmuHelper = mmuAllocate( mmu, currOP->current, conStruct,
                                     currOutput, togle,
                                     currOP->processNum, timerString );
            currOutput = mmuHelper->output;
            returnMessage = mmuHelper->returnMessage;
            destroyMMUHelper( mmuHelper );
            if( returnMessage == SEGFAULT )
            {
               currOutput = configureOutput( currOutput, timerString,
                     "Time:%s, Process %d, Segmentation Fault - Process ended\n",
                     currOP->processNum, NULL, NULL, -1, togle );
            }
         }
         else
         {
            mmuHelper = mmuAccess( mmu, currOP->current, conStruct,
                                    currOutput, togle,
                                    currOP->processNum, timerString );
            currOutput = mmuHelper->output;
            returnMessage = mmuHelper->returnMessage;
            destroyMMUHelper( mmuHelper );
            if( returnMessage == SEGFAULT )
            {
               currOutput = configureOutput( currOutput, timerString,
                     "Time:%s, Process %d, Segmentation Fault - Process ended\n",
                     currOP->processNum, NULL, NULL, -1, togle );
            }
         }
         semaphores->memoryAllocate = 1;
      }
      else if( currOP->current->compLetter == 'P' )
      {
         currOutput = configureOutput( currOutput, timerString,
                                       "Time:%s, Process %d, %s Operation Start\n",
                                       currOP->processNum,
                                       currOP->current->opString,
                                       NULL, -1, togle );
         while( 1 )
         {
            if( stringComp( conStruct->cpuScheduleCode, "SJF-N" ) == False &&
                stringComp( conStruct->cpuScheduleCode, "FCFS-N" ) == False )
            {
               if( currOP->processTime < conStruct->quantumTime * conStruct->processCycle )
               {
                  waitTime = currOP->processTime;
               }
               else
               {
                  waitTime = conStruct->quantumTime * conStruct->processCycle;
               }
               cpValue->processTime -= waitTime;
               currOP->processTime -= waitTime;
            }
            waitProcess( waitTime, timerString );
            if( currOP->processTime == 0 || interruptQueue->count > 0 )
            {
               break;
            }
         }

         currOutput = configureOutput( currOutput, timerString,
                                       "Time:%s, Process %d, %s Operation End\n",
                                       currOP->processNum,
                                       currOP->current->opString,
                                       NULL, -1, togle );
      }

      currOP->state = EXIT;
      if( currOP->next == NULL)
      {
         return currOutput;
      }
      if( currOP->next->processNum != currOP->processNum )
      {
         return currOutput;
      }
      currOP = currOP->next;
   }
}

void *runIO( void *voidIOHelper )
{
   char *outputStart;
   char *outputEnd;
   int semaphore;
   IOHelper *ioHelper = voidIOHelper;
   OutputStruct *currOutput = ioHelper->currOutput;
   char *timerString = ioHelper->timerString;
   ListNode *currProcess = ioHelper->currProcess;
   ProcessControlBlock *currOP = ioHelper->currOP;
   Semaphores *semaphores = ioHelper->semaphores;
   List* interruptQueue = ioHelper->interruptQueue;
   Boolean togle = ioHelper->togle;
   ProcessControlBlock *cpValue = currProcess->value;
   if( currOP->current->compLetter == 'I')
   {
      outputStart = "Time:%s, Process %d, %s Input Start\n";
      outputEnd = "Time:%s, Process %d, %s Input End\n";
      semaphore = semaphores->input;
   }
   else
   {
      outputStart = "Time:%s, Process %d, %s Output Start\n";
      outputEnd = "Time:%s, Process %d, %s Output End\n";
      semaphore = semaphores->output;
   }

   currOutput = configureOutput( currOutput, timerString, outputStart,
                                 currOP->processNum, currOP->current->opString,
                                 NULL, -1, togle );
   cpValue->processTime = cpValue->processTime - currOP->processTime;
   while( semaphore == 0 )
   {
      // wait
   }
   semaphores->input = 0;
   waitProcess( currOP->processTime, timerString );
   semaphores->input = 1;
   currOutput = configureOutput( currOutput, timerString, outputEnd,
                                 currOP->processNum, currOP->current->opString,
                                 NULL, -1, togle );
   // put into interrupt queue
   currOP->state = EXIT;
   setPCBState( cpValue, READY, cpValue->processNum + 1 );
   while( semaphores->modifyInterrupt == 0 )
   {
      // wait
   }
   semaphores->modifyInterrupt = 0;
   list_insert_after( interruptQueue, currProcess, NULL );
   semaphores->modifyInterrupt = 1;
   destroyIOHelper( ioHelper );
   return currOutput;
}

void waitProcess( int waitTime, char *timerString )
{
   double finalTime = accessTimer( 1, timerString ) + ( (double)waitTime / 1000 );
   while( accessTimer( 1, timerString ) < finalTime )
   {
      //wait
   }
   return;
}

OutputStruct *configureOutput( OutputStruct *currOutput, char *timerString,
                               char* printString, int process, char *opString,
                               char* scheduleCode, int processTime, Boolean togle )
{
   char outputString[80];
   currOutput->next = createOutputStruct();
   currOutput = currOutput->next;
   accessTimer( 1, timerString );
   if( process == -1 )
   {
      snprintf( outputString, 80, printString, timerString );
   }
   else if( scheduleCode != NULL )
   {
      snprintf( outputString, 80, printString, timerString,
                scheduleCode, process, processTime );
   }
   else if( opString != NULL )
   {
      snprintf( outputString, 80, printString, timerString, process, opString );
   }
   else
   {
      snprintf( outputString, 80, printString, timerString, process );
   }
   stringCopy( outputString, currOutput->value );
   print( outputString, togle );
   return currOutput;
}

// Writes saved output from pcb to file
void writeOutput( OutputStruct *outputList, ConfigStruct *conStruct )
{
   FILE *outputFile;
   OutputStruct *currNode;

   if( outputList->value == NULL )
   {
      return;
   }
   outputFile = fopen( conStruct->logFileName, "w" );
   currNode = outputList;
   while( 1 )
   {
      fputs( currNode->value, outputFile );
      if( currNode->next == NULL )
      {
         break;
      }
      currNode = currNode->next;
   }
   fclose( outputFile );
   return;
}

// creates the pcb
// side note
// The PCB works on a two levels, a linked list that saves the pcbNode for each
// process beginning, and a pcb linked list that holds every part of every
// process. This is mainly for sim04 interrupts
ReturnMessage createPCB( ConfigStruct *configStruct, List *metaList, List *pcbList )
{
   MetaDataStruct *currMeta;
   ListNode *currNode = metaList->first;
   ProcessControlBlock *currPCB = createPCBStruct();
   ProcessControlBlock *currProcess;
   currMeta = currNode->value;
   setPCBFirst( currMeta, pcbList, currPCB );

   while( 1 )
   {
      currNode = currNode->next;
      currMeta = currNode->value;

      if( ( currMeta->compLetter == 'I' ) ||
          ( currMeta->compLetter == 'O') )
      {
         setPCB( currMeta, currPCB, configStruct->ioCycle );
      }
      else if( currMeta->compLetter == 'M' )
      {
         setPCB( currMeta, currPCB, 0 );
      }
      else if( currMeta->compLetter == 'P' )
      {
         setPCB( currMeta, currPCB, configStruct->processCycle );
      }
      else if( currMeta->compLetter == 'A' )
      {
         if( stringComp( currMeta->opString, "start" ) )
         {
            setPCBProcess( currMeta, pcbList, currPCB );
            currProcess = pcbList->last->value;
         }
         else
         {
            setPCB( currMeta, currPCB, 0 );
         }
      }
      else if( currMeta->compLetter == 'P' )
      {
         setPCBEndcap( currMeta, pcbList, currPCB );
      }

      if( currNode->next == NULL )
      {
         break;
      }
      currProcess->processTime += currPCB->processTime;
      currPCB = currPCB->next;
   }
   return 0;
}

// sets up the first node in the PCB linked list
void setPCBFirst( MetaDataStruct *currMeta, List *pcbList,
                  ProcessControlBlock *currPCB )
{
   currPCB->current = currMeta;
   currPCB->processTime = 0;
   currPCB->processNum = -1;
   currPCB->state = NEW;
   list_insert_after( pcbList, list_create_node( currPCB ), NULL );

}

// sets a node in the pcb
void setPCB( MetaDataStruct *currMeta, ProcessControlBlock *currPCB, int cycle )
{
   ProcessControlBlock *nextPCB = createPCBStruct();
   currPCB->next = nextPCB;
   nextPCB->prev = currPCB;
   nextPCB->current = currMeta;
   nextPCB->processTime = ( cycle * currMeta->cycleTime );
   nextPCB->processNum = currPCB->processNum;
}

// sets a new process node in the pcb linked list, and adds it to the pcb
void setPCBProcess( MetaDataStruct *currMeta, List *pcbList,
                    ProcessControlBlock *currPCB )
{
   ProcessControlBlock *nextPCB = createPCBStruct();
   currPCB->next = nextPCB;
   nextPCB->prev = currPCB;
   nextPCB->current = currMeta;
   nextPCB->processTime = 0;
   nextPCB->processNum = ( currPCB->processNum + 1 );
   list_insert_after( pcbList, list_create_node( nextPCB ), NULL );
}

// sets the last node in the pcb
void setPCBEndcap( MetaDataStruct *currMeta, List *pcbList,
                    ProcessControlBlock *currPCB )
{
   ProcessControlBlock *nextPCB = createPCBStruct();
   currPCB->next = nextPCB;
   nextPCB->prev = currPCB;
   nextPCB->current = currMeta;
   nextPCB->processTime = 0;
   nextPCB->processNum = -1;
   list_insert_after( pcbList, list_create_node( nextPCB ), NULL );
}

// sets the pcb into the ready state
void setPCBReady( ProcessControlBlock *pcb )
{
   while( 1 )
   {
      pcb->state = READY;
      if( pcb->next == NULL )
      {
         break;
      }
      pcb = pcb->next;
   }
   return;
}

// sets any process into a specified state
void setPCBState( ProcessControlBlock *pcb, int state, int nextProcess )
{
   while( 1 )
   {
      if( pcb->state != EXIT )
      {
         pcb->state = state;
      }
      if( pcb->next == NULL ||
          pcb->next->processNum == nextProcess )
      {
         break;
      }
      pcb = pcb->next;
   }
   return;
}

// Mallocs a new pcb structure
ProcessControlBlock *createPCBStruct( void )
{
   ProcessControlBlock *pcb = malloc( sizeof( ProcessControlBlock ) );
   pcb->processTime = 0;
   pcb->processNum = 0;
   pcb->state = NEW;
   return pcb;
}

// Recursively frees the pcb struct
void destroyPCB( ProcessControlBlock *pcb )
{
   if( pcb->next != NULL )
   {
      destroyPCB( pcb->next );
   }
   free( pcb );
   return;
}

IOHelper *createIOHelper( OutputStruct *currOutput, char *timerString,
                          ListNode *currProcess, ProcessControlBlock *currOP,
                          Semaphores *semaphores, List *interruptQueue, Boolean togle )
{
   IOHelper *ioHelper = malloc( sizeof( IOHelper ) );
   ioHelper->currOutput = currOutput;
   ioHelper->timerString = timerString;
   ioHelper->currProcess = currProcess;
   ioHelper->currOP = currOP;
   ioHelper->semaphores = semaphores;
   ioHelper->interruptQueue = interruptQueue;
   ioHelper->togle = togle;
   return ioHelper;

}

void destroyIOHelper( IOHelper *ioHelper )
{
   free( ioHelper );
}

Semaphores *createSemaphores( void )
{
   Semaphores *semaphores = malloc( sizeof( Semaphores ) );
   semaphores->input = 1;
   semaphores->output = 1;
   semaphores->memoryAllocate = 1;
   semaphores->operation = 1;
   semaphores->modifyInterrupt = 1;
   semaphores->outputFile = 1;
   return semaphores;
}

void destroySemaphores( Semaphores *semaphores )
{
   free( semaphores );
}

ListNode *chooseNextProcess( List *pcb, ConfigStruct *conStruct, List* interruptQueue )
{
   char *cpuSchedule = conStruct->cpuScheduleCode;
   if ( stringComp( cpuSchedule, "FCFS-N" ) == True )
   {
      return chooseNextFCFSN( pcb );
   }
   else if( stringComp( cpuSchedule, "SJF-N" ) == True )
   {
      return chooseNextSJFN( pcb );
   }
   else if( stringComp( cpuSchedule, "SRTF-P" ) == True )
   {
      return NULL;//todo sim04
   }
   else if( stringComp( cpuSchedule, "FCFS-P" ) == True )
   {
      return NULL;//todo sim04
   }
   else if( stringComp( cpuSchedule, "RR-P" ) == True )
   {
      return NULL;//todo sim04
   }
   return NULL;//todo sim04
}

ListNode *chooseNextFCFSN( List *pcb )
{
   ListNode *currNode = pcb->first;
   ProcessControlBlock *currPCB = currNode->value;
   while( 1 )
   {
      if( currPCB->state == 1)
      {
         break;
      }


      if( currNode->next == NULL )
      {
         currNode = NULL;
         break;
      }
      currNode = currNode->next;
      currPCB = currNode->value;
   }
   return currNode;
}

ListNode *chooseNextSJFN( List *pcb )
{
   ListNode *shortestNode = NULL;
   int shortestTime = 999999999; //very big number
   ListNode *currNode = pcb->first;
   ProcessControlBlock *currPCB = currNode->value;

   while ( 1 )
   {

      if( ( currPCB->state == 1 ) && ( currPCB->processTime < shortestTime ) )
      {

         shortestNode = currNode;
         shortestTime = currPCB->processTime;
      }

      if( currNode->next == NULL )
      {
         break;
      }

      currNode = currNode->next;
      currPCB = currNode->value;
   }
   return shortestNode;
}
