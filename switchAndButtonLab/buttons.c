/*
 * buttons.c
 *
 *  Created on: Mar 21, 2018
 *      Author: logan
 */

#include "buttons.h"
#include "xil_io.h"
#include "supportFiles/display.h"
#include <stdio.h>

//constants in lab specs
#define BUTTONS_GPIO_DEVICE_BASE_ADDRESS XPAR_PUSH_BUTTONS_BASEADDR //BUTTONS BASE ADDRESS
#define GPIO_DATA_OFFSET 0x0 //DATA_REGISTERS BASELINE OFFSET
#define GPIO_TRI_OFFSET 0x4 //TRI_OFFSET

//constants to denote each button
#define BUTTON_00 0x0 //denotes bit value for button 0
#define BUTTON_01 0x1 //denotes bit value for button 1
#define BUTTON_02 0x2 //denotes bit value for button 2
#define BUTTON_03 0x3 //denotes bit value for button 3
#define B0 0x1 //denotes decimal value for button 0
#define B1 0x2 //denotes decimal value for button 1
#define B2 0x4 //denotes decimal value for button 2
#define B3 0x8 //denotes decimal value for button 3

//constant to denote all buttons being turned on
#define ALL_BUTTONS_ON 0xF

//constants for LCD locations
#define TOP_CORNER 0x0 //sets up top corner to write text
#define EIGHTH_SCREEN_WIDTH display_width()/8 //splits screen for rectangles
#define QUARTER_SCREEN_WIDTH display_width()/4 //splits screen for rectangles
#define QUARTER_SCREEN_HEIGHT display_height()/4 //splits screen for rectangles
#define HALF_SCREEN_WIDTH display_width()/2 //splits screen for rectangles
#define HALF_SCREEN_HEIGHT display_height()/2 //splits screen for rectangles
#define DEFAULT_TEXT_SIZE 0x2 //sets default text size
#define HALF_TEXT_SIZE DEFAULT_TEXT_SIZE/2 //sets half default text size
#define TEXT_WIDTH_OFFSET HALF_TEXT_SIZE - 20 //variable to position text properly
#define NOT_BLACK 0x1111 //check if something is not black
#define ALL_INPUT 0xF //CHECK TO MAKE SURE GPIO_TRI IS INPUT

//constants to define button action
#define PRESSED 0x1 // button pressed - 1
#define NOT_PRESSED 0x0 //button not pressed - 2

//write to the GPIO TRI Register to set it as input
void buttons_writeGpioRegister(int32_t offset, int32_t value){ //helper function to write gpio register
    Xil_Out32(BUTTONS_GPIO_DEVICE_BASE_ADDRESS + offset, value); //lets us read the GPIO registers in C
}

// Helper function to read GPIO registers.
int32_t buttons_readGpioRegister(int32_t offset) {
//   Just use the low-level Xilinx call.
   return Xil_In32(BUTTONS_GPIO_DEVICE_BASE_ADDRESS + offset); //lets us read the GPIO registers in C
}

// Initializes the button driver software and hardware. Returns one of the defined status values.
int32_t buttons_init(){
    display_init();  // Must init all of the software and underlying hardware for LCD.
    display_fillScreen(DISPLAY_BLACK);  // Blank the screen.
    buttons_writeGpioRegister(GPIO_TRI_OFFSET, ALL_INPUT); //write the registers as inputs
    if(buttons_readGpioRegister(GPIO_TRI_OFFSET) != ALL_INPUT){ //checking to make sure there registers are inputs
        return BUTTONS_INIT_STATUS_FAIL; //written improperly
    }
    return BUTTONS_INIT_STATUS_OK; //the GPIO TRI was written properly
}

// Returns the current value of all 4 buttons as the lower 4 bits of the returned value.
// bit3 = BTN3, bit2 = BTN2, bit1 = BTN1, bit0 = BTN0.
int32_t buttons_read(){
    return buttons_readGpioRegister(GPIO_DATA_OFFSET);
}

