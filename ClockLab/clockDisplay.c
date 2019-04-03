/*
 * clockDisplay.c
 *
 *  Created on: Apr 10, 2018
 *      Author: logan
 */

//Logan Thorneloe

#include "clockDisplay.h"
#include "supportFiles/display.h"
#include "supportFiles/utils.h"
#include <stdio.h>
#include "../intervalTimerLab/intervalTimer.h"

#define DISPLAY_CENTER_Y display_height()/2 //center y value
#define DISPLAY_CENTER_X display_width()/2 //center x value
#define CLEAR 0x0
#define TEST_TOUCHES 150 //number of touches to test display

//defining text size
#define TEXT_SIZE 6

//split screen into grid of six squares
#define LEFT_BUTTONS display_width()/3 //right side of left buttons
#define MIDDLE_BUTTONS LEFT_BUTTONS*2 //right side of middle buttons
#define SEPARATE_VERTICAL_BUTTONS DISPLAY_CENTER_Y //the vertical center lines separating upper and lower buttons

//character width
#define CHARACTER_WIDTH TEXT_SIZE*6
//character height
#define CHARACTER_HEIGHT TEXT_SIZE*8
//controls for drawing triangles
#define TWO_STEPS_AWAY 0x2 //two characters away
#define THREE_STEPS_AWAY 0x3 //three characters away
#define FOUR_STEPS_AWAY 0x4 //four characters away
#define CHARACTER_PADDING 0x2 //padding between characters and arrows

//force the display to update all display members
#define UPDATE_ALL true //tells the screen to be updated entirely
#define NOT_UPDATE_ALL false //only update a portion of the screen

//defines for the rollover parameters of the clock
#define SECONDS_MINUTES_ROLLOVER 59 //highest seconds value
#define SECONDS_MINUTES_ROLLUNDER 0 //lowest seconds value
#define HOURS_ROLLOVER 12 //highest hours value
#define HOURS_ROLLUNDER 1 //lowest hours value
#define SINGLE_INCDEC 1 //single incremental/decremental value

//digit x-position for set cursor on the lcd
#define NUMBERS_Y DISPLAY_CENTER_Y-(CHARACTER_HEIGHT/CHARACTER_PADDING) //x position for all numbers
#define HOURS_1 DISPLAY_CENTER_X-(CHARACTER_WIDTH*FOUR_STEPS_AWAY) //x position for first hours digit
#define HOURS_2 HOURS_1 + CHARACTER_WIDTH //x position for second hours digit
#define MIN_1 HOURS_2 + CHARACTER_WIDTH*2 //x position for first min digit
#define MIN_2 MIN_1 + CHARACTER_WIDTH //x position for second min digit
#define SECONDS_1 MIN_2 + CHARACTER_WIDTH*2 //x position for first second digit
#define SECONDS_2 SECONDS_1 + CHARACTER_WIDTH //x position for second second digit

//array index for each digit
#define HOURS_ARR_1 0x0 //first hours digit array index
#define HOURS_ARR_2 0x1 //second hours digit array index
#define MIN_ARR_1 0x3 //first minutes digit array index
#define MIN_ARR_2 0x4 //second minutes digit array index
#define SECONDS_ARR_1 0x6 //first seconds digit array index
#define SECONDS_ARR_2 0x7 //second seconds digit array index

//clock delays
#define ONE_TENTH 100 //clock delay for test fct
#define ONE_TWENTIETH 50 //clock delay for test fct

//time variables
#define CHAR_ARRAY_LENGTH 9
#define HOURS 12
static uint8_t hours = HOURS, seconds = 0, minutes = 0; //storing minutes, hours, and seconds
static char new_time[CHAR_ARRAY_LENGTH] = ("12:00:00"); //needs the null terminator at the end of the char array
static char current_display_time[CHAR_ARRAY_LENGTH] = ("12:00:00"); //storing current display time

//variables to let the incremental functions know if the seconds or minutes should roll over
#define NO_ROLL_OVER false
#define ROLL_OVER true

