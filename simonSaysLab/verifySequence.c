/* verifySequence.h
 *
 *  Created on: Oct 28, 2014
 *      Author: logan
 */

#include "verifySequence.h"
#include "globals.h"
#include "simonDisplay.h"
#include "supportFiles/display.h"
#include "supportFiles/utils.h"
#include "../switchAndButtonLab/buttons.h"
#include "buttonHandler.h"
#include <stdio.h>

enum verifySequence_st_t {
    init_st, //waiting for enabling
    reset_st,
    waiting_for_release_st, //waiting for a release
    check_release_st, //check the release
    final_st //finished flashing
} verifyState;

#define TIME_OUT_MAX 16 //set the maximum
#define CLEAR 0 //define to clear

void debugStatePrint5() {
  static verifySequence_st_t previousState; //keep track of current state
  static bool firstPass = true; //keep track of first pass through
  // Only print the message if:
  // 1. This the first pass and the value for previousState is unknown.
  // 2. previousState != currentState - this prevents reprinting the same state name over and over.
  if (previousState != verifyState || firstPass) {
    firstPass = false;                // previousState will be defined, firstPass is false.
    previousState = verifyState;     // keep track of the last state that you were in.
    switch(verifyState) {            // This prints messages based upon the state that you were in.
      case init_st:
        printf("init_st\n\r"); //print to show I am in init state
        break;
      case reset_st:
        printf("reset_st\n\r"); //print to show I am in reset state
        break;
      case waiting_for_release_st:
        printf("waiting_for_release_st\n\r"); //print to show I am in release state
        break;
      case check_release_st:
        printf("check_release_st\n\r"); //print to show I am in check release state
        break;
      case final_st:
        printf("final_st\n\r"); //print to show I am in final state
        break;
     }
  }
}


bool isEnabledFlagverifySequence; //flag to turn state machine on
bool timeOutError; //variable to check time out
bool userInputError; //variable to check user input
uint16_t index, checkForTimeOut; //variables for index and to check for time out

// State machine will run when enabled.
void verifySequence_enable(){
    isEnabledFlagverifySequence = true;
}

// This is part of the interlock. You disable the state-machine and then enable it again.
void verifySequence_disable(){
    isEnabledFlagverifySequence = false;
}

// Used to detect if there has been a time-out error.
bool verifySequence_isTimeOutError(){
    return timeOutError;
}

// Used to detect if the user tapped the incorrect sequence.
bool verifySequence_isUserInputError(){
    return userInputError;
}

// Used to detect if the verifySequence state machine has finished verifying.
bool verifySequence_isComplete(){
    if(verifyState == final_st){ //if done
        return true; //return true
    } //otherwise
    return false; //return false
}

// Initialize the state machine
void verifySequence_init(){
    verifyState = init_st; //init the original state
}

// Standard tick function.
void verifySequence_tick(){
    switch(verifyState){
        case init_st:
            timeOutError = false; //set the errors to assume false
            userInputError = false;
            index = CLEAR; //reset the index we're checking
            if(isEnabledFlagverifySequence){ //if enabled
                buttonHandler_init(); //init the button handler
                verifyState = reset_st; //state transition
            }
            break;
        case reset_st:
            //disable and reenable the button handler to detect the next input
            buttonHandler_enable(); //enable the button handler state machine
            checkForTimeOut = CLEAR; //reset time out timer
            verifyState = waiting_for_release_st; //state transition
            break;
        case waiting_for_release_st:
            if(checkForTimeOut >= TIME_OUT_MAX){ //if time out
                timeOutError = true; //set time out to true
                verifyState = final_st; //go to final state
                simonDisplay_drawSquare(buttonHandler_getRegionNumber(), true); //erase the square I just pressed
            }
            if(buttonHandler_releaseDetected()){ //release detected, move to next state
                verifyState = check_release_st; //state transition
            }
            checkForTimeOut++; //increment counter
            break;
        case check_release_st:
            if(buttonHandler_getRegionNumber() == globals_getSequenceValue(index)){ //this means its the correct press
                verifyState = reset_st; //check next input
                buttonHandler_disable(); //disable state machine
            }else{
                userInputError = true; //wrong user input so they are wrong
                verifyState = final_st; //transition to final state
            }
            index++; //increment index to verify with
            if(index >= globals_getSequenceIterationLength()){ //we're done going through the sequence
                verifyState = final_st; //transition state
            }
            break;
        case final_st:
            buttonHandler_disable(); //we need to disable the button handler
            if(!isEnabledFlagverifySequence){ //stay here until this state machine is disabled, then go to init
                verifyState = init_st; //go back to the init state
            }
            break;
    }
    //state actions
    switch(verifyState){ //this is primarily blank because the actions are done on the state transitions
        case init_st: //nothing done here
            break;
        case reset_st://nothing done here
            break;
        case waiting_for_release_st://nothing done here
            break;
        case check_release_st://nothing done here
            break;
        case final_st://nothing done here
            break;
    }
}

