/*
 * ticTacToeControl.c
 *
 *  Created on: Apr 10, 2018
 *      Author: logan
 */

//Logan Thorneloe

#include "ticTacToeControl.h" //include for control
#include "ticTacToeDisplay.h" //include to use ticTacToeDisplay
#include "minimax.h" //used for board
#include "supportFiles/display.h" //include for using display
#include <stdio.h> //standard printing
#include "../switchAndButtonLab/buttons.h" //allows me to use the buttons
#include "../intervalTimerLab/intervalTimer.h" //debug with interval timer

#define CLEAR 0x0 //CLEAR bits

//the period is 50 ms
//needs to portray the welcome screen for a few seconds
#define WELCOME_MAX 60
//needs to wait for 50 ms
#define ADC_MAX 1
//waits three seconds before autoplaying the computer
#define AUTO_MAX 40

// constants for the welcome screen display
#define TEXT_START 30 //x location of text start
#define FIRST_LINE 50 //first line location
#define SECOND_LINE FIRST_LINE + 30 //second line of text location
#define THIRD_LINE SECOND_LINE + 30 //third line of text location
#define FOURTH_LINE THIRD_LINE + 30 //fourth line of text location
#define TEXT_SIZE 2 //text size for welcome screen

#define RESET_MASK 1 //mask for resetting
#define RESET 1 //reset button value

#define NUM_ROWS 3 //number of rows
#define NUM_COLS 3 //number of columns

#define ERASE true //erase the currently played X's and O's

uint16_t welcomeTimer, autoTimer, adcTimer; //timers for the different states
uint8_t row, column; //row and column to play in
minimax_board_t board; //board to keep track of plays
bool isX; //lets us know if the current player is X or O
bool isValid; //checks the validity of a players choice

// States for the controller state machine.
enum ticTacToeControl_st_t {
    init_st, //init state the board
    welcome_st, //displays welcome message
    gameStart_st, //resets the board, starts the game
    adc_st, //waits for the adc to settle when player touches board
    checkPlay_st, //checks the current players move to make sure its valid
    setPlayerMove_st, //set the X or O for the player on the board, evaluates board
    playerWait_st, //waits for the player to play
    minimax_st, //computes best move for computer
    setCompMove_st, //set the X or O for the computer on the board
    win_st //game over, someone has won or the score is tied
} currState;

void debugStatePrint2() {
  static ticTacToeControl_st_t previousState; //keep track of current state
  static bool firstPass = true; //keep track of first pass through
  // Only print the message if:
  // 1. This the first pass and the value for previousState is unknown.
  // 2. previousState != currentState - this prevents reprinting the same state name over and over.
  if (previousState != currState || firstPass) {
    firstPass = false;                // previousState will be defined, firstPass is false.
    previousState = currState;     // keep track of the last state that you were in.
    switch(currState) {            // This prints messages based upon the state that you were in.
      case init_st:
        printf("init_st\n\r"); //print to show I am in init state
        break;
      case welcome_st:
        printf("welcome_st\n\r"); //print to show I am in welcome state
        break;
      case gameStart_st:
        printf("gameStart_st\n\r"); //print to show I am in game start state
        break;
      case adc_st:
        printf("adc_st\n\r"); //print to show I am in ad timer state
        break;
      case checkPlay_st:
        printf("checkPlay_st\n\r"); //print to show I am in checkPlay state
        break;
      case setPlayerMove_st:
        printf("setPlayerMove_st\n\r"); //print to show I am in setPlayerMove state
        break;
      case playerWait_st:
        printf("playerWait_st\n\r"); //print to show I am in playerWait state
        break;
      case minimax_st:
        printf("minimax_st\n\r"); //print to show I am in minimax state
        break;
      case setCompMove_st:
        printf("setCompMove_st\n\r"); //print to show I am in setcompMove state
        break;
      case win_st:
        printf("win_st\n\r"); //print to show I am in win state
        break;
     }
  }
}

