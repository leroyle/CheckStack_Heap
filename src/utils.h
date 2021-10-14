#ifndef _UTILS_H_
#define _UTILS_H_
#include <LoRaWan-RAK4630.h>

#ifdef ENABLE_MEM_CHECK

// We need to delcare sbrk
#ifdef __arm__
// should use uinstd.h to define sbrk but Due causes a conflict
extern "C" char* sbrk(int incr);
#else  // __ARM__
extern char *__brkval;
#endif  // __arm__

const char * getTaskName();
TaskStatus_t * getTaskDetails(TaskStatus_t *taskDetails);
uint32_t stackSpaceLeft();
uint32_t heapSpaceLeft();
void doError();
void overflow();

#endif
#endif