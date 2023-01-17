#ifndef _DC_PERFORMANCE_
#define _DC_PERFORMANCE_

#include <libapi.h>

void dcPerformance_Init();
void dcPerformance_BeginCounter();
unsigned int dcPerformance_EndCounter();
unsigned int dcPerformance_EndCounterPrintf(const char *label);

#endif