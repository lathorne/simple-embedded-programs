#include "ticTacToeDisplay.h"
#include "supportFiles/display.h"
#include <stdio.h>
#include "../switchAndButtonLab/buttons.h"
#include "../switchAndButtonLab/switches.h"
#include "supportFiles/utils.h"

//lines of tic tac toe board
#define FIRST_HOR_LINE display_height()/3 //first horizontal line for board
#define SECOND_HOR_LINE 2*FIRST_HOR_LINE //second horizontal line for board
#define FIRST_VERT_LINE display_width()/3 //first vertical line for board
#define SECOND_VERT_LINE 2*FIRST_VERT_LINE //second vertical line for board
#define SIDE_OF_SCREEN 0 //side of screen location
#define WIDTH display_width() //width of display
#define HEIGHT display_height() //height of display

//row and column numbers
#define ROW_0 0 //first row
#define ROW_1 1 //second row
#define ROW_2 2 //third row

//coordinate for drawing Os
#define CENTER_OF_ROW FIRST_HOR_LINE/2 //center of row
#define CENTER_OF_COLUMN FIRST_VERT_LINE/2 //center of column
#define WIDTH_OF_X_OR_O RADIUS*2 //size of X or O

//coordinates for drawing Xs
#define TOP_LEFT_X CENTER_OF_COLUMN - RADIUS //top left of x
#define TOP_LEFT_Y CENTER_OF_ROW - RADIUS //top left of x

#define TOP_RIGHT_X CENTER_OF_COLUMN + RADIUS //top right of x
#define TOP_RIGHT_Y CENTER_OF_ROW - RADIUS //top right of x

#define BOTTOM_LEFT_X CENTER_OF_COLUMN - RADIUS //bottom left of x
#define BOTTOM_LEFT_Y CENTER_OF_ROW + RADIUS// bottom left of x

#define BOTTOM_RIGHT_X CENTER_OF_COLUMN + RADIUS //bottom right of x
#define BOTTOM_RIGHT_Y CENTER_OF_ROW + RADIUS //bottom right of x

//radius of circle
#define RADIUS CENTER_OF_ROW*2/3

//checking buttons and switches
#define RESET_MASK 1 //mask for resetting
#define TERMINATE_MASK 2 //mask for terminating test
#define RESET 1 //reset value
#define TERMINATE 2 //terminate value
#define SWITCH_ON_MASK 1 //mask for checking if the switch is on
#define SWITCH_ON 1 //switch value

//variables for running Test
#define WAIT_TIME 50 //50ms
#define DO_NOT_ERASE false //do not erase drawn characters on board

// Inits the tic-tac-toe display, draws the lines that form the board.
void ticTacToeDisplay_init(){
    display_init(); //init display
    display_fillScreen(DISPLAY_BLACK);  // Blank the screen.
    ticTacToeDisplay_drawBoardLines(); //draw the board lines
}

// Draws an X at the specified row and column.
// erase == true means to erase the X by redrawing it as background. erase == false, draw the X as foreground.
// row and column are numbers 0-2
void ticTacToeDisplay_drawX(uint8_t row, uint8_t column, bool erase){
    if(!erase){ //draw green
        display_drawLine(TOP_LEFT_X+(FIRST_VERT_LINE*column), TOP_LEFT_Y+(FIRST_HOR_LINE*row), BOTTOM_RIGHT_X+(FIRST_VERT_LINE*column), BOTTOM_RIGHT_Y+(FIRST_HOR_LINE*row), DISPLAY_GREEN);
        display_drawLine(BOTTOM_LEFT_X+(FIRST_VERT_LINE*column), BOTTOM_LEFT_Y+(FIRST_HOR_LINE*row), TOP_RIGHT_X+(FIRST_VERT_LINE*column), TOP_RIGHT_Y+(FIRST_HOR_LINE*row), DISPLAY_GREEN);
    }else{ //fill in black
        display_drawLine(TOP_LEFT_X+(FIRST_VERT_LINE*column), TOP_LEFT_Y+(FIRST_HOR_LINE*row), BOTTOM_RIGHT_X+(FIRST_VERT_LINE*column), BOTTOM_RIGHT_Y+(FIRST_HOR_LINE*row), DISPLAY_BLACK);
        display_drawLine(BOTTOM_LEFT_X+(FIRST_VERT_LINE*column), BOTTOM_LEFT_Y+(FIRST_HOR_LINE*row), TOP_RIGHT_X+(FIRST_VERT_LINE*column), TOP_RIGHT_Y+(FIRST_HOR_LINE*row), DISPLAY_BLACK);
    }
}