//takes in which rectangle is being written and writes test accordingly
void buttons_writeText(int32_t rectToWrite){
    //switch case depending on the button pressed
    switch(rectToWrite){
    case BUTTON_00: //for BTN0
        //sets the cursor where it needs to be
        display_setCursor(display_width()-EIGHTH_SCREEN_WIDTH-TEXT_WIDTH_OFFSET, QUARTER_SCREEN_HEIGHT-HALF_TEXT_SIZE);
        display_setTextSize(DEFAULT_TEXT_SIZE); //initializes text size
        display_setTextColor(DISPLAY_BLACK);
        display_print("BTN0"); //displays text
        break;
    case BUTTON_01: //for BTN1
        //sets the cursor where it needs to be
        display_setCursor(HALF_SCREEN_WIDTH+EIGHTH_SCREEN_WIDTH-TEXT_WIDTH_OFFSET, QUARTER_SCREEN_HEIGHT-HALF_TEXT_SIZE);
        display_setTextSize(DEFAULT_TEXT_SIZE); //initializes text size
        display_setTextColor(DISPLAY_BLACK);
        display_print("BTN1"); //displays text
        break;
    case BUTTON_02: //for BTN2
        //sets the cursor where it needs to be
        display_setCursor(HALF_SCREEN_WIDTH-EIGHTH_SCREEN_WIDTH-TEXT_WIDTH_OFFSET, QUARTER_SCREEN_HEIGHT-HALF_TEXT_SIZE);
        display_setTextSize(DEFAULT_TEXT_SIZE); //initializes text size
        display_setTextColor(DISPLAY_WHITE);
        display_print("BTN2"); //displays text
        break;
    case BUTTON_03: //for BTN3
        //sets the cursor where it needs to be
        display_setCursor(EIGHTH_SCREEN_WIDTH-TEXT_WIDTH_OFFSET, QUARTER_SCREEN_HEIGHT-HALF_TEXT_SIZE);
        display_setTextSize(DEFAULT_TEXT_SIZE); //initializes text size
        display_setTextColor(DISPLAY_WHITE);
        display_print("BTN3"); //displays text
        break;
    default: //should never reach this value
        return; //if it does, return
    }
}

//based on the button passed in, will create a rectangle for that button
void buttons_writeRectangle(int32_t rectToWrite, int16_t rect_color){
    switch(rectToWrite){ //switch statement depending on what button was pressed
    case BUTTON_00: //first button is pressed
        if(rect_color != DISPLAY_BLACK){ //means the button was not already pressed and needs to fill with color
            //draws colored rectangle
            display_fillRect(display_width()-QUARTER_SCREEN_WIDTH, TOP_CORNER, QUARTER_SCREEN_WIDTH, HALF_SCREEN_HEIGHT, DISPLAY_YELLOW);
            //needs to display BTN text - use the letter height to determine the top right corner of where I should write the button name
            buttons_writeText(rectToWrite);
        }else{ //this means I need to display a black rectangle with no text
            display_fillRect(display_width()-QUARTER_SCREEN_WIDTH, TOP_CORNER, QUARTER_SCREEN_WIDTH, HALF_SCREEN_HEIGHT, DISPLAY_BLACK);
        }
        break;
    case BUTTON_01: //second button is pressed
        if(rect_color != DISPLAY_BLACK){ //means the button was not already pressed and needs to fill with color
            //draws rectangle
            display_fillRect(HALF_SCREEN_WIDTH, TOP_CORNER, QUARTER_SCREEN_WIDTH, HALF_SCREEN_HEIGHT, DISPLAY_GREEN);
            buttons_writeText(rectToWrite);
        }else{ //this means I need to display a black rectangle with no text
            display_fillRect(HALF_SCREEN_WIDTH, TOP_CORNER, QUARTER_SCREEN_WIDTH, HALF_SCREEN_HEIGHT, DISPLAY_BLACK);
        }
        break;
    case BUTTON_02: //third button is pressed
        if(rect_color != DISPLAY_BLACK){ //means the button was not already pressed and needs to fill with color
            //draws rectangle
            display_fillRect(QUARTER_SCREEN_WIDTH, TOP_CORNER, QUARTER_SCREEN_WIDTH, HALF_SCREEN_HEIGHT, DISPLAY_RED);
            buttons_writeText(rectToWrite);
        }else{ //this means I need to display a black rectangle with no text
            display_fillRect(QUARTER_SCREEN_WIDTH, TOP_CORNER, QUARTER_SCREEN_WIDTH, HALF_SCREEN_HEIGHT, DISPLAY_BLACK);
        }
        break;
    case BUTTON_03: //fourth button is pressed
        if(rect_color != DISPLAY_BLACK){ //means the button was not already pressed and needs to fill with color
            //draws rectangle
            display_fillRect(TOP_CORNER, TOP_CORNER, QUARTER_SCREEN_WIDTH, HALF_SCREEN_HEIGHT, DISPLAY_BLUE);
            buttons_writeText(rectToWrite);
        }else{ //this means I need to display a black rectangle with no text
            display_fillRect(TOP_CORNER, TOP_CORNER, QUARTER_SCREEN_WIDTH, HALF_SCREEN_HEIGHT, DISPLAY_BLACK);
        }
        break;
    default:
        return;
    }
}

