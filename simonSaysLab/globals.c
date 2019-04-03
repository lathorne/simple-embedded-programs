#include "globals.h" //include .h
#include <stdio.h>
#include <string.h>

// The length of the sequence.
// The static keyword means that globals_sequenceLength can only be accessed
// by functions contained in this file.
static uint16_t globals_sequenceLength;  // The length of the sequence.
static uint16_t globals_sequenceIterationLength; //the current length of sequence iterated to.
static uint8_t *globals_fullSequence; //stores the full sequence

// This is the length of the complete sequence at maximum length.
// You must copy the contents of the sequence[] array into the global variable that you maintain.
// Do not just grab the pointer as this will fail.
void globals_setSequence(const uint8_t sequence[], uint16_t length){
    memcpy(globals_fullSequence, sequence, length); //copy the sequence
    globals_sequenceLength = length; //copy the length to a global variable
}

// This returns the value of the sequence at the index.
uint8_t globals_getSequenceValue(uint16_t index){
    return *(globals_fullSequence + index); //returns value at index
}

// Retrieve the sequence length.
uint16_t globals_getSequenceLength(){
    return globals_sequenceLength;
}

// This is the length of the sequence that you are currently working on,
// not the maximum length but the interim length as
// the user works through the pattern one color at a time.
void globals_setSequenceIterationLength(uint16_t length) {
    globals_sequenceIterationLength = length;
}

// This is the length of the sequence that you are currently working on,
// not the maximum length but the interim length as
// the person playing the game works through the pattern one color at a time.
uint16_t globals_getSequenceIterationLength(){
    return globals_sequenceIterationLength;
}

// You will need to implement the other functions.