// Called only once - performs any necessary inits.
// This is a good place to draw the triangles and any other
// parts of the clock display that will never change.
void clockDisplay_init(){
    display_init();  // Must init all of the software and underlying hardware for LCD.
    display_fillScreen(DISPLAY_BLACK);  // Blank the screen.
    //upper-left triangle
    display_fillTriangle(DISPLAY_CENTER_X-(CHARACTER_WIDTH*FOUR_STEPS_AWAY), DISPLAY_CENTER_Y+CHARACTER_HEIGHT,
                         DISPLAY_CENTER_X-(CHARACTER_WIDTH*TWO_STEPS_AWAY), DISPLAY_CENTER_Y+CHARACTER_HEIGHT,
                         DISPLAY_CENTER_X-(CHARACTER_WIDTH*THREE_STEPS_AWAY), DISPLAY_CENTER_Y+(CHARACTER_HEIGHT*TWO_STEPS_AWAY), DISPLAY_GREEN);
    //upper-middle triangle
    display_fillTriangle(DISPLAY_CENTER_X-(CHARACTER_WIDTH), DISPLAY_CENTER_Y+CHARACTER_HEIGHT,
                         DISPLAY_CENTER_X+(CHARACTER_WIDTH), DISPLAY_CENTER_Y+CHARACTER_HEIGHT,
                         DISPLAY_CENTER_X, DISPLAY_CENTER_Y+(CHARACTER_HEIGHT*TWO_STEPS_AWAY), DISPLAY_GREEN);
    //upper-right triangle
    display_fillTriangle(DISPLAY_CENTER_X+(CHARACTER_WIDTH*FOUR_STEPS_AWAY), DISPLAY_CENTER_Y+CHARACTER_HEIGHT,
                         DISPLAY_CENTER_X+(CHARACTER_WIDTH*TWO_STEPS_AWAY), DISPLAY_CENTER_Y+CHARACTER_HEIGHT,
                         DISPLAY_CENTER_X+(CHARACTER_WIDTH*THREE_STEPS_AWAY), DISPLAY_CENTER_Y+(CHARACTER_HEIGHT*TWO_STEPS_AWAY), DISPLAY_GREEN);
    //lower-left triangle
    display_fillTriangle(DISPLAY_CENTER_X-(CHARACTER_WIDTH*FOUR_STEPS_AWAY), DISPLAY_CENTER_Y-CHARACTER_HEIGHT,
                         DISPLAY_CENTER_X-(CHARACTER_WIDTH*TWO_STEPS_AWAY), DISPLAY_CENTER_Y-CHARACTER_HEIGHT,
                         DISPLAY_CENTER_X-(CHARACTER_WIDTH*THREE_STEPS_AWAY), DISPLAY_CENTER_Y-(CHARACTER_HEIGHT*TWO_STEPS_AWAY), DISPLAY_GREEN);
    //lower-middle triangle
    display_fillTriangle(DISPLAY_CENTER_X-(CHARACTER_WIDTH), DISPLAY_CENTER_Y-CHARACTER_HEIGHT,
                         DISPLAY_CENTER_X+(CHARACTER_WIDTH), DISPLAY_CENTER_Y-CHARACTER_HEIGHT,
                         DISPLAY_CENTER_X, DISPLAY_CENTER_Y-(CHARACTER_HEIGHT*TWO_STEPS_AWAY), DISPLAY_GREEN);
    //lower-right triangle
    display_fillTriangle(DISPLAY_CENTER_X+(CHARACTER_WIDTH*FOUR_STEPS_AWAY), DISPLAY_CENTER_Y-CHARACTER_HEIGHT,
                         DISPLAY_CENTER_X+(CHARACTER_WIDTH*TWO_STEPS_AWAY), DISPLAY_CENTER_Y-CHARACTER_HEIGHT,
                         DISPLAY_CENTER_X+(CHARACTER_WIDTH*THREE_STEPS_AWAY), DISPLAY_CENTER_Y-(CHARACTER_HEIGHT*TWO_STEPS_AWAY), DISPLAY_GREEN);
    clockDisplay_updateTimeDisplay(UPDATE_ALL);
}

