/**
 * A simple app to monitor stack and heap usage. 
 * the loop will recusivly call itself, allocating
 * memory and dumping heap and stack statistics.
 * The stack statistics closly follow but do not exactly
 * match those seen with Seggers Ozone debugger
 * 
 * This does require runtime changes as noted in the top 
 * level README
 * 
 * NOTE: You must start a terminal debug session before loop()
 * will be called.
 */

#include <Arduino.h>
#include <LoRaWan-RAK4630.h> //http://librarymanager/All#SX126x
#include "utils.h"

// For initial stack/heap stats within the loop
bool firstPass = true;		

// malloc size when exhausing heap
uint32_t mallocSize = 5000;	
uint32_t totalMalloced = 0;

// set one of these to true to force exhaustion of stack or heap
bool exhaustStack = false;
bool exhaustHeap = false;

void setup() {
}

uint8_t loopCnt = 0;
void loop() {
  TaskStatus_t xTaskDetails;
  uint32_t appLoopStartStack;
  appLoopStartStack = 1;

  printf("\r\nIn loop: %d\r\n", loopCnt);
  loopCnt++;
  // doError();

  // get task details struct, we extract stack details from this struc
  vTaskGetInfo(NULL,
				 &xTaskDetails,
				 pdTRUE,
				 eInvalid);
  // dump stats for the first call within the loop()
  if(firstPass == true)
  {

    printf(PSTR("(%s): Start Loop First Pass\r\n    Stack Details for task: ======> \r\n"),xTaskDetails.pcTaskName);
    printf(PSTR("\tTask's Allocated stack address = 0x%x\r\n\tUser Main loop first stack addr=0x%x\r\n"), (unsigned int) xTaskDetails.mypxStack, (unsigned int) &appLoopStartStack);  

    printf(PSTR("\r\n    Heap Details =======> \r\n"));
   
    StackType_t startingSbrk = (uint32_t ) reinterpret_cast<char*>(sbrk(0));
    uint32_t *startHeap = (uint32_t *) pvPortMalloc(1);

    printf(PSTR("\tLoop starting sbrk:   0x%x\r\n\tFirst pvPortMalloc in loop: 0x%x\r\n"), (unsigned int) startingSbrk, (unsigned int) startHeap);
    printf(PSTR("End Loop First Pass Details =======> \r\n"));
 
    Serial.flush();
    vPortFree(startHeap);

    firstPass = false;
  }

  stackSpaceLeft();
  heapSpaceLeft();

  exhaustStack = true;
 
  // call loop recursively to exhaust memory
  if(exhaustStack == true)
  {
    delay(300);
    // alloc more heap space
    uint32_t* mPtr = (uint32_t*)  pvPortMalloc(mallocSize);
    totalMalloced += mallocSize;
    printf(PSTR("Main loop current next malloc ptr = 0x%0x\r\n"),(unsigned int) mPtr);
    printf(PSTR("Main loop Total malloc'd memory = %d (0x%0x)\r\n"),(unsigned int) totalMalloced, (unsigned int) totalMalloced);
    yield();

    // this function will trash the stack and cause a
    // hardfault/reset
    // overflow();
    loop();
    
  }

  // alloc some heap space
  uint32_t* mPtr = (uint32_t*)  pvPortMalloc(mallocSize);
  totalMalloced += mallocSize;

  printf(PSTR("Main loop current next malloc ptr = 0x%0x\r\n"),(unsigned int) mPtr);
  printf(PSTR("Main loop Total malloc'd memory = %d (0x%0x)\r\n"),(unsigned int) totalMalloced, (unsigned int) totalMalloced);

  Serial.flush();
  delay(1000);
}
