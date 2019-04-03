/*
 * clockControl.c
 *
 *  Created on: Apr 10, 2018
 *      Author: logan
 */

//Logan Thorneloe

#include "clockControl.h"
#include "clockDisplay.h"
#include "supportFiles/display.h"
#include <stdio.h>

#define CLEAR 0x0 //CLEAR bits

//the period 40 ms
#define ADC_COUNTER_MAX_VALUE 0x1 //1 periods will give the ADC 40ms
#define RATE_COUNTER_MAX_VALUE 0x2 //lowest possible value to make rate counter fast/ near 100ms
#define AUTO_COUNTER_MAX_VALUE 12 //12 periods will give the auto counter 500ms
#define RUNNING_COUNTER_MAX_VALUE 25 //25 periods for a counter of 1000ms or 1 second

//determine the length of the longest process using interval timer from last lab
//start interval timer at start of tick function and stop it at the end and use that value
//to calc auto_counter, adc_counter, and rate_counter:
//adc needs 40-50ms, so as many of periods as give that
//auto counter needs 500ms, max value is num of time periods that give that value, detects how long you're pushing on the screen
//rate counter needs to be as low as possible, choose 1 to be fast
//running_counter needs to be as close to 1000ms as possible, max*interval_timer calculation = 1000ms


// States for the controller state machine.
enum clockControl_st_t {
    init_st,                 // Start here, transition out of this state on the first tick.
    never_touched_st,        // Wait here until the first touch - clock is disabled until set.
    waiting_for_touch_st,    // waiting for touch, clock is enabled and running.
    ad_timer_running_st,     // waiting for the touch-controller ADC to settle.
    auto_timer_running_st,   // waiting for the auto-update delay to expire
                                 // (user is holding down button for auto-inc/dec)
    rate_timer_running_st,   // waiting for the rate-timer to expire to know when to perform the auto inc/dec.
    rate_timer_expired_st,   // when the rate-timer expires, perform the inc/dec function.
    add_second_to_clock_st   // add a second to the clock time and reset the ms counter.
} currentState;

uint32_t adcCounter, autoCounter, rateCounter, runningCounter;

// This is a debug state print routine. It will print the names of the states each
// time tick() is called. It only prints states if they are different than the
// previous state.
void debugStatePrint() {
  static clockControl_st_t previousState; //keep track of current state
  static bool firstPass = true; //keep track of first pass through
  // Only print the message if:
  // 1. This the first pass and the value for previousState is unknown.
  // 2. previousState != currentState - this prevents reprinting the same state name over and over.
  if (previousState != currentState || firstPass) {
    firstPass = false;                // previousState will be defined, firstPass is false.
    previousState = currentState;     // keep track of the last state that you were in.
    //printf("msCounter:%d\n\r", msCounter);
    switch(currentState) {            // This prints messages based upon the state that you were in.
      case init_st:
        printf("init_st\n\r"); //print to show I am in init state
        break;
      case never_touched_st:
        printf("never_touched_st\n\r"); //print to show I am in never touched state
        break;
      case waiting_for_touch_st:
        printf("waiting_for_touch_st\n\r"); //print to show I am in waiting state
        break;
      case ad_timer_running_st:
        printf("ad_timer_running_st\n\r"); //print to show I am in ad timer state
        break;
      case auto_timer_running_st:
        printf("auto_timer_running_st\n\r"); //print to show I am in auto timer state
        break;
      case rate_timer_running_st:
        printf("rate_timer_running_st\n\r"); //print to show I am in running timer state
        break;
      case rate_timer_expired_st:
        printf("rate_timer_expired_st\n\r"); //print to show I am in rate timer state
        break;
      case add_second_to_clock_st:
        printf("add_second_to_clock_st\n\r"); //print to show I am in adding second state
        break;
     }
  }
}