// Updates the time display with latest time, making sure to update only those digits that
// have changed since the last update.
// if forceUpdateAll is true, update all digits.
void clockDisplay_updateTimeDisplay(bool forceUpdateAll){
    //set the text size, color, and background color
    display_setTextColor(DISPLAY_GREEN, DISPLAY_BLACK); //set time text color to green
    display_setTextSize(TEXT_SIZE); //set numbers to current text size
    if(forceUpdateAll){ //update the entire clock for the current time
        sprintf(current_display_time, "%2hd:%02hd:%02hd", hours, minutes, seconds); //sets the current_display_time variable to the current time
        display_setCursor(HOURS_1, NUMBERS_Y); //set cursor to top left of first hours number
        display_print(current_display_time); //print the current time to the board
    }else{
        //need to compare the current display to the current hours down here
        sprintf(new_time, "%2hd:%02hd:%02hd", hours, minutes, seconds); //sets the current_display_time variable to the current time
        // check each index in both arrays to see if they're are different, if so, update
        if(new_time[HOURS_ARR_1] != current_display_time[HOURS_ARR_1]){ //update the hours first digit
            display_setCursor(HOURS_1, NUMBERS_Y); //set cursor to top left of first hours number
            display_print(new_time[HOURS_ARR_1]); //print the new first hours digit to the board
        }

        if(new_time[HOURS_ARR_2] != current_display_time[HOURS_ARR_2]){ //update the hours second digit
            display_setCursor(HOURS_2, NUMBERS_Y); //set the cursor to the second hours digit
            display_print(new_time[HOURS_ARR_2]); //print the new second hours digit
        }

        if(new_time[MIN_ARR_1] != current_display_time[MIN_ARR_1]){ //update the minutes first digit
            display_setCursor(MIN_1, NUMBERS_Y); //the cursor to the first minutes digit
            display_print(new_time[MIN_ARR_1]); //print the new first minutes digit
        }

        if(new_time[MIN_ARR_2] != current_display_time[MIN_ARR_2]){ //update the minutes second digit
            display_setCursor(MIN_2, NUMBERS_Y); //the cursor to the second minutes digit
            display_print(new_time[MIN_ARR_2]); //print the new seconds minutes digit
        }

        if(new_time[SECONDS_ARR_1] != current_display_time[SECONDS_ARR_1]){ //update the seconds first digit
            display_setCursor(SECONDS_1, NUMBERS_Y); //the cursor to the first seconds digit
            display_print(new_time[SECONDS_ARR_1]); //print the new first seconds digit
        }

        if(new_time[SECONDS_ARR_2] != current_display_time[SECONDS_ARR_2]){ //update the seconds second digit
            display_setCursor(SECONDS_2, NUMBERS_Y); //the cursor to the seconds second digit
            display_print(new_time[SECONDS_ARR_2]); //print the new second seconds digit
        }
    }
    sprintf(current_display_time, "%s", new_time); //set the current display time to the new time
}

//increments the hours
void clockDisplay_IncHrs(){
    if(hours == HOURS_ROLLOVER){ //checks for rollover
        hours = HOURS_ROLLUNDER; //rolls under
    }else{
        hours += SINGLE_INCDEC; //incremental as normal
    }
}

//decrements the hours
void clockDisplay_DecHrs(){
    if(hours == HOURS_ROLLUNDER){ //checks for rollunder
        hours = HOURS_ROLLOVER; //rolls over
    }else{
        hours -= SINGLE_INCDEC; //decrements as normal
    }

}

//increments the minutes
void clockDisplay_IncMin(bool autoIncDec){
    if(minutes == SECONDS_MINUTES_ROLLOVER){ //checks for rollover
        minutes = SECONDS_MINUTES_ROLLUNDER; //rolls under
        if(autoIncDec) //this will be true if we are incrementing by the second, so the increase of minutes should change the hours
            clockDisplay_IncHrs(); //increment the hours when the minutes rollover
    }else{
        minutes += SINGLE_INCDEC; //increments as normal
    }
}

