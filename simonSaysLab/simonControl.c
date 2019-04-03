
#include "simonControl.h" //include .h
#include "globals.h" //include globals
#include "simonDisplay.h" //include display functions
#include "supportFiles/display.h" //include display
#include "supportFiles/utils.h"
#include <stdio.h>
#include <stdlib.h>
#include "verifySequence.h" //include verify functions
#include "flashSequence.h" //include flash functions

#define max(a,b) a > b ? a : b; //defining our max function

#define END_GAME_MAX 20 //max for end game counter
#define VERFIY_MAX 20 //max for verify counter
#define NEW_LEVEL_MAX 30 //max for new level counter
#define CELEBRATION_MAX 20 //max for the celebration counter

#define CLEAR 0 //clear
#define START 1 //beginning iteration
#define START_LEVEL 4 //starting level
#define NUM_ZONES 4 //number of squares
#define SQUARE_WIDTH 20 //size of text

//defines for the SIMON display message
#define CENTER_X display_width()/2 //x center
#define CENTER_Y display_height()/2 //y center
#define LONGEST_SEQUENCE_OFFSET 210 //offsetting the number for the longesst sequence
#define X_OFFSET 100 //offset x value
#define CEL_X_OFFSET 50 //offset  the celebrating message
#define Y_OFFSET 20 //offset the celebration message
#define SIMON_LARGE_TEXT_SIZE 5 //simon text size
#define SIMON_SMALL_TEXT_SIZE 2 //text size for small messages

//defines for the new level display
#define X_DISPLAY_COORDINATE CENTER_X/2 - 50
#define Y_DISPLAY_OFFSET 20 //20 pixel offset

//variables for the state machine
uint8_t seed; //setting the seed number
uint8_t currentIter; //current iteration within the level
uint8_t level; //current level
uint8_t longestSequence; //variable to keep track of the longest sequence

//time out counters
uint8_t endGameCounter; //clocks timeout for endgame state
uint8_t verifyCounter; //clock the timeout for the verification of sequence
uint8_t newLevelCounter; //clocks timeout for new level state
uint8_t celebrationCounter; //clocks the celebration display


//enum for the state declaration
enum simonControl_st_t {
    init_st, //init state
    display_st, //displays game start message
    flashSequence_st, //state for flash sequence state machine
    verifySequence_st, //state for verify sequence state machine
    celebrate_st, //state to display yay
    newLevel_st, //display to ask for new level
    endGame_st //displays longest sequence
} controlState;

void debugStatePrint6() {
  static simonControl_st_t previousState; //keep track of current state
  static bool firstPass = true; //keep track of first pass through
  // Only print the message if:
  // 1. This the first pass and the value for previousState is unknown.
  // 2. previousState != currentState - this prevents reprinting the same state name over and over.
  if (previousState != controlState || firstPass) {
    firstPass = false;                // previousState will be defined, firstPass is false.
    previousState = controlState;     // keep track of the last state that you were in.
    switch(controlState) {            // This prints messages based upon the state that you were in.
      case init_st:
        printf("init_st\n\r"); //print to show I am in init state
        break;
      case display_st:
        printf("display_st\n\r"); //print to show I am in display state
        break;
      case flashSequence_st:
        printf("flashSequence_st\n\r"); //print to show I am in flashsequence state
        break;
      case verifySequence_st:
        printf("verifySequence_st\n\r"); //print to show I am in verifysequence state
        break;
      case celebrate_st:
        printf("celebrate_st\n\r"); //print to show I am in celebrate state
        break;
      case newLevel_st:
        printf("newLevel_st\n\r"); //print to show I am in new level state
        break;
      case endGame_st:
        printf("endGame_st\n\r"); //print to show I am in end game state
        break;
     }
  }
}

//resets the sequence for the level
void simonControl_resetSequence(uint8_t level){
    uint8_t sequence[level]; //create new array
    for(uint8_t i = 0; i < level; i++){ //go through array
        sequence[i] = rand() % NUM_ZONES; //set random value in each index
    }
    currentIter = START; //set current iteration
    globals_setSequence(sequence, level); //set the sequence in the globals
    globals_setSequenceIterationLength(currentIter); //set the iteration length to one because we are restarting a level
}

