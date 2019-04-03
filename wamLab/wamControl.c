#include "wamControl.h"
#include <stdlib.h>
#include "supportFiles/display.h" //include display
#include <stdio.h>

#define TICK_LOWER 10 //upper bound for tick length
#define TICK_UPPER 21 //upper bound for tick length
#define CLEAR 0 //clear the variable
#define ADC_MAX 1 //max for adc counter
#define HITS_PER_LEVEL 5 //5 hits per level

enum wamControl_st_t {
    init_st, //init state
    wait_for_touch_st, //waiting for a touch
    adc_st, //waiting for adc to settle
    eval_touch_st, //evaluate the touch
    game_over_st, //game over
} wamState;

uint16_t milliPer; //milliseconds per tick variable
uint16_t maxMisses; //maximum number of misses
uint16_t maxActiveMoles; //maximum number of active moles

static wamDisplay_point_t* touchPoint; //the touch point for the mole whack

static uint16_t adcCounter; //set the ADC counter
static bool gameoverFlag; //bool for game over flag

// Call this before using any wamControl_ functions.
void wamControl_init(){
    gameoverFlag = false; //not game over
    adcCounter = CLEAR; //clear
    wamState = init_st; //set the state to init
}

// Call this to set how much time is consumed by each tick of the controlling state machine.
// This information makes it possible to set the awake and sleep time of moles in ms, not ticks.
void wamControl_setMsPerTick(uint16_t msPerTick){
    milliPer = msPerTick; //set the milliseconds per tick
}

// This returns the time consumed by each tick of the controlling state machine.
uint16_t wamControl_getMsPerTick(){
    return milliPer; //return the milliseconds per tick
}

// Use this predicate to see if the game is finished.
bool wamControl_isGameOver(){ //set this to a flag
//    return gameoverFlag; //return the game over validity
    if(wamDisplay_getMissScore() >= maxMisses){ //there are too many misses
        return true;
    }
    return false; //otherwise, the game is not over
}

// Standard tick function.
void wamControl_tick(){
    wamDisplay_updateAllMoleTickCounts(); //update all the mole tick counts each tick regardless of state
    //state transitions
    switch(wamState){
    case init_st: //in init
        wamState = wait_for_touch_st; //go to the state to wait for a touch
        break;
    case wait_for_touch_st: //in wait for touch
        if(display_isTouched()){ //if touched
            //clear the old touch data
            wamState = adc_st; //go to adc settle
            adcCounter = CLEAR; //clear the adc counter
        } //otherwise, wait here
        break;
    case adc_st: //in adc settle state
        if(adcCounter >= ADC_MAX){ //wait for the adc to settle
            wamState = eval_touch_st; //move to eval touch state
        }
        adcCounter++; //increment counter
        break;
    case eval_touch_st: //in eval touch state
        if(wamControl_isGameOver()){ //if the game is over, might have to move this to end of state
            wamState = game_over_st; //move to game over state
        }else{ //otherwise
            wamState = wait_for_touch_st; //wait for another touch
        }
        break;
    case game_over_st: //game over state means we are done
        break;
    }

    //state actions
    switch(wamState){
    case init_st: //in init state, no state action here
        break;
    case wait_for_touch_st: //waiting for touch state
        if(wamDisplay_getActiveMoleCount() < maxActiveMoles + wamDisplay_getLevel()){ //if there are not a maximum number of moles active , the level controls the # active moles
            wamDisplay_activateRandomMole(); //activate a random mole
        }
        break;
    case adc_st: //wait for adc to settle, no state action here
        break;
    case eval_touch_st:
        display_clearOldTouchData(); //clear old touch data
        uint8_t z; //pressure variable
        display_getTouchedPoint(&(touchPoint->x), &(touchPoint->y), &z); //receives touched point
        //need to evaluate the touch here
        wamDisplay_whackMole(touchPoint); //evaluates the point
        break;
    case game_over_st: //game over state, probably unnecessary but no state actions here
        break;
    }
}

// Returns a random value that indicates how long the mole should sleep before awaking.
wamDisplay_moleTickCount_t wamControl_getRandomMoleAsleepInterval(){
     return (rand() % TICK_UPPER) + TICK_LOWER;
}

// Returns a random value that indicates how long the mole should stay awake before going dormant.
wamDisplay_moleTickCount_t wamControl_getRandomMoleAwakeInterval(){
    return (rand() % TICK_UPPER) + TICK_LOWER;
}

// Set the maximum number of active moles.
void wamControl_setMaxActiveMoles(uint16_t count){
    maxActiveMoles = count; //set the max number of moles
}

// Get the current allowable count of active moles.
uint16_t wamControl_getMaxActiveMoles(){
    return maxActiveMoles; //return the maximum number of moles
}

// Set the seed for the random-number generator.
void wamControl_setRandomSeed(uint32_t seed){
    srand(seed); //set the random seed
}

// Set the maximum number of misses until the game is over.
void wamControl_setMaxMissCount(uint16_t missCount){
    maxMisses = missCount; //set the maximum number of misses until the game is over
}

