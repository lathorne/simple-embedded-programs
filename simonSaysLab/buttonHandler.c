
#include "buttonHandler.h" //include .h
#include "supportFiles/display.h" //include display functions
#include "simonDisplay.h" //include display functions
#include "supportFiles/utils.h"
#include <stdio.h>

//state machine flags
bool fingerRemovalFlag;
bool isEnabledFlagButtonHandler;

uint8_t currentSquare; //variable to keep track of current square

// States for the controller state machine.
enum simonControl_st_t {
    disable_st, //waiting for enabling/init state
    wait_for_touch_st, //waiting for user input
    adc_settle_st, //adc settle
    wait_for_release_st, //waiting for user to release their hand
    final_st //end state
} simonState;

void debugStatePrint3() {
  static simonControl_st_t previousState; //keep track of current state
  static bool firstPass = true; //keep track of first pass through
  // Only print the message if:
  // 1. This the first pass and the value for previousState is unknown.
  // 2. previousState != currentState - this prevents reprinting the same state name over and over.
  if (previousState != simonState || firstPass) {
    firstPass = false;                // previousState will be defined, firstPass is false.
    previousState = simonState;     // keep track of the last state that you were in.
    switch(simonState) {            // This prints messages based upon the state that you were in.
      case disable_st:
        printf("disable_st\n\r"); //print to show I am in init state
        break;
      case wait_for_touch_st:
        printf("wait_for_touch_st\n\r"); //print to show I am in welcome state
        break;
      case wait_for_release_st:
        printf("wait_for_release_st\n\r"); //print to show I am in ad timer state
      case adc_settle_st:
        printf("adc_settle_st\n\r"); //print to show I am in ad timer state
      case final_st:
        printf("final_st\n\r"); //print to show I am in ad timer state
        break;
     }
  }
}

// Get the simon region numbers. See the source code for the region numbering scheme.
uint8_t buttonHandler_getRegionNumber(){
    int16_t x, y; //coordinates for touched point
    uint8_t z; //pressure variable
    display_getTouchedPoint(&x, &y, &z); //receives touched point
    uint8_t value = simonDisplay_computeRegionNumber(x, y); //use the display to return the region number
    return value; //returns value
}

// Turn on the state machine. Part of the interlock.
void buttonHandler_enable(){
    isEnabledFlagButtonHandler = true; //enable the state machine
}

// Turn off the state machine. Part of the interlock.
void buttonHandler_disable(){
    isEnabledFlagButtonHandler = false; //disable the state machine
}

// The only thing this function does is return a boolean flag set by the buttonHandler state machine. To wit:
// Once enabled, the buttonHandler state-machine first waits for a touch. Once a touch is detected, the
// buttonHandler state-machine computes the region-number for the touched area. Next, the buttonHandler
// state-machine waits until the player removes their finger. At this point, the state-machine should
// set a bool flag that indicates the the player has removed their finger. Once the buttonHandler()
// state-machine is disabled, it should clear this flag.
// All buttonHandler_releasedDetected() does is return the value of this flag.
// As such, the body of this function should only contain a single line of code.
// If this function does more than return a boolean set by the buttonHandler state machine, you are going about
// this incorrectly.
bool buttonHandler_releaseDetected(){
    return fingerRemovalFlag;
}

// Initialize the state machine
void buttonHandler_init(){
    simonState = disable_st; //init to disable st
}