//displays welcome message
void ticTacToeControl_displayWelcomeMessage(bool erase){
    if(!erase){ //we are drawing rather than erasing
        display_init(); //init screen
        display_fillScreen(DISPLAY_BLACK); //set screen to black
        display_setCursor(TEXT_START, FIRST_LINE); //set cursor to start writing text
        display_setTextColor(DISPLAY_WHITE); //set text color
        display_setTextSize(TEXT_SIZE); //set text size
        display_println("Welcome!"); //print welcome
        display_setCursor(TEXT_START, SECOND_LINE); //set new cursor
        display_println("Touch first to play X's"); //println
        display_setCursor(TEXT_START, THIRD_LINE); //set new cursor
        display_println("OR"); //println
        display_setCursor(TEXT_START, FOURTH_LINE); //set new cursor
        display_println("Wait to play O's"); //println
    }else{ //this will erase the welcome message
        display_setCursor(TEXT_START, FIRST_LINE); //set cursor to start writing text
        display_setTextColor(DISPLAY_BLACK); //set text color
        display_setTextSize(TEXT_SIZE); //set text size
        display_println("Welcome!"); //print welcome
        display_setCursor(TEXT_START, SECOND_LINE); //set new cursor
        display_println("Touch first to play X's"); //println
        display_setCursor(TEXT_START, THIRD_LINE); //set new cursor
        display_println("OR"); //println
        display_setCursor(TEXT_START, FOURTH_LINE); //set new cursor
        display_println("Wait to play O's"); //println
    }

}

void ticTacToeControl_reset(){ //resets the ticTacToe board
    autoTimer = CLEAR; //clear the auto timer
    isX = true; //whoever plays next is X
    for(uint8_t i = 0; i < NUM_ROWS; i++){ //goes through rows and columns to write over X or O
        for(uint8_t j = 0; j < NUM_COLS; j++){ //this show be more efficient that the other way I was doing it
            if(board.squares[i][j] == MINIMAX_X_SQUARE){
                ticTacToeDisplay_drawX(i, j, ERASE); //erase an X
            }else if(board.squares[i][j] == MINIMAX_O_SQUARE){
                ticTacToeDisplay_drawO(i, j, ERASE); //erase an O
            }
        }
    }
    minimax_initBoard(&board); //init the board for the reset
}

void ticTacToeControl_addPlayToCurrentBoard(){
    if(isX){ //if the current player is an x
       board.squares[row][column] = MINIMAX_X_SQUARE; //add an x to the board
    }else{ //if O
       board.squares[row][column] = MINIMAX_O_SQUARE; //add an O to the board
    }
}

void ticTacToeControl_checkIsValid(){
    if(board.squares[row][column] != MINIMAX_EMPTY_SQUARE){ //checks to see if there is something in the square, if so, the move is invalid
        isValid = false; //invalid play
    }else{ //otherwise, valid play, set isValid to true and mark the square as played on the board
        isValid = true; //valid play
        ticTacToeControl_addPlayToCurrentBoard(); //adds current play to board
    }
}

void ticTacToeControl_checkForReset(){ //check if the reset button is being pressed, this is called in every state
    if((buttons_read() & RESET_MASK) == RESET){ //reset the board
        currState = gameStart_st; //go back to the gamestart_st
        ticTacToeControl_reset(); //reset the control
    }
}