#define MESSAGE_X 0
//#define MESSAGE_Y (display_width()/4)
#define MESSAGE_Y (display_height()/2)
#define MESSAGE_TEXT_SIZE 2
//#define MESSAGE_STARTING_OVER
#define BUTTON_0 0  // Index for button 0
#define BUTTON_1 1  // Index for button 1
#define BUTTON_2 2  // Index for button 2
#define BUTTON_3 3  // Index for button 3
// Prints the instructions that the user should follow when
// testing the verifySequence state machine.
// Takes an argument that specifies the length of the sequence so that
// the instructions are tailored for the length of the sequence.
// This assumes a simple incrementing pattern so that it is simple to
// instruct the user.
void verifySequence_printInstructions(uint8_t length, bool startingOver) {
    display_fillScreen(DISPLAY_BLACK);              // Clear the screen.
    display_setTextSize(MESSAGE_TEXT_SIZE);     // Make it readable.
    display_setCursor(MESSAGE_X, MESSAGE_Y);    // Rough center.
    if (startingOver) {                                             // Print a message if you start over.
        display_fillScreen(DISPLAY_BLACK);          // Clear the screen if starting over.
        display_setTextColor(DISPLAY_WHITE);        // Print whit text.
        display_println("Starting Over. ");         // Starting over message.
    }
    // Print messages are self-explanatory, no comments needed.
    // These messages request that the user touch the buttons in a specific sequence.
    display_println("Tap: ");
    display_println();
    switch (length) {
    case 1:
        display_println("red");
        break;
    case 2:
        display_println("red, yellow ");
        break;
    case 3:
        display_println("red, yellow, blue ");
        break;
    case 4:
        display_println("red, yellow, blue, green ");
        break;
    default:
        break;
    }
    display_println("in that order.");
    display_println();
    display_println("hold BTN0 to quit.");
}

// Just clears the screen and draws the four buttons used in Simon.
void verifySequence_drawButtons() {
    display_fillScreen(DISPLAY_BLACK);  // Clear the screen.
    simonDisplay_drawAllButtons();      // Draw all the buttons.
}

// This will set the sequence to a simple sequential pattern.
#define MAX_TEST_SEQUENCE_LENGTH 4  // the maximum length of the pattern
uint8_t verifySequence_testSequence[MAX_TEST_SEQUENCE_LENGTH] = {0, 1, 2, 3};  // A simple pattern.
#define MESSAGE_WAIT_MS 4000  // Display messages for this long.

// Increment the sequence length making sure to skip over 0.
// Used to change the sequence length during the test.
int16_t incrementSequenceLength(int16_t sequenceLength) {
    int16_t value = (sequenceLength + 1) % (MAX_TEST_SEQUENCE_LENGTH+1);
    if (value == 0) value++;
    return value;
}

// Used to select from a variety of informational messages.
enum verifySequence_infoMessage_t {
    user_time_out_e,            // means that the user waited too long to tap a color.
    user_wrong_sequence_e,      // means that the user tapped the wrong color.
    user_correct_sequence_e,    // means that the user tapped the correct sequence.
    user_quit_e                 // means that the user wants to quite.
};

