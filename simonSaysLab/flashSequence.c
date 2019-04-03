/*
 * flashSequence.c
 *
 *  Created on: Oct 28, 2014
 *      Author: logan
 */

#include "flashSequence.h" //include .h
#include "globals.h" //include globals to access it
#include "simonDisplay.h" //include the display functions
#include "supportFiles/display.h"
#include "supportFiles/utils.h"
#include <stdio.h>

enum flashSequence_st_t {
    init_st, //waiting for enabling
    flash_st, //flashing the sequence
    flash_st2, //second flashing state
    final_st //finished flashing
} flashState;

#define FLASH_TIME 4 //the amount of time spend one a single flash
#define CLEAR 0 //clear number

uint16_t flashCounter; //counter to use with flash time

bool isEnabledFlagflashSequence; //flag to set state machine to enabled
uint16_t num_flash; //keep track of the number of the flash
uint16_t iteration_times; //keep track of the iteration

void debugStatePrint4() {
  static flashSequence_st_t previousState; //keep track of current state
  static bool firstPass = true; //keep track of first pass through
  // Only print the message if:
  // 1. This the first pass and the value for previousState is unknown.
  // 2. previousState != currentState - this prevents reprinting the same state name over and over.
  if (previousState != flashState || firstPass) {
    firstPass = false;                // previousState will be defined, firstPass is false.
    previousState = flashState;     // keep track of the last state that you were in.
    switch(flashState) {            // This prints messages based upon the state that you were in.
      case init_st:
        printf("init_st\n\r"); //print to show I am in init state
        break;
      case flash_st:
        printf("flash_st\n\r"); //print to show I am in welcome state
        break;
      case flash_st2:
        printf("flash_st2\n\r"); //print to show I am in welcome state
        break;
      case final_st:
        printf("final_st\n\r"); //print to show I am in ad timer state
        break;
     }
  }
}

// Turns on the state machine. Part of the interlock.
void flashSequence_enable(){
    isEnabledFlagflashSequence = true;
}

// Turns off the state machine. Part of the interlock.
void flashSequence_disable(){
    isEnabledFlagflashSequence = false;
}

// Other state machines can call this to determine if this state machine is finished.
bool flashSequence_isComplete(){
    if(flashState == final_st){ //flashing has completed
        return true; //return true
    }
    return false; //otherwise, false
}

// Initialize the state machine
void flashSequence_init(){
    flashState = init_st; //set to init state
    isEnabledFlagflashSequence = false; //set enabled flag to false
}

// Standard tick function.
void flashSequence_tick(){
    switch(flashState){
        case init_st:
            num_flash = CLEAR; //set the current number of flashes to zero
            if(isEnabledFlagflashSequence){ //state machine is enabled
                flashState = flash_st; //go to the flash state
                flashCounter = CLEAR; //set flash counter to zero
            } //otherwise, stay in init state
            break;
        case flash_st:
            //all the actions for the flash state are done here
            if(flashCounter >= FLASH_TIME){ //add counter to make game playable
                flashState = flash_st2; //transition state
            }
            simonDisplay_drawSquare(globals_getSequenceValue(num_flash), false); //draw square
            flashCounter++; //increment counter
            break;
        case flash_st2:
            simonDisplay_drawSquare(globals_getSequenceValue(num_flash), true); //erase square
            num_flash++; //increase the number of flashes
            if(num_flash >= globals_getSequenceIterationLength()){ //if we've flashed through the entire sequence
                flashState = final_st; //go to final state
            }else{ //otherwise
                flashState = flash_st; //go back to flash again
                flashCounter = CLEAR; //clear the counter
            }
            break;
        case final_st:
            if(!isEnabledFlagflashSequence){ //stay here until machine is no longer enabled
                flashState = init_st; //reset the state machine
            }
            break;
    }
    //state actions
    switch(flashState){ //this is primarily blank because the actions are done on the state transitions
        case init_st: //nothing done here
            break;
        case flash_st://nothing done here
            break;
        case flash_st2://nothing done here
            break;
        case final_st://nothing done here
            break;
    }
}

