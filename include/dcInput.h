#ifndef _DC_INPUT_
#define _DC_INPUT_

#include <stdlib.h>
#include <stdbool.h>
#include <types.h>
#include <libetc.h>

typedef struct {
  int padId;
  u_long padState;
  u_long prevPadState;
} SDC_Input;

void dcInput_Init();
void dcInput_InitializePad(SDC_Input *input, int padId);
bool dcInput_BecomesPressed(SDC_Input *input, int key);
bool dcInput_BecomesReleased(SDC_Input *input, int key);
bool dcInput_IsPressed(SDC_Input *input, int key);
void dcInput_UpdateInput(SDC_Input *input);

#endif