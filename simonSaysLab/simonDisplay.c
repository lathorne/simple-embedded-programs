#include "simonDisplay.h" //include .h
#include <stdbool.h>
#include <stdint.h>
#include "supportFiles/utils.h" //include utils
#include <stdio.h>
#include "supportFiles/display.h" //include display functions

// Width, height of the simon "buttons"
#define SIMON_DISPLAY_BUTTON_WIDTH 60 //button width
#define SIMON_DISPLAY_BUTTON_HEIGHT 60 //button height
#define HALF_BUTTON_WIDTH SIMON_DISPLAY_BUTTON_WIDTH/2 //half button width
#define HALF_BUTTON_HEIGHT SIMON_DISPLAY_BUTTON_HEIGHT/2 //half button height

// Width, height of the simon "squares.
// Note that the video shows the squares as larger but you
// can use this smaller value to make the game easier to implement speed-wise.
#define SIMON_DISPLAY_SQUARE_WIDTH  120 //width of square
#define SIMON_DISPLAY_SQUARE_HEIGHT 120 //height of square

#define HALF_DISPLAY_X display_width()/2 //half display x value
#define HALF_DISPLAY_Y display_height()/2 //half display y value

#define REGION_0_SQUARE_START_X HALF_DISPLAY_X/2 - SIMON_DISPLAY_BUTTON_WIDTH/2 //region zero x
#define REGION_0_SQUARE_START_Y HALF_DISPLAY_Y/2 - SIMON_DISPLAY_BUTTON_WIDTH/2 //region zero y

#define REGION_1_SQUARE_START_X HALF_DISPLAY_X + HALF_DISPLAY_X/2 - SIMON_DISPLAY_BUTTON_WIDTH/2 //region one x
#define REGION_1_SQUARE_START_Y HALF_DISPLAY_Y/2 - SIMON_DISPLAY_BUTTON_WIDTH/2 //region one y

#define REGION_2_SQUARE_START_X HALF_DISPLAY_X/2 - SIMON_DISPLAY_BUTTON_WIDTH/2 //region 2 x
#define REGION_2_SQUARE_START_Y HALF_DISPLAY_Y + HALF_DISPLAY_Y/2 - SIMON_DISPLAY_BUTTON_WIDTH/2 //region 2 y

#define REGION_3_SQUARE_START_X HALF_DISPLAY_X + HALF_DISPLAY_X/2 - SIMON_DISPLAY_BUTTON_WIDTH/2 //region 3 x
#define REGION_3_SQUARE_START_Y HALF_DISPLAY_Y + HALF_DISPLAY_Y/2 - SIMON_DISPLAY_BUTTON_WIDTH/2 //region 3 y

#define SQUARE_OFFSET 30 //square offset

#define DRAW false //variable to draw
#define ERASE true //variable to erase

// Given coordinates from the touch pad, computes the region number.

// The entire touch-screen is divided into 4 rectangular regions, numbered 0 - 3.
// Each region will be drawn with a different color. Colored buttons remind
// the user which square is associated with each color. When you press
// a region, computeRegionNumber returns the region number that is used
// by the other routines.
/*
|----------|----------|
|          |          |
|    0     |     1    |
|  (RED)   | (YELLOW) |
-----------------------
|          |          |
|     2    |    3     |
|  (BLUE)  |  (GREEN) |
-----------------------
*/

// These are the definitions for the regions.
#define SIMON_DISPLAY_REGION_0 0 //region 0
#define SIMON_DISPLAY_REGION_1 1 //region 1
#define SIMON_DISPLAY_REGION_2 2 //region 2
#define SIMON_DISPLAY_REGION_3 3 //region 3

int8_t simonDisplay_computeRegionNumber(int16_t x, int16_t y){
    if(x < HALF_DISPLAY_X){ //left half of screen
        if(y < HALF_DISPLAY_Y){ //top half of screen
            return SIMON_DISPLAY_REGION_0;
        }else{ //bottom half of screen
            return SIMON_DISPLAY_REGION_2;
        }
    }else{ //right half of screen
        if(y < HALF_DISPLAY_Y){ //top half of screen
            return SIMON_DISPLAY_REGION_1;
        }else{ //bottom half of screen
            return SIMON_DISPLAY_REGION_3;
        }
    }
}

