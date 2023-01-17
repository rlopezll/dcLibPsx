#include "dcPerformance.h"
#include <stdio.h>

#define RCntIntr 0x1000

int gCounterBeforeUpdate = 0;

void dcPerformance_Init() {
    int CounterMaxValue = 240; //This means this counter resets every 240 HBlanks/
    SetRCnt(RCntCNT1, CounterMaxValue, RCntIntr);
    StartRCnt(RCntCNT1);
}

void dcPerformance_BeginCounter() {
    gCounterBeforeUpdate = GetRCnt(RCntCNT1);
}

unsigned int dcPerformance_EndCounter() {
    int counterAfterUpdate = GetRCnt(RCntCNT1);
    return counterAfterUpdate - gCounterBeforeUpdate;
}

unsigned int dcPerformance_EndCounterPrintf(const char *label) {
    int counter = dcPerformance_EndCounter();
    printf("Counter '%s': %d\n", label, counter);
    return counter;
}
