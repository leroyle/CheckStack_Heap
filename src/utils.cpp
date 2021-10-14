#include "utils.h"

#ifdef ENABLE_MEM_CHECK

/**
 * NOTE: for this to work the freeRTOS task.c/task.h files must be modified to
 * add extra stack details into the task control block (TCB)
 * 
 * The following must be defined in the freeRTOS config file:
 * framework-arduinoadafruitnrf52/cores/nRF5/freertos/config/FreeRTOSConfig.h
 * 
 * #define INCLUDE_uxTaskGetStackHighWaterMark  1
 * #define configCHECK_FOR_STACK_OVERFLOW       2
 *   - value of 1 the only check is to ensure the current stack pointer has 
 *     not been moved beyond the allocated memory space.
 *   - value of 2 will cause the newly allocated space to be filled with
 *     a known value. 
 *  During a task swap the first 4 addresses of the stack are compared
 *  to the fill value. If different a stack overflow is assumed
 * 
 * HIGH WATER MARK
 * The stack used by a task will grow and shrink as the task executes and
 * interrupts are processed. uxTaskGetStackHighWaterMark() returns the minimum
 * amount of remaining stack space that was available to the task since the 
 * task started executing - that is the amount of stack that remained unused 
 * when the task stack was at its greatest (deepest) value. This is what is
 * referred to as the stack 'high water mark'. 
 * 
 */


/**
 * Print stack details
 * 
 * The FreeRTOS runtime task.c and task.h has been modified
 * to include the structure members used within this function
 * The runtime files modified should accompany this project
 * This borrows from the Segger Ozone FreeRTOS plugin code used
 * to dump stack statistics.
 */
uint32_t stackSpaceLeft()
{

  TaskStatus_t xTaskDetails;
 
  getTaskDetails(&xTaskDetails);
  uint32_t highMark = xTaskDetails.usStackHighWaterMark;

  printf(PSTR("\r\n(%s): Stack Space Check() =======> \r\n"), xTaskDetails.pcTaskName);
   
  uint32_t freeSpace = (uint32_t) xTaskDetails.mypxTopOfStack - (uint32_t) xTaskDetails.mypxStack;
  uint32_t usedSpace = (uint32_t)xTaskDetails.mypxEndOfStack - (uint32_t)xTaskDetails.mypxTopOfStack;
  
  printf(PSTR("\tAllocated stack ptr:%p \r\n\tStack free space 0x%ld \r\n\tStack used space 0x%ld \r\n\tTotal Stack size 0x%ld \r\n\tStack high water mark 0x%ld\r\n"),
  xTaskDetails.myAllocStackPtr, freeSpace, usedSpace, freeSpace + usedSpace, highMark);

  printf(PSTR("Stack Space Check() =======> \r\n\r\n"));
  Serial.flush();

  return highMark;
}

/**
 * Print heap details
 * 
 */

uint32_t heapSpaceLeft()
{

  TaskStatus_t xTaskDetails;
  getTaskDetails(&xTaskDetails);

  // values from app linker .map file
  // Name   Origin             Length             Attributes
  // FLASH  0x0000000000026000 0x00000000000c7000 xr
  // RAM    0x0000000020006000 0x000000000003a000 xrw
  // StackStart == ram origin + Length
  // 0x20040000    = 0x20006000 + 0x3a000

  uint32_t stackStart = 0x20040000;     // value from app linker map file
  uint32_t ramEnd = stackStart + 1;
  uint32_t sbrkVal = (uint32_t ) reinterpret_cast<char*>(sbrk(0));
  uint32_t ramLeft = ramEnd - sbrkVal;
  printf(PSTR("\r\n(%s): Heap Space Check() =======> \r\n"), xTaskDetails.pcTaskName);
  printf(PSTR("\tCurrent sbrk() value: 0x%x\r\n\tAvailable Memory:     0x%x\r\n"), (unsigned int) sbrkVal, (unsigned int) ramLeft);
  printf(PSTR("Heap Space Check() =======> \r\n\r\n"));
  return ramLeft;
}

/**
 * get current task details structure
 */
TaskStatus_t * getTaskDetails(TaskStatus_t *taskDetails)
{
   vTaskGetInfo(NULL,
        taskDetails,
        pdTRUE,
        eInvalid);

   return taskDetails;
}

/** 
 * return task name
 */
const char * getTaskName()
{
  TaskStatus_t xTaskDetails;
  getTaskDetails(&xTaskDetails);
	
	return xTaskDetails.pcTaskName;
}

// this will cause an exception as we are overwriting
// the link return address on the stack
// With PlatformIO debugger the device will restart.
// with Segger Ozone, the HardFault exception will be caught
// and displayed. Plus you will see "return address out of 
// range" in the call stack view 
void overflow()
{
  char buff[2];
  sprintf(buff,"%s\r\n","TEST of a stack over flowTEST of a stack over flowTEST of a stack over flow");
  buff[1]= '\0';
  printf(buff);
}

// cannot seem to get this to work yet. Might need to
// customize the version in the FreeRTOS runtime
void vApplicationStackOverflowHook(TaskHandle_t xTask, signed char *pcTaskName)
{
  printf("Stack overflow: %s\r\n", pcTaskName);
}


// overwrite array
void doError()
{
 const char *arr[] = { "0", "1"};
 uint8_t foo = 2;
 
 printf("foo = %s\r\n",  arr[foo]);
}

#endif