//decrements the minutes
void clockDisplay_DecMin(bool autoIncDec){
    if(minutes == SECONDS_MINUTES_ROLLUNDER){ //checks for rollunder
        minutes = SECONDS_MINUTES_ROLLOVER; // rolls over
        if(autoIncDec) //this will be true if we are incrementing by the second, so the decrease of minutes should change the hours
            clockDisplay_DecHrs(); //if the minutes rollunder, we need to decrement the hours
    }else{
        minutes -= SINGLE_INCDEC; //decrements as normal
    }

}

//increments the seconds
void clockDisplay_IncSec(bool autoIncDec){
    if(seconds == SECONDS_MINUTES_ROLLOVER){ //if it will rollover, make it one instead
        seconds = SECONDS_MINUTES_ROLLUNDER; //rolls under
        if(autoIncDec)//this will be true if we are incrementing by the second, so the increase of seconds should change the minutes
            clockDisplay_IncMin(autoIncDec); //increment minutes when seconds rollover
    }else{
        seconds += SINGLE_INCDEC; //increments as normal
    }
}

//decrements the seconds
void clockDisplay_DecSec(bool autoIncDec){
    if(seconds == SECONDS_MINUTES_ROLLUNDER){ //if it will rollunder, make it 59 instead
        seconds = SECONDS_MINUTES_ROLLOVER;
        if(autoIncDec) //this will be true if we are incrementing by the second, so the decrease of minutes should change the hours
            clockDisplay_DecMin(autoIncDec); //decrement minutes when seconds rollunder
    }else{
        seconds -= SINGLE_INCDEC; //decrements as normal
    }
}

// Reads the touched coordinates and performs the increment or decrement,
// depending upon the touched region.
void clockDisplay_performIncDec(){
    int16_t x, y; //coordinates for touched point
    uint8_t z;
    display_getTouchedPoint(&x, &y, &z); //receives touched point

    if(y < DISPLAY_CENTER_Y){ //top half of touch screen, we are incrementing
        if(x < LEFT_BUTTONS){ //hours
            clockDisplay_IncHrs();
        }else if(x > MIDDLE_BUTTONS){ //seconds
            clockDisplay_IncSec(NO_ROLL_OVER);
        }else{ //otherwise, minutes
            clockDisplay_IncMin(NO_ROLL_OVER);
        }
    }else{ //bottom half of the touch screen, we are decrementing
        if(x < LEFT_BUTTONS){ //hours
            clockDisplay_DecHrs();
        }else if(x > MIDDLE_BUTTONS){ //seconds
            clockDisplay_DecSec(NO_ROLL_OVER);
        }else{ //otherwise, minutes
            clockDisplay_DecMin(NO_ROLL_OVER);
        }
    }
    clockDisplay_updateTimeDisplay(NOT_UPDATE_ALL); //we don't want to update everything here, only the things that have changed
}

// Advances the time forward by 1 second and update the display.
void clockDisplay_advanceTimeOneSecond(){
    clockDisplay_IncSec(ROLL_OVER);
    clockDisplay_updateTimeDisplay(NOT_UPDATE_ALL); //we don't want to update everything here, only the things that have changed
}

// Run a test of clock-display functions.
void clockDisplay_runTest(){
    clockDisplay_init(); //init clock
    uint16_t num_of_touches = CLEAR; //zero touches
    while(num_of_touches < TEST_TOUCHES){ //gives the tester 150 touches to test the incrementing and decrementing
        if(display_isTouched()){ //someone is touching, so we need to update our stuff
            display_clearOldTouchData(); //get ready for the next touch by clearing data
            utils_msDelay(ONE_TWENTIETH); //wait 50 ms before getting the touched point, get rid of this for the state machine, I assume
            clockDisplay_performIncDec(); //perform the incrementing or the decrementing
            num_of_touches++;
        }
    }
    for(uint16_t i = 0; i < ONE_TENTH; i++){
        //advance the clock one second
        clockDisplay_advanceTimeOneSecond();
        //update display
        clockDisplay_updateTimeDisplay(NOT_UPDATE_ALL); //only update new values
        utils_msDelay(ONE_TENTH); //delay for 1/10 of a second
    }
    //done with run test
}

