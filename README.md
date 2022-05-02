
### NOTE: This repo is deprecated. The code below requires update to the FreeRTOS implementation. The command line interface repo at https://github.com/leroyle/Embedded_Cli contains reworked stack and heap check routines, as well as others, that do not require FreeRTOS changes.

### HeapCheck
A device app that will demonstrate monitoring heap and stack usage.  

 NOTE: for this to work FreeRTOS runtime files must be modified. The files contained within the RuntimeUpdates directory of this project are based on PlatformIO Adafruit implementation of the runtime/freertos support.
 The implementation version can be found in the versioning files within RuntimeUpdates/framework-arduinoadafruitnrf52/

 #### Runtime Modifications
 The freeRTOS task.c/task.h files must be modified to add extra stack details into the task control block (TCB)
 The following must be defined in the freeRTOS config file:
 ```
 framework-arduinoadafruitnrf52/cores/nRF5/freertos/config/FreeRTOSConfig.h
 ```
 ```
 * #define INCLUDE_uxTaskGetStackHighWaterMark  1
 * #define configCHECK_FOR_STACK_OVERFLOW       2
 ```
 configCHECK_FOR_STACK_OVERFLOW supports two different values

 * A value of 1  
 the only check is to ensure the current stack pointer has not been moved beyond the allocated memory space.

 * value of 2  
    * will cause the newly allocated space to be filled with a known value.  
    * During a task swap the first 4 addresses of the stack are compared to the fill value. If different a stack overflow is assumed  
 

 #### HIGH WATER MARK
 The stack used by a task will grow and shrink as the task executes and interrupts are processed. The runtime function uxTaskGetStackHighWaterMark() returns the minimum
 amount of remaining stack space that was available to the task since the task started executing.  
 That is the amount of stack that remained unused when the task stack was at its greatest (deepest) value. This is what is referred to as the stack 'high water mark'. 
 
 #### Support Utilities
 The file utilities.c contain the support functions that can be called when monitoring stack and heap usage.

* getTaskName();  
Returns the current task name
* getTaskDetails();  
Return a filled structure containing task details
* stackSpaceLeft();  
Dump stack statistics to the debug port
* heapSpaceLeft();  
Dump heap statistics to the debug port
* doError();  
Experimental array overwite 
* overflow();  
Overflow the stack