// Standard tick function.
void ticTacToeControl_tick(){
    // Perform state update first.
      switch(currState) { //switch statement to control
          case(init_st): //in init state
                  currState = welcome_st; //move to the welcome state
                  welcomeTimer = CLEAR; //reset welcome timer
                  break;
          case(welcome_st): //in welcome state
                  if(welcomeTimer >= WELCOME_MAX){ //when time is up in the welcome state
                      currState = gameStart_st; //go to game start state
                      ticTacToeControl_displayWelcomeMessage(ERASE); //get rid of the welcome text
                      ticTacToeDisplay_drawBoardLines(); //draw board lines
                      isX = true; //the first player is playing X's
                  }
                  break;
          case(gameStart_st): //in game start st
                  ticTacToeControl_checkForReset(); //check for a reset
                  if(autoTimer >= AUTO_MAX){ //when its time for the computer to play
                      currState = minimax_st; //go to minimax state
                      minimax_computeNextMove(&board, isX, &row, &column); //this will set row and column
                  }
                  if(display_isTouched()){ //this means its the players turn
                      currState = adc_st; //go to adc settle st
                      adcTimer = CLEAR; //reset adc timer
                      display_clearOldTouchData(); //clear old touch data
                  }
                  break;
          case(adc_st): //in adc settle state
                  ticTacToeControl_checkForReset(); //check for a reset
                  if(adcTimer >= ADC_MAX){ //might need to check for the display to still be touched here
                      currState = checkPlay_st; //go to check play state
                      ticTacToeDisplay_touchScreenComputeBoardRowColumn(&row, &column); //computes where the player touched
                      ticTacToeControl_checkIsValid(); //checks to see if it is valid, and stores a valid play on the board
                  }
                  break;
          case(checkPlay_st):
                  ticTacToeControl_checkForReset(); //check for reset
                  if(isValid){ //valid play
                      currState = setPlayerMove_st; //move on to set X or O on screen
                      if(isX){ //if we are playing X's
                          ticTacToeDisplay_drawX(row, column, false); //draws the players move as an X
                      }else{
                          ticTacToeDisplay_drawO(row, column, false); //draws the players move as an O
                      }
                      if(minimax_isGameOver(minimax_computeBoardScore(&board, isX))){ //is the game over??
                          currState = win_st; //go to win st
                      }
                  }else{ //invalid play
                      currState = playerWait_st; //way for other output from player

                  }
                  break;
          case(setPlayerMove_st):
                    ticTacToeControl_checkForReset(); //check for reset
                    isX = !isX; //change to the other player token
                    currState = minimax_st; //computers turn
                    minimax_computeNextMove(&board, isX, &row, &column); //this will set row and column for computer play
                  break;
          case(playerWait_st):
                  ticTacToeControl_checkForReset(); //check for reset
                  if(display_isTouched()){ //wait for another touch from the player, otherwise continue waiting
                      currState = adc_st; //go to adc settle state
                      adcTimer = CLEAR; //reset the ADC time
                      display_clearOldTouchData(); //clear out the old touch data
                  }
                  break;
          case(minimax_st): //in minimax st
                  ticTacToeControl_checkForReset(); //check for reset
                  currState = setCompMove_st; //enter setCompMove st
                  if(isX){ //check if the player is playing X or O
                      ticTacToeDisplay_drawX(row, column, false); //draws the players move as an X
                  }else{
                      ticTacToeDisplay_drawO(row, column, false); //draws the players move as an O
                  }
                  ticTacToeControl_addPlayToCurrentBoard(); //adds computer play to board
                  break;
          case(setCompMove_st): //state to set computer move
                  ticTacToeControl_checkForReset(); //check board for reset
                  if(minimax_isGameOver(minimax_computeBoardScore(&board, isX))){ //is the game over??
                      currState = win_st; //go to win state
                  }else{
                      isX = !isX; //change to the other player token
                      currState = playerWait_st; //players turn
                  }
                  break;
          case(win_st): //the game has come to an end
                  ticTacToeControl_checkForReset(); //check for board reset
                  break;
          default: //standard default case
              break;

      }
      // Perform state action next.
      switch(currState) { //switch statement to perform state actions
          case(welcome_st):
                  welcomeTimer++; //increment welcome timer
                  break;
          case(gameStart_st):
                  autoTimer++; //increase the autoplay timer
                  break;
          case(adc_st):
                  adcTimer++; //increment the adc timer
                  break;
          default: //standard default case
                  break;
      }
}

// Call this before you call clockControl_tick().
void ticTacToeControl_init(){
    ticTacToeControl_displayWelcomeMessage(!ERASE); //display welcome message outside of state machine
    currState = init_st; //sets enum value
}