// Draws a colored "button" that the user can touch.
// The colored button is centered in the region but does not fill the region.
// If erase argument is true, draws the button as black background to erase it.
void simonDisplay_drawButton(uint8_t regionNumber, bool erase){
    if(regionNumber == SIMON_DISPLAY_REGION_0){ //button in zone zero
        if(erase){ //needs erasing
            display_fillRect(REGION_0_SQUARE_START_X, REGION_0_SQUARE_START_Y, SIMON_DISPLAY_BUTTON_WIDTH, SIMON_DISPLAY_BUTTON_HEIGHT, DISPLAY_BLACK);
        }else{ //draw the actual button
            display_fillRect(REGION_0_SQUARE_START_X, REGION_0_SQUARE_START_Y, SIMON_DISPLAY_BUTTON_WIDTH, SIMON_DISPLAY_BUTTON_HEIGHT, DISPLAY_RED);
        }
    }else if(regionNumber == SIMON_DISPLAY_REGION_1){ //button in zone one
        if(erase){ //needs erasing
            display_fillRect(REGION_1_SQUARE_START_X, REGION_1_SQUARE_START_Y, SIMON_DISPLAY_BUTTON_WIDTH, SIMON_DISPLAY_BUTTON_HEIGHT, DISPLAY_BLACK);
        }else{ //draw the actual button
            display_fillRect(REGION_1_SQUARE_START_X, REGION_1_SQUARE_START_Y, SIMON_DISPLAY_BUTTON_WIDTH, SIMON_DISPLAY_BUTTON_HEIGHT, DISPLAY_YELLOW);
        }
    }else if(regionNumber == SIMON_DISPLAY_REGION_2){ //button in zone two
        if(erase){ //needs erasing
            display_fillRect(REGION_2_SQUARE_START_X, REGION_2_SQUARE_START_Y, SIMON_DISPLAY_BUTTON_WIDTH, SIMON_DISPLAY_BUTTON_HEIGHT, DISPLAY_BLACK);
        }else{ //draw the actual button
            display_fillRect(REGION_2_SQUARE_START_X, REGION_2_SQUARE_START_Y, SIMON_DISPLAY_BUTTON_WIDTH, SIMON_DISPLAY_BUTTON_HEIGHT, DISPLAY_BLUE);
        }
    }else if(regionNumber == SIMON_DISPLAY_REGION_3){ //button in zone three
        if(erase){ //needs erasing
            display_fillRect(REGION_3_SQUARE_START_X, REGION_3_SQUARE_START_Y, SIMON_DISPLAY_BUTTON_WIDTH, SIMON_DISPLAY_BUTTON_HEIGHT, DISPLAY_BLACK);
        }else{ //draw the actual button
            display_fillRect(REGION_3_SQUARE_START_X, REGION_3_SQUARE_START_Y, SIMON_DISPLAY_BUTTON_WIDTH, SIMON_DISPLAY_BUTTON_HEIGHT, DISPLAY_GREEN);
        }
    }
}

// Convenience function that draws all of the buttons.
void simonDisplay_drawAllButtons(){
    simonDisplay_drawButton(SIMON_DISPLAY_REGION_0, DRAW); //draw first button
    simonDisplay_drawButton(SIMON_DISPLAY_REGION_1, DRAW); //draw second button
    simonDisplay_drawButton(SIMON_DISPLAY_REGION_2, DRAW); //draw third button
    simonDisplay_drawButton(SIMON_DISPLAY_REGION_3, DRAW); //draw last button
}

// Convenience function that erases all of the buttons.
void simonDisplay_eraseAllButtons(){
    simonDisplay_drawButton(SIMON_DISPLAY_REGION_0, ERASE); //erase first button
    simonDisplay_drawButton(SIMON_DISPLAY_REGION_1, ERASE); //erase second button
    simonDisplay_drawButton(SIMON_DISPLAY_REGION_2, ERASE); //erase third button
    simonDisplay_drawButton(SIMON_DISPLAY_REGION_3, ERASE); //erase last button
}