// Standard tick function.
void buttonHandler_tick(){
    //perform the state update here
    switch(simonState){
        case disable_st:
            if(isEnabledFlagButtonHandler){ //transition to enable the state machine
                simonState = wait_for_touch_st;
            } //otherwise, stay in disabled state
            break;
        case wait_for_touch_st:
            if(!isEnabledFlagButtonHandler){ //if the state machine is no longer enabled
                simonState = disable_st; //disable the machine if it is no longer enabled
            }else if(display_isTouched()){ //if the screen is touched, transition to new state
                display_clearOldTouchData(); //clear the old touch data
                simonState = adc_settle_st; //transition state
            }
            break;
        case adc_settle_st: //waiting for adc to settle
            if(!isEnabledFlagButtonHandler){ //if no longer enabled
                 simonState = disable_st; //disable the state machine if it is no longer enabled
            }else if(display_isTouched()){ //is display is touched
                simonState = wait_for_release_st; //transition state
                currentSquare = buttonHandler_getRegionNumber(); //computer the current square being touched
                simonDisplay_drawSquare(currentSquare, false); //draws the square
            }else if(!display_isTouched()){ //if display was not touched, false alarm
                simonState = wait_for_touch_st; //transition state
            }
            break;
        case wait_for_release_st:
            if(!isEnabledFlagButtonHandler){ //if no longer enabled
                simonState = disable_st; //disable the state machine if it is no longer enabled
            } else if(!display_isTouched()){ //the display is no longer touched
                fingerRemovalFlag = true; //set the finger removal flag to true
                simonDisplay_drawSquare(currentSquare, true); //erases the square
                simonDisplay_drawButton(currentSquare, false); //draws the button
                simonState = final_st; //transition to final state
            }
            break;
        case final_st:
            if(!isEnabledFlagButtonHandler){ //stay here until the state machine is disabled
                simonState = disable_st; //disable the state machine if it is no longer enabled
            }
            break;
    }

    //perform the state action here
     switch(simonState){
        case disable_st:
            fingerRemovalFlag = false; //clear this flag
            break;
        case wait_for_touch_st: //no moore action here
            break;
        case adc_settle_st: //no moore action here
            break;
        case wait_for_release_st: //no moore action here
            break;
        case final_st: //no moore action
            break;
        }

}

#define RUN_TEST_TERMINATION_MESSAGE1 "buttonHandler_runTest()"  // Info message.
#define RUN_TEST_TERMINATION_MESSAGE2 "terminated."              // Info message.
#define RUN_TEST_TEXT_SIZE 2            // Make text easy to see.
#define RUN_TEST_TICK_PERIOD_IN_MS 100  // Assume a 100 ms tick period.
#define TEXT_MESSAGE_ORIGIN_X 0                  // Text is written starting at the right, and
#define TEXT_MESSAGE_ORIGIN_Y (display_height()/2) // middle.

// buttonHandler_runTest(int16_t touchCount) runs the test until
// the user has touched the screen touchCount times. It indicates
// that a button was pushed by drawing a large square while
// the button is pressed and then erasing the large square and
// redrawing the button when the user releases their touch.

void buttonHandler_runTest(int16_t touchCountArg) {
    int16_t touchCount = 0;                 // Keep track of the number of touches.
    display_init();                         // Always have to init the display.
    display_fillScreen(DISPLAY_BLACK);      // Clear the display.
    // Draw all the buttons for the first time so the buttonHandler doesn't need to do this in an init state.
    // Ultimately, simonControl will do this when the game first starts up.
    simonDisplay_drawAllButtons();
    buttonHandler_init();                   // Initialize the button handler state machine
    buttonHandler_enable();
    while (touchCount < touchCountArg) {    // Loop here while touchCount is less than the touchCountArg
        buttonHandler_tick();               // Advance the state machine.
        utils_msDelay(RUN_TEST_TICK_PERIOD_IN_MS);
        if (buttonHandler_releaseDetected()) {  // If a release is detected, then the screen was touched.
            touchCount++;                       // Keep track of the number of touches.
            // Get the region number that was touched.
            printf("button released: %d\n\r", buttonHandler_getRegionNumber());
            // Interlocked behavior: handshake with the button handler (now disabled).
            buttonHandler_disable();
            utils_msDelay(RUN_TEST_TICK_PERIOD_IN_MS);
            buttonHandler_tick();               // Advance the state machine.
            buttonHandler_enable();             // Interlocked behavior: enable the buttonHandler.
            utils_msDelay(RUN_TEST_TICK_PERIOD_IN_MS);
            buttonHandler_tick();               // Advance the state machine.
        }
    }
    display_fillScreen(DISPLAY_BLACK);        // clear the screen.
    display_setTextSize(RUN_TEST_TEXT_SIZE);  // Set the text size.
    display_setCursor(TEXT_MESSAGE_ORIGIN_X, TEXT_MESSAGE_ORIGIN_Y); // Move the cursor to a rough center point.
    display_println(RUN_TEST_TERMINATION_MESSAGE1); // Print the termination message on two lines.
    display_println(RUN_TEST_TERMINATION_MESSAGE2);
}