//display original message
void simonControl_displayStartMessage(bool erase){
    if(erase){ //if erase is true, set text color to black
        display_setTextColor(DISPLAY_BLACK); //set text color
    }else{
        display_setTextColor(DISPLAY_WHITE); //set text color
    }
    display_setCursor(CENTER_X - X_OFFSET, CENTER_Y - Y_OFFSET); //sets the cursor for the larger test
    display_setTextSize(SIMON_LARGE_TEXT_SIZE); //sets  the text size for larger test
    display_println("SIMON"); //display text on LCD
    display_setCursor(CENTER_X - X_OFFSET, CENTER_Y + Y_OFFSET); //sets the cursor for the small test
    display_setTextSize(SIMON_SMALL_TEXT_SIZE); //sets  the text size for small test
    display_println("Touch to start"); //display text on LCD
}

//display longest sequence
void simonControl_displayLongestSequence(bool erase){
    if(erase){ //if erase is true, set text color to black
        display_setTextColor(DISPLAY_BLACK); //set text color
    }else{
        display_setTextColor(DISPLAY_WHITE); //set text color
    }
    display_setCursor(X_DISPLAY_COORDINATE, CENTER_Y - Y_DISPLAY_OFFSET); //set the cursor
    display_setTextSize(SIMON_SMALL_TEXT_SIZE); //sets  the text size for small test
    display_print("Longest Sequence: "); //displaying longest sequence
    display_setCursor(X_DISPLAY_COORDINATE +  LONGEST_SEQUENCE_OFFSET, CENTER_Y - Y_DISPLAY_OFFSET); //set the cursor
    display_print(longestSequence); //print the longest sequence
}

//deletes the longest sequence number
void simonControl_deleteLongestNumber(){
    display_fillRect(X_DISPLAY_COORDINATE +  LONGEST_SEQUENCE_OFFSET, CENTER_Y - Y_DISPLAY_OFFSET, SQUARE_WIDTH, SQUARE_WIDTH, DISPLAY_BLACK); //fills rectangle over number
}

//ask for new level
void simonControl_displayNewLevelMessage(bool erase){
    if(erase){ //if erase is true, set text color to black
        display_setTextColor(DISPLAY_BLACK); //set text color
    }else{
        display_setTextColor(DISPLAY_WHITE); //set text color
    }
    display_setCursor(X_DISPLAY_COORDINATE, CENTER_Y - Y_DISPLAY_OFFSET); //set the cursor
    display_setTextSize(SIMON_SMALL_TEXT_SIZE); //sets  the text size for small test
    display_println("Touch for new level"); //display text on LCD
}

//display celebratory message at the end of a level
void simonControl_displayCelebratoryMessage(bool erase){
    if(erase){ //if erase is true, set text color to black
        display_setTextColor(DISPLAY_BLACK); //set text color
    }else{
        display_setTextColor(DISPLAY_WHITE); //set text color
    }
    display_setCursor(CENTER_X - CEL_X_OFFSET, CENTER_Y - Y_OFFSET); //sets the cursor for the larger test
    display_setTextSize(SIMON_LARGE_TEXT_SIZE); //sets  the text size for larger test
    display_println("Yay!"); //display text on LCD
}

// Initialize the state machine
void simonControl_init(){
    controlState = init_st; //init the state machine
    seed = CLEAR; //reset the seed number for randoms
    display_init(); //init the display
    display_fillScreen(DISPLAY_BLACK); //fill the screen with black
}