// Draws a bigger square that completely fills the region.
// If the erase argument is true, it draws the square as black background to "erase" it.
void simonDisplay_drawSquare(uint8_t regionNo, bool erase){
    if(regionNo == SIMON_DISPLAY_REGION_0){ //square in zone zero
        if(erase){ //needs erasing
            display_fillRect(REGION_0_SQUARE_START_X - SQUARE_OFFSET, REGION_0_SQUARE_START_Y - SQUARE_OFFSET, SIMON_DISPLAY_SQUARE_WIDTH, SIMON_DISPLAY_SQUARE_HEIGHT, DISPLAY_BLACK);
        }else{ //draw the actual square
            display_fillRect(REGION_0_SQUARE_START_X - SQUARE_OFFSET, REGION_0_SQUARE_START_Y - SQUARE_OFFSET, SIMON_DISPLAY_SQUARE_WIDTH, SIMON_DISPLAY_SQUARE_HEIGHT, DISPLAY_RED);
        }
    }else if(regionNo == SIMON_DISPLAY_REGION_1){ //square in zone one
        if(erase){ //needs erasing
            display_fillRect(REGION_1_SQUARE_START_X - SQUARE_OFFSET, REGION_1_SQUARE_START_Y - SQUARE_OFFSET, SIMON_DISPLAY_SQUARE_WIDTH, SIMON_DISPLAY_SQUARE_HEIGHT, DISPLAY_BLACK);
        }else{ //draw the actual square
            display_fillRect(REGION_1_SQUARE_START_X - SQUARE_OFFSET, REGION_1_SQUARE_START_Y - SQUARE_OFFSET, SIMON_DISPLAY_SQUARE_WIDTH, SIMON_DISPLAY_SQUARE_HEIGHT, DISPLAY_YELLOW);
        }
    }else if(regionNo == SIMON_DISPLAY_REGION_2){ //square in zone two
        if(erase){ //needs erasing
            display_fillRect(REGION_2_SQUARE_START_X - SQUARE_OFFSET, REGION_2_SQUARE_START_Y - SQUARE_OFFSET, SIMON_DISPLAY_SQUARE_WIDTH, SIMON_DISPLAY_SQUARE_HEIGHT, DISPLAY_BLACK);
        }else{ //draw the actual square
            display_fillRect(REGION_2_SQUARE_START_X - SQUARE_OFFSET, REGION_2_SQUARE_START_Y - SQUARE_OFFSET, SIMON_DISPLAY_SQUARE_WIDTH, SIMON_DISPLAY_SQUARE_HEIGHT, DISPLAY_BLUE);
        }
    }else if(regionNo == SIMON_DISPLAY_REGION_3){ //square in zone three
        if(erase){ //needs erasing
            display_fillRect(REGION_3_SQUARE_START_X - SQUARE_OFFSET, REGION_3_SQUARE_START_Y - SQUARE_OFFSET, SIMON_DISPLAY_SQUARE_WIDTH, SIMON_DISPLAY_SQUARE_HEIGHT, DISPLAY_BLACK);
        }else{ //draw the actual square
            display_fillRect(REGION_3_SQUARE_START_X - SQUARE_OFFSET, REGION_3_SQUARE_START_Y - SQUARE_OFFSET, SIMON_DISPLAY_SQUARE_WIDTH, SIMON_DISPLAY_SQUARE_HEIGHT, DISPLAY_GREEN);
        }
    }
}

#define TOUCH_PANEL_ANALOG_PROCESSING_DELAY_IN_MS 60 // in ms
#define MAX_STR 255
#define TEXT_SIZE 2
#define TEXT_VERTICAL_POSITION 0
#define TEXT_HORIZONTAL_POSITION (display_height()/2)
#define INSTRUCTION_LINE_1 "Touch and release to start the Simon demo."
#define INSTRUCTION_LINE_2 "Demo will terminate after %d touches."
#define DEMO_OVER_MESSAGE_LINE_1 "Simon demo terminated"
#define DEMO_OVER_MESSAGE_LINE_2 "after %d touches."
#define TEXT_VERTICAL_POSITION 0 // Start at the far left.
#define ERASE_THE_SQUARE true  // drawSquare() erases if this is passed in.
#define DRAW_THE_SQUARE false  // drawSquare() draws the square if this is passed in.
#define ERASE_THE_BUTTON true  // drawSquare() erases if this is passed in.
#define DRAW_THE_BUTTON false  // drawSquare() draws the square if this is passed in.
// Runs a brief demonstration of how buttons can be pressed and squares lit up to implement the user
// interface of the Simon game. The routine will continue to run until the touchCount has been reached, e.g.,
// the user has touched the pad touchCount times.