// Prints out informational messages based upon a message type (see above).
void verifySequence_printInfoMessage(verifySequence_infoMessage_t messageType) {
    // Setup text color, position and clear the screen.
    display_setTextColor(DISPLAY_WHITE);
    display_setCursor(MESSAGE_X, MESSAGE_Y);
    display_fillScreen(DISPLAY_BLACK);
  switch(messageType) {
  case user_time_out_e:  // Tell the user that they typed too slowly.
        display_println("Error:");
        display_println();
        display_println("  User tapped sequence");
        display_println("  too slowly.");
    break;
  case user_wrong_sequence_e:  // Tell the user that they tapped the wrong color.
        display_println("Error: ");
        display_println();
        display_println("  User tapped the");
        display_println("  wrong sequence.");
    break;
  case user_correct_sequence_e:  // Tell the user that they were correct.
        display_println("User tapped");
        display_println("the correct sequence.");
    break;
  case user_quit_e:             // Acknowledge that you are quitting the test.
    display_println("quitting runTest().");
    break;
  default:
    break;
  }
}

#define TICK_PERIOD_IN_MS 100
// Tests the verifySequence state machine.
// It prints instructions to the touch-screen. The user responds by tapping the
// correct colors to match the sequence.
// Users can test the error conditions by waiting too long to tap a color or
// by tapping an incorrect color.
void verifySequence_runTest() {
    display_init();  // Always must do this.
    buttons_init();  // Need to use the push-button package so user can quit.
    int16_t sequenceLength = 1;  // Start out with a sequence length of 1.
    verifySequence_printInstructions(sequenceLength, false);  // Tell the user what to do.
    utils_msDelay(MESSAGE_WAIT_MS);  // Give them a few seconds to read the instructions.
    verifySequence_drawButtons();    // Now, draw the buttons.
    // Set the test sequence and it's length.
    globals_setSequence(verifySequence_testSequence, MAX_TEST_SEQUENCE_LENGTH);
    globals_setSequenceIterationLength(sequenceLength);
    verifySequence_init();    // Initialize the verifySequence state machine
    // Enable the verifySequence state machine.
    verifySequence_enable();  // Everything is interlocked, so first enable the machine.
    // Need to hold button until it quits as you might be stuck in a delay.
    while (!(buttons_read() & BUTTONS_BTN0_MASK)) {
        // verifySequence uses the buttonHandler state machine so you need to "tick" both of them.
        verifySequence_tick();  // Advance the verifySequence state machine.
        buttonHandler_tick();   // Advance the buttonHandler state machine.
        utils_msDelay(TICK_PERIOD_IN_MS);       // Wait for a tick period.
        // If the verifySequence state machine has finished, check the result,
        // otherwise just keep ticking both machines.
        if (verifySequence_isComplete()) {
            if (verifySequence_isTimeOutError()) {                // Was the user too slow?
                verifySequence_printInfoMessage(user_time_out_e); // Yes, tell the user that they were too slow.
            } else if (verifySequence_isUserInputError()) {       // Did the user tap the wrong color?
                verifySequence_printInfoMessage(user_wrong_sequence_e); // Yes, tell them so.
            } else {
                verifySequence_printInfoMessage(user_correct_sequence_e); // User was correct if you get here.
            }
            utils_msDelay(MESSAGE_WAIT_MS);                            // Allow the user to read the message.
            sequenceLength = incrementSequenceLength(sequenceLength);  // Increment the sequence.
            globals_setSequenceIterationLength(sequenceLength);        // Set the length for the verifySequence state machine.
            verifySequence_printInstructions(sequenceLength, true);    // Print the instructions.
            utils_msDelay(MESSAGE_WAIT_MS);                            // Let the user read the instructions.
            verifySequence_drawButtons();                              // Draw the buttons.
            verifySequence_disable();                                  // Interlock: first step of handshake.
            verifySequence_tick();                                     // Advance the verifySequence machine.
            utils_msDelay(TICK_PERIOD_IN_MS);                          // Wait for tick period.
            verifySequence_enable();                                   // Interlock: second step of handshake.
            utils_msDelay(TICK_PERIOD_IN_MS);                          // Wait for tick period.
        }
    }
    verifySequence_printInfoMessage(user_quit_e);  // Quitting, print out an informational message.
}