// Draws an O at the specified row and column.
// erase == true means to erase the O by redrawing it as background. erase == false, draw the O as foreground.
// row and column are numbers 0-2
void ticTacToeDisplay_drawO(uint8_t row, uint8_t column, bool erase){
    if(!erase){ //draw green
        display_drawCircle(CENTER_OF_COLUMN + (FIRST_VERT_LINE*column), CENTER_OF_ROW + (FIRST_HOR_LINE*row), RADIUS , DISPLAY_GREEN);
    }else{ //fill in black
        display_drawCircle(CENTER_OF_COLUMN + (FIRST_VERT_LINE*column), CENTER_OF_ROW + (FIRST_HOR_LINE*row), RADIUS , DISPLAY_BLACK);
    }
}

// After a touch has been detected and after the proper delay, this sets the row and column arguments
// according to where the user touched the board.
void ticTacToeDisplay_touchScreenComputeBoardRowColumn(uint8_t* row, uint8_t* column){
    uint8_t space_0 = (uint8_t)ROW_0, space_1 = (uint8_t)ROW_1, space_2 = (uint8_t)ROW_2;
    int16_t x, y; //coordinates for touched point
    uint8_t z;
    display_getTouchedPoint(&x, &y, &z); //receives touched point

    *row = y < FIRST_HOR_LINE ? space_0 : y > SECOND_HOR_LINE ? space_2 : space_1; //setting appropriate row for touch
    *column = x < FIRST_VERT_LINE ? space_0 : x > SECOND_VERT_LINE ? space_2 : space_1; //setting appropriate column for touch
}

// Runs a test of the display. Does the following.
// Draws the board. Each time you touch one of the screen areas, the screen will paint
// an X or an O, depending on whether switch 0 (SW0) is slid up (O) or down (X).
// When BTN0 is pushed, the screen is cleared. The test terminates when BTN1 is pushed.
void ticTacToeDisplay_runTest(){
    uint8_t *row, *column;
    while(1){ //forever loop
        if((buttons_read() & RESET_MASK) == RESET){ //reset the board
            display_fillScreen(DISPLAY_BLACK); //fill screen with black
            ticTacToeDisplay_drawBoardLines(); //redraw board lines
        }
        if((buttons_read() & TERMINATE_MASK) == TERMINATE){ //terminate the test
            break; //exit while loop to terminate test
        }//otherwise, we will continue with our test
        if(display_isTouched()){ //we've got some action going on
            display_clearOldTouchData(); //clear the old touch data
            utils_msDelay(WAIT_TIME);
            ticTacToeDisplay_touchScreenComputeBoardRowColumn(row, column); //computes the row and column of touch
            if((switches_read() & SWITCH_ON_MASK) == SWITCH_ON){ //write an O
                ticTacToeDisplay_drawO(*row, *column, DO_NOT_ERASE);
            }else{ //draw an X
                ticTacToeDisplay_drawX(*row, *column, DO_NOT_ERASE);
            }
        }
    }
    display_fillScreen(DISPLAY_BLACK); //fill screen with black
    printf("Test has terminated.\n\r"); //finishing the test for the tic tac toe display
}

// This will draw the four board lines.
void ticTacToeDisplay_drawBoardLines(){
    display_drawLine(FIRST_VERT_LINE, SIDE_OF_SCREEN, FIRST_VERT_LINE, DISPLAY_HEIGHT, DISPLAY_GREEN); //draw first vertical line
    display_drawLine(SECOND_VERT_LINE, SIDE_OF_SCREEN, SECOND_VERT_LINE, DISPLAY_HEIGHT, DISPLAY_GREEN); //draw second vertical line
    display_drawLine(SIDE_OF_SCREEN, FIRST_HOR_LINE, DISPLAY_WIDTH, FIRST_HOR_LINE, DISPLAY_GREEN); //draw first horizontal line
    display_drawLine(SIDE_OF_SCREEN, SECOND_HOR_LINE, DISPLAY_WIDTH, SECOND_HOR_LINE, DISPLAY_GREEN); //draw second horizontal line
}