// This will set the sequence to a simple sequential pattern.
// It starts by flashing the first color, and then increments the index and flashes the first
// two colors and so forth. Along the way it prints info messages to the LCD screen.
#define TEST_SEQUENCE_LENGTH 8  // Just use a short test sequence.
uint8_t flashSequence_testSequence[TEST_SEQUENCE_LENGTH] = {
    SIMON_DISPLAY_REGION_0,
    SIMON_DISPLAY_REGION_1,
    SIMON_DISPLAY_REGION_2,
    SIMON_DISPLAY_REGION_3,
    SIMON_DISPLAY_REGION_3,
    SIMON_DISPLAY_REGION_2,
    SIMON_DISPLAY_REGION_1,
    SIMON_DISPLAY_REGION_0};    // Simple sequence.
#define INCREMENTING_SEQUENCE_MESSAGE1 "Incrementing Sequence"  // Info message.
#define RUN_TEST_COMPLETE_MESSAGE "Runtest() Complete"          // Info message.
#define MESSAGE_TEXT_SIZE 2     // Make the text easy to see.
#define TWO_SECONDS_IN_MS 2000  // Two second delay.
#define TICK_PERIOD 75          // 200 millisecond delay.
#define TEXT_ORIGIN_X 0                  // Text starts from far left and
#define TEXT_ORIGIN_Y (display_height()/2) // middle of screen.

// Print the incrementing sequence message.
void flashSequence_printIncrementingMessage() {
  display_fillScreen(DISPLAY_BLACK);  // Otherwise, tell the user that you are incrementing the sequence.
  display_setCursor(TEXT_ORIGIN_X, TEXT_ORIGIN_Y);// Roughly centered.
  display_println(INCREMENTING_SEQUENCE_MESSAGE1);// Print the message.
  utils_msDelay(TWO_SECONDS_IN_MS);   // Hold on for 2 seconds.
  display_fillScreen(DISPLAY_BLACK);  // Clear the screen.
}

// Run the test: flash the sequence, one square at a time
// with helpful information messages.
void flashSequence_runTest() {
  display_init();                   // We are using the display.
  display_fillScreen(DISPLAY_BLACK);    // Clear the display.
  globals_setSequence(flashSequence_testSequence, TEST_SEQUENCE_LENGTH);    // Set the sequence.
  flashSequence_init();               // Initialize the flashSequence state machine
  flashSequence_enable();             // Enable the flashSequence state machine.
  int16_t sequenceLength = 1;         // Start out with a sequence of length 1.
  globals_setSequenceIterationLength(sequenceLength);   // Set the iteration length.
  display_setTextSize(MESSAGE_TEXT_SIZE); // Use a standard text size.
  while (1) {                             // Run forever unless you break.
    flashSequence_tick();             // tick the state machine.
    utils_msDelay(TICK_PERIOD);   // Provide a 1 ms delay.
    if (flashSequence_isComplete()) {   // When you are done flashing the sequence.
      flashSequence_disable();          // Interlock by first disabling the state machine.
      flashSequence_tick();             // tick is necessary to advance the state.
      utils_msDelay(TICK_PERIOD);       // don't really need this here, just for completeness.
      flashSequence_enable();           // Finish the interlock by enabling the state machine.
      utils_msDelay(TICK_PERIOD);       // Wait 1 ms for no good reason.
      sequenceLength++;                 // Increment the length of the sequence.
      if (sequenceLength > TEST_SEQUENCE_LENGTH)  // Stop if you have done the full sequence.
        break;
      // Tell the user that you are going to the next step in the pattern.
      flashSequence_printIncrementingMessage();
      globals_setSequenceIterationLength(sequenceLength);  // Set the length of the pattern.
    }
  }
  // Let the user know that you are finished.
  display_fillScreen(DISPLAY_BLACK);              // Blank the screen.
  display_setCursor(TEXT_ORIGIN_X, TEXT_ORIGIN_Y);// Set the cursor position.
  display_println(RUN_TEST_COMPLETE_MESSAGE);     // Print the message.
}