// Runs a test of the buttons. As you push the buttons, graphics and messages will be written to the LCD
// panel. The test will until all 4 pushbuttons are simultaneously pressed.
void buttons_runTest(){
    if(buttons_init() == BUTTONS_INIT_STATUS_FAIL){ //if the GPIO register isn't written properly, we don't operate
        printf("buttons_init failed!\n\r"); //print buttons init failed
        return;
    }
    buttons_init(); //init the buttons

    //read from the button input
    uint32_t buttons = buttons_read();

    //initialize each button to not-pressed to start, change these values when they are pressed so the logic will work
    uint8_t B0pressed = NOT_PRESSED; //setting button 0 to not-pressed
    uint8_t B1pressed = NOT_PRESSED; //setting button 1 to not-pressed
    uint8_t B2pressed = NOT_PRESSED; //setting button 2 to not-pressed
    uint8_t B3pressed = NOT_PRESSED; //setting button 3 to not-pressed

    while(buttons != ALL_BUTTONS_ON){ //stop running test when all buttons are pressed
        if((B0 & buttons) && !B0pressed){ //means BTN0 is pressed and wasn't already pressed
            //fill in rectangle here
            buttons_writeRectangle(BUTTON_00, NOT_BLACK);
            B0pressed = PRESSED; //reset button pressed logic
        }
        else if (!(B0 & buttons) && B0pressed){ //means BTN0 is no longer pressed
            //fill with blackness here
            buttons_writeRectangle(BUTTON_00, DISPLAY_BLACK);
            B0pressed = NOT_PRESSED; //reset button pressed logic
        }

        if((B1 & buttons) && !B1pressed){ //means BTN1 is pressed and wasn't already pressed
            //fill in rectangle here
            buttons_writeRectangle(BUTTON_01, NOT_BLACK);
            B1pressed = PRESSED; //reset button pressed logic
        }
        else if (!(B1 & buttons) && B1pressed){ //means BTN1 is no longer pressed
            //fill with blackness here
            buttons_writeRectangle(BUTTON_01, DISPLAY_BLACK);
            B1pressed = NOT_PRESSED; //reset button pressed logic
        }

        if((B2 & buttons) && !B2pressed){ //means BTN2 is pressed and wasn't already pressed
            //fill in rectangle here
            buttons_writeRectangle(BUTTON_02, NOT_BLACK);
            B2pressed = PRESSED; //reset button pressed logic
        }
        else if (!(B2 & buttons) && B2pressed){ //means BTN2 is no longer pressed
            //fill with blackness here
            buttons_writeRectangle(BUTTON_02, DISPLAY_BLACK);
            B2pressed = NOT_PRESSED; //reset button pressed logic
        }

        if((B3 & buttons) && !B3pressed){ //means BTN3 is pressed and wasn't already pressed
            //fill in rectangle here
            buttons_writeRectangle(BUTTON_03, NOT_BLACK);
            B3pressed = PRESSED; //reset button pressed logic
        }
        else if (!(B3 & buttons) && B3pressed){ //means BTN3 is no longer pressed
            //fill with blackness here
            buttons_writeRectangle(BUTTON_03, DISPLAY_BLACK);
            B3pressed = NOT_PRESSED; //reset button pressed logic
        }

        buttons = buttons_read(); //continues to read the buttons at each iteration of the while loop
    }

    display_fillScreen(DISPLAY_BLACK);  // Blank the screen after the test is done running
}