// I used a busy-wait delay (utils_msDelay) that uses a for-loop and just blocks until the time has passed.
// When you implement the game, you CANNOT use this function as we discussed in class. Implement the delay
// using the non-blocking state-machine approach discussed in class.
void simonDisplay_runTest(uint16_t touchCount) {
  display_init();         // Always initialize the display.
  char str[MAX_STR];      // Enough for some simple printing.
  uint8_t regionNumber = 0;   // Convenience variable.
  uint16_t touches = 0;   // Terminate when you receive so many touches.
  // Write an informational message and wait for the user to touch the LCD.
  display_fillScreen(DISPLAY_BLACK);              // clear the screen.
  display_setCursor(TEXT_VERTICAL_POSITION, TEXT_HORIZONTAL_POSITION); // move to the middle of the screen.
  display_setTextSize(TEXT_SIZE);                 // Set the text size for the instructions.
  display_setTextColor(DISPLAY_RED, DISPLAY_BLACK);   // Reasonable text color.
  sprintf(str, INSTRUCTION_LINE_1);                   // Copy the line to a buffer.
  display_println(str);                               // Print to the LCD.
  display_println();                                  // new-line.
  sprintf(str, INSTRUCTION_LINE_2, touchCount);       // Copy the line to a buffer.
  display_println(str);                               // Print to the LCD.
  while (!display_isTouched());       // Wait here until the screen is touched.
  while (display_isTouched());        // Now wait until the touch is released.
  display_fillScreen(DISPLAY_BLACK);  // Clear the screen.
  simonDisplay_drawAllButtons();      // Draw all of the buttons.
  bool touched = false;         // Keep track of when the pad is touched.
  int16_t x, y;                     // Use these to keep track of coordinates.
  uint8_t z;                        // This is the relative touch pressure.
  while (touches < touchCount) {  // Run the loop according to the number of touches passed in.
    if (!display_isTouched() && touched) {          // user has stopped touching the pad.
      simonDisplay_drawSquare(regionNumber, ERASE_THE_SQUARE);  // Erase the square.
      simonDisplay_drawButton(regionNumber, DRAW_THE_BUTTON);  //  Draw the button.
      touched = false;                  // Released the touch, set touched to false.
    } else if (display_isTouched() && !touched) {   // User started touching the pad.
      touched = true;                             // Just touched the pad, set touched = true.
      touches++;                                  // Keep track of the number of touches.
      display_clearOldTouchData();                // Get rid of data from previous touches.
      // Must wait this many milliseconds for the chip to do analog processing.
      utils_msDelay(TOUCH_PANEL_ANALOG_PROCESSING_DELAY_IN_MS);
      display_getTouchedPoint(&x, &y, &z);        // After the wait, get the touched point.
      regionNumber = simonDisplay_computeRegionNumber(x, y);// Compute the region number, see above.
      simonDisplay_drawSquare(regionNumber, DRAW_THE_SQUARE);  // Draw the square (erase = false).
    }
  }
  // Done with the demo, write an informational message to the user.
  display_fillScreen(DISPLAY_BLACK);        // clear the screen.
  // Place the cursor in the middle of the screen.
  display_setCursor(TEXT_VERTICAL_POSITION, TEXT_HORIZONTAL_POSITION);
  display_setTextSize(TEXT_SIZE); // Make it readable.
  display_setTextColor(DISPLAY_RED, DISPLAY_BLACK);  // red is foreground color, black is background color.
  sprintf(str, DEMO_OVER_MESSAGE_LINE_1);    // Format a string using sprintf.
  display_println(str);                     // Print it to the LCD.
  sprintf(str, DEMO_OVER_MESSAGE_LINE_2, touchCount);  // Format the rest of the string.
  display_println(str);  // Print it to the LCD.
}
