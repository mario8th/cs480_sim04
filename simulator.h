// Program Information ////////////////////////////////
/***
   * @file simulator.h
   *
   * @brief header for simulator.c
   *
   * @details Declares structs and methods for simulator.c
   *
   * @version 1.0
   *          C.S Student (26 February 2018)
   *          Initial Development and testing of simulator class
   *
   * @Note Requires
   *       helper_lib.h, helper_lib.c, linked_list.h, linked_list.c
   *       Linked List class created for NAU ee222
   *       Boolean enum created by Dr. Micheal Leverington
   *       simtimer created by Dr. Micheal Leverington
*/
#ifndef SIMULATOR
#define SIMULATOR

#include "helper_lib.h"
#include "linked_list.h"
#include "input_data_management.h"
#include "mmu.h"

// pcb for holding every part of every process from the metadata
struct ProcessControlBlock;

typedef struct ProcessControlBlock
{
   struct ProcessControlBlock *next;
   struct ProcessControlBlock *prev;
   MetaDataStruct *current;
   int processTime;
   int processNum;
   int state;
}ProcessControlBlock;

typedef struct Semaphores
{
   // All start 1, 0 when in use
   int input;
   int output;
   int memoryAllocate;
   int operation;
   int modifyInterrupt;
   int outputFile;
}Semaphores;

typedef struct IOHelper
{
   OutputStruct *currOutput;
   char *timerString;
   ListNode *currProcess;
   ProcessControlBlock *currOP;
   Semaphores *semaphores;
   List* interruptQueue;
   Boolean togle;
}IOHelper;

// states for pcb processes
typedef enum { NEW, READY, RUNNING, BLOCKED, EXIT } state;

ReturnMessage startSimulator( ConfigStruct*, List* );

OutputStruct *runSimulator( List*, OutputStruct*, char*, Boolean togle,
                            ConfigStruct*, List* );

OutputStruct *runProcess( ListNode*, ProcessControlBlock*, OutputStruct*, char*,
                          Semaphores*, Boolean, ConfigStruct*, List*, List* );

void *runIO( void* );

void *runO( void* );

void waitProcess( int, char* );

OutputStruct *configureOutput( OutputStruct*, char*, char*, int,
                               char*, char*, int, Boolean );

void writeOutput( OutputStruct*, ConfigStruct* );

ReturnMessage createPCB( ConfigStruct*, List*, List* );

void setPCBFirst( MetaDataStruct*, List*, ProcessControlBlock* );

void setPCB( MetaDataStruct*, ProcessControlBlock*, int );

void setPCBProcess( MetaDataStruct*, List*, ProcessControlBlock* );

void setPCBEndcap( MetaDataStruct*, List*, ProcessControlBlock* );

void setPCBReady( ProcessControlBlock* );

void setPCBState( ProcessControlBlock*, int, int );

ProcessControlBlock *createPCBStruct( void );

void destroyPCB( ProcessControlBlock* );

IOHelper *createIOHelper( OutputStruct*, char*, ListNode*,
                          ProcessControlBlock*, Semaphores*, List*, Boolean );

void destroyIOHelper( IOHelper* );

Semaphores *createSemaphores( void );

void destroySemaphores( Semaphores* );

ListNode *chooseNextProcess( List* , ConfigStruct*, List* );

ListNode *chooseNextFCFSN( List* );

ListNode *chooseNextSJFN( List* );

#endif