// Standard tick function.
void clockControl_tick(){
    debugStatePrint(); //show what state we are working in
    // Perform state update first.
      switch(currentState) { //state machine switch statement- dependent upon the state one is in
        case init_st:
            currentState = never_touched_st; //init_st goes straight to never_touched to begin clock cycles
          break;
        case never_touched_st: //waiting for human input
            if(display_isTouched()){ //once touched
                currentState = waiting_for_touch_st; //once touched, starts incrementing immediately
            }else{ //not touched
                currentState = never_touched_st; //stay in never touched state
            }
          break;
        case waiting_for_touch_st: //waiting for incrementing touch
            if(display_isTouched()){ //if touched, start incrementation part of state machine
                 currentState = ad_timer_running_st; //adjust current state to incrementation portion of state machine
                 display_clearOldTouchData(); //MEALY ACTION
            }else{
                 currentState = add_second_to_clock_st; //not touched, keep incrementing the timer by one second
            }
          break;
        case ad_timer_running_st: //checking to make sure the counter is pushed long enough to show the user wants to increment
            if(display_isTouched() && adcCounter == ADC_COUNTER_MAX_VALUE){ //if this display is still pressed after a certain amount of time
                currentState = auto_timer_running_st; //switch to another timer state
            }else if(!display_isTouched() && adcCounter == ADC_COUNTER_MAX_VALUE){ //if the display is not pressed after a certain amount of time
                currentState = waiting_for_touch_st; //user did not mean to increment, return to waiting state
                clockDisplay_performIncDec(); //MEALY ACTION
            }else{
                currentState = ad_timer_running_st; //otherwise, stay in current state and continue to increment counter
            }
          break;
        case auto_timer_running_st: //state to delay incrementing
            if(!display_isTouched()){ //if the display is no longer touched
                currentState = waiting_for_touch_st; //transition to state to wait for input because user no longer wants to increment
                clockDisplay_performIncDec(); //MEALY ACTION
            }else if(autoCounter == AUTO_COUNTER_MAX_VALUE){ //if the user wants to increment
                currentState = rate_timer_running_st; //move to state to auto-increment
                clockDisplay_performIncDec(); //MEALY ACTION
            }else{
                currentState = auto_timer_running_st; //otherwise, stay in state and continue to update auto counter
            }
          break;
        case rate_timer_running_st: //timer to control rate at which the screen updates
            if(!display_isTouched()){ //the screen is no longer touched
                currentState = waiting_for_touch_st; //user returns to wait state because they no longer want to increment
            }else if(rateCounter == RATE_COUNTER_MAX_VALUE){ //if the rate counter is at its max value
                currentState = rate_timer_expired_st; //go to rate counter expired state to increment the screen
            }else{
                currentState = rate_timer_running_st; //otherwise, stay in this state to increment rate counter
            }
          break;
        case rate_timer_expired_st: // state for when the rate timer expires
            if(display_isTouched()){ //if the display is still touched, return back to the counting state for the rate timer
                currentState = rate_timer_running_st; //back to counting state
                clockDisplay_performIncDec(); //MEALY ACTION
            }else{ //display is not touched
               currentState = waiting_for_touch_st; //go back to waiting state to wait for user input
            }
          break;
        case add_second_to_clock_st: //works in conjunction with waiting state to increment the clock while waiting
            if(display_isTouched()){ //if the display is touched
               currentState = ad_timer_running_st; //start the auto-increment process
               display_clearOldTouchData(); //MEALY ACTION
            }else if(runningCounter >= RUNNING_COUNTER_MAX_VALUE){ // if this counter reaches its max value, we need to update the screen because a second has passed
               currentState = waiting_for_touch_st; //return to wait state
               clockDisplay_advanceTimeOneSecond(); //increment the clock as we wait for the auto increment, MEALY action on way to back to waiting for touch state
            }else{
               currentState = add_second_to_clock_st; //otherwise, continue in this state and increment the running counter
            }
          break;
        default:
          printf("clockControl_tick state update: hit default\n\r"); //print to show if the user reaches the default
          break;
      }

      // Perform state action next.
      switch(currentState) { //switch statement to perform state actions
        case init_st: //nothing is done in the init state
          break;
        case never_touched_st:
            //goes from init to here, this goes straight to running the clock on its own
          break;
        case waiting_for_touch_st: //actions for waiting state
            runningCounter = CLEAR; //clear the running counter that increments seconds
            adcCounter = CLEAR; //clear the ad counter
            autoCounter = CLEAR; //clear auto counter
            rateCounter = CLEAR; //clear the rate counter
          break;
        case ad_timer_running_st: //state to increment the ad timer
            adcCounter++; //increment timer
          break;
        case auto_timer_running_st: //state to increment the auto timer
            autoCounter++; //increment time
          break;
        case rate_timer_running_st: //state to increment the rate timer
            rateCounter++; //increment time
          break;
        case rate_timer_expired_st: //state to clear the rate timer
            rateCounter = CLEAR; //clear timer
          break;
        case add_second_to_clock_st: //state to increment the running timer
            runningCounter++; //increment time
          break;
        default:
          printf("clockControl_tick state action: hit default\n\r"); //print to show if the user reaches the default
          break;
      }
}

// Call this before you call clockControl_tick().
void clockControl_init(){
    currentState = init_st; //sets enum value
}
