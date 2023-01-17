#include "dcInput.h"

void dcInput_Init() {
    PadInit(0);
}

void dcInput_InitializePad(SDC_Input *input, int padId) {
    input->padId = padId;
    input->padState = PadRead(input->padId);
}

bool dcInput_BecomesPressed(SDC_Input *input, int key) {
    return ( _PAD(input->padId, key ) & input->padState ) && !(( _PAD(input->padId, key ) & input->prevPadState ));
}

bool dcInput_BecomesReleased(SDC_Input *input, int key) {
    return ( _PAD(input->padId, key ) & input->prevPadState ) && !(( _PAD(input->padId, key ) & input->padState ));
}

bool dcInput_IsPressed(SDC_Input *input, int key) {
    return ( _PAD(input->padId, key ) & input->padState );
}

void dcInput_UpdateInput(SDC_Input *input) {
    input->prevPadState = input->padState;
    input->padState = PadRead(input->padId);
}