// Standard tick function.
void simonControl_tick(){
    //mealy actions first
    switch(controlState){
        case init_st:
            controlState = display_st; //transition to display state
            simonControl_displayStartMessage(false); //display the game start message
            longestSequence = CLEAR; //clear the longest sequence for this game
            break;
        case display_st:
            seed++; //increment the seed value when the state doesn't change
            if(display_isTouched()){ //if the display is touched
                controlState = flashSequence_st; //change state
                level = START_LEVEL; //set starting level
                srand(seed); //set the seed for the randoms values
                simonControl_resetSequence(level); //reset the flashing sequence
                simonControl_displayStartMessage(true); //erase the start message
                flashSequence_enable(); //enable the flash sequence
            }
            break;
        case flashSequence_st:
            if(flashSequence_isComplete()){ //the sequence is done flashing
                controlState = verifySequence_st; //transition to new sequence
                flashSequence_disable(); //disable the flash sequence
                verifySequence_enable(); //enable the verify sequence state machine
            }
            break;
        case verifySequence_st:
            if(verifySequence_isTimeOutError() || verifySequence_isUserInputError()){ //if there is a time out or the player is wrong
                endGameCounter = CLEAR; //reset the end game counter
                controlState = endGame_st; //finishes game
                verifySequence_disable(); //disable the verify sequence state machine
                simonDisplay_eraseAllButtons(); //erase the buttons
                simonControl_displayLongestSequence(false); //show longest sequence message
            }else if(verifySequence_isComplete() && currentIter == level){ //they verified the sequence and finished the level
                controlState = celebrate_st; //change state
                longestSequence = level; //the level is conquered
                simonDisplay_eraseAllButtons(); //erase the buttons
                simonControl_displayCelebratoryMessage(false); //show celebratory message
                celebrationCounter = CLEAR; //reset the celebration counter
                verifySequence_disable(); //disable the verify sequence state machine
            }else if(verifySequence_isComplete()){ //the player correctly verified the sequence
                controlState = flashSequence_st; //change state
                verifySequence_disable(); //disable the verify sequence state machine
                flashSequence_enable(); //reenable the flash sequence state machine
                longestSequence = max(currentIter, longestSequence); //set the longest sequence equal to the maximum of current iter and longest sequence
                currentIter++; //increment the current flash iteration of the level
                globals_setSequenceIterationLength(currentIter); //set the iteration lengths in the globals
                simonDisplay_eraseAllButtons(); //erase the buttons
            }
            break;
        case celebrate_st:
            if(celebrationCounter >= CELEBRATION_MAX){ //once the time is done in celebration state
                simonControl_displayCelebratoryMessage(true); //clear celebratory message
                simonControl_displayNewLevelMessage(false); //show the new level message
                controlState = newLevel_st; //transition to new state
                newLevelCounter = CLEAR; //clear the new level counter
            }
            celebrationCounter++; //increment celebration counter
            break;
        case newLevel_st:
            if(newLevelCounter >= NEW_LEVEL_MAX){ //done timing new level
                controlState = endGame_st; //change state
                simonControl_displayNewLevelMessage(true); //erase new level message
                simonControl_displayLongestSequence(false); //show longest sequence message
                endGameCounter = CLEAR; //reset the end game counter
            }else if(display_isTouched()){ //the player has touched for a new level
                controlState = flashSequence_st; //set the state
                simonControl_displayNewLevelMessage(true); //erase new level message
                level++; //increment the level
                simonControl_resetSequence(level); //reset the sequence with the new level
                flashSequence_enable(); //enable the flash sequence state machine
            }
            newLevelCounter++; //increment the new level counter
            break;
        case endGame_st:
            if(endGameCounter >= END_GAME_MAX){ //done with the time out
                controlState = display_st; //transition state
                longestSequence = CLEAR; //clear the longest sequence for the next game
                simonControl_displayLongestSequence(true); //erase the longest sequence message
                simonControl_deleteLongestNumber(); //gets rid of the number
                simonControl_displayStartMessage(false); //display the simon start message
            }
            endGameCounter++; //increment the end game counter
            break;
    }

    //moore actions/state update
    switch(controlState){
        case init_st: //no moore action here
            break;
        case display_st: //no moore action here
            break;
        case flashSequence_st: //no moore action here
            break;
        case verifySequence_st: //only state with state action
            simonDisplay_drawAllButtons(); //draw all buttons here
            break;
        case celebrate_st: //no moore action here
            break;
        case newLevel_st: //no moore action here
            break;
        case endGame_st: //no moore action here
            break;
        }
}


