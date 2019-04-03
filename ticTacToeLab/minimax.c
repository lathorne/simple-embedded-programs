#include "minimax.h"
#include <stdio.h>
#include <string.h>

#define MIDDLE_ROW_OR_COLUMN 1 //middle column/row constant

#define GAME_IS_OVER true //game over
#define GAME_IS_NOT_OVER false //game not over

#define MIN -20 //minimum score
#define MAX 20 //maximum score

#define DEFAULT 3 //default value for choices
#define DEFAULT_ROW 2 //default row to play
#define DEFAULT_COL 2 //default column to play

//declare my own struct so it is signed numbers
typedef struct { //defining a board struct
    int8_t squares[MINIMAX_BOARD_ROWS][MINIMAX_BOARD_COLUMNS];  // State of game as passed in.
} score_board_t;

minimax_move_t c; //a typedef to set the column/row choice
minimax_move_t* choice = &c;

// Init the score board to all empty squares.
void minimax_initScoreBoard(score_board_t* board){ //init the scoreboard struct we are using
    for(uint8_t i = 0; i < MINIMAX_BOARD_ROWS; i++){ //go through rows
        for(uint8_t j = 0; j < MINIMAX_BOARD_COLUMNS; j++){ //go through columns
            board->squares[i][j] = MINIMAX_NOT_ENDGAME; //set as not an end game condition
        }
    }
}

minimax_score_t minimax(minimax_board_t* board, bool current_player_is_x){ //minimax function for scoreboard

    minimax_score_t currentScore = minimax_computeBoardScore(board, !current_player_is_x); //setting the current score for the board to check for end game

    if(minimax_isGameOver(currentScore)){ //checking for end game
        return currentScore; //return end game score
    }

    score_board_t not_pointer; //setting up our score board
    score_board_t *scores_board = &not_pointer;
    minimax_initScoreBoard(scores_board); //init scores board
    minimax_score_t score; //typedef for scoreboard

    for(uint8_t i = 0; i < MINIMAX_BOARD_ROWS; i++){ //go through rows
        for(uint8_t j = 0; j < MINIMAX_BOARD_COLUMNS; j++){ //go through columns
            if(board->squares[i][j] == MINIMAX_EMPTY_SQUARE){ //if the current square is empty
                if(current_player_is_x){ //assign the square an x because X is the current player
                    board->squares[i][j] = MINIMAX_X_SQUARE; //set X
                }else{ //assign the square an O because O is the current player
                    board->squares[i][j] = MINIMAX_O_SQUARE; //set O
                }
                score = minimax(board, !current_player_is_x); //pass in the adjusted board and the next player
                scores_board->squares[i][j] = score; //add this score
                board->squares[i][j] = MINIMAX_EMPTY_SQUARE; //set the square back to an empty square for next recursion
            }
        }
    }//end of for-loop, at this point we should have computed the entire move-scores table at this level

    if(current_player_is_x){ //check for current player
        for(uint8_t i = 0; i < MINIMAX_BOARD_ROWS; i++){ //go through rows
            for(uint8_t j = 0; j < MINIMAX_BOARD_COLUMNS; j++){ //go through columns
                if(scores_board->squares[i][j] != MINIMAX_NOT_ENDGAME && (scores_board->squares[i][j] >= score)){ //set this condition as best score
                    score = scores_board->squares[i][j]; //set score to higher score
                    choice->row = i; //set choice row
                    choice->column = j; //set choice column
                }
            }
        }
    }else{ //the current player is not x
        for(uint8_t i = 0; i < MINIMAX_BOARD_ROWS; i++){ //go through rows
            for(uint8_t j = 0; j < MINIMAX_BOARD_COLUMNS; j++){ //go through columns
                if(scores_board->squares[i][j] != MINIMAX_NOT_ENDGAME && (scores_board->squares[i][j] <= score)){ //set this score as best score
                    score = scores_board->squares[i][j]; //set score
                    choice->row = i; //set choice row
                    choice->column = j; //set choice column
                }
            }
        }
    }
    return score; //return max score
}

bool minimax_boardIsEmpty(minimax_board_t* board){
    bool empty = true; //assume it is empty
    for(uint8_t i = 0; i <= DEFAULT_ROW;  i++){ //go through rows
        for(uint8_t j = 0; j <= DEFAULT_COL;  j++){ //go through columns
            if(board->squares[i][j] != MINIMAX_EMPTY_SQUARE){ //if the board square isn't an empty, then the board isn't empty
                empty = false; //clearly not empty
                return empty;
            }
        }
    }
    return empty; //otherwise, board is empty
}

// This routine is not recursive but will invoke the recursive minimax function.
// You will call this function from the controlling state machine that you will implement in a later milestone.
// It computes the row and column of the next move based upon:
// the current board,
// the player. true means the computer is X. false means the computer is O.
// When called from the controlling state machine, you will call this function as follows:
// 1. If the computer is playing as X, you will call this function with current_player_is_x = true.
// 2. If the computer is playing as O, you will call this function with current_player_is_x = false.
// minimax_computeNextMove directly passes the current_player_is_x argument into the minimax() (helper) function.
// To assign values to the row and column arguments, you must use the following syntax in the body of the function:
// *row = move_row; *column = move_column; (for example).
void minimax_computeNextMove(minimax_board_t* board, bool current_player_is_x, uint8_t* row, uint8_t* column){
    if(minimax_boardIsEmpty(board)){ //check for an empty board
        *row = DEFAULT_ROW; //set row from choice
        *column = DEFAULT_COL; //set column from choice
        return; //if board is empty don't run full minimax
    }
    choice->row = DEFAULT; //set these values in the non-recursive minimax
    choice->column = DEFAULT;
    minimax(board, current_player_is_x); //call first minimax call
    *row = choice->row; //set row from choice
    *column = choice->column; //set column from choice
}

// Determine that the game is over by looking at the score.
bool minimax_isGameOver(minimax_score_t score){
    return score != MINIMAX_NOT_ENDGAME;
}

// Returns the score of the board, based upon the player and the board.
// This returns one of 4 values: MINIMAX_X_WINNING_SCORE,
// MINIMAX_O_WINNING_SCORE, MINIMAX_DRAW_SCORE, MINIMAX_NOT_ENDGAME
// Note: the player argument makes it possible to speed up this function.
// Assumptions:
// (1) if current_player_is_x == true, the last thing played was an 'X'.
// (2) if current_player_is_x == false, the last thing played was an 'O'.
// Hint: If you know the game was not over when an 'X' was played // you don't need to look for 'O's, and vice-versa.
minimax_score_t minimax_computeBoardScore(minimax_board_t* board, bool player_is_x){

    minimax_score_t checker = player_is_x ? MINIMAX_X_SQUARE : MINIMAX_O_SQUARE; //change what we are comparing our squares to based on who played last
    minimax_score_t win_return = player_is_x ? MINIMAX_X_WINNING_SCORE: MINIMAX_O_WINNING_SCORE; //set our return based on who could potentially win

    bool isEmpty = false; //we will assume there are no empties

    for(uint8_t i = 0; i < MINIMAX_BOARD_ROWS; i++){ //go through rows
        for(uint8_t j = 0; j < MINIMAX_BOARD_COLUMNS; j++){ //go through columns
            if(board->squares[i][j] == MINIMAX_EMPTY_SQUARE){
                isEmpty = true; //there is still an empty space
            }
            if(i == MIDDLE_ROW_OR_COLUMN){ //check the column
                if( (board->squares[i-1][j] == checker) &&
                    (board->squares[i][j] == checker) &&
                    (board->squares[i+1][j] == checker)){
                    return win_return; //there is a win
                }
            }if(j == MIDDLE_ROW_OR_COLUMN){ //check the row
                if( (board->squares[i][j-1] == checker) &&
                    (board->squares[i][j] == checker) &&
                    (board->squares[i][j+1] == checker)){
                    return win_return; //there is a win
                }
            }if(i == MIDDLE_ROW_OR_COLUMN && j == MIDDLE_ROW_OR_COLUMN){ //check the diagonal
                if( (board->squares[i][j] == checker) &&
                    (board->squares[i-1][j+1] == checker) &&
                    (board->squares[i+1][j-1] == checker)){
                    return win_return; //there is a win
                }
                if( (board->squares[i][j] == checker) && //check the other diagonal
                    (board->squares[i+1][j+1] == checker) &&
                    (board->squares[i-1][j-1] == checker)){
                    return win_return; //there is a win
                }
            }
        } //end for-loop
    } //end for-loop
    //if it has made it here, it is either a draw or the game goes on!
    if(isEmpty){
        return MINIMAX_NOT_ENDGAME; //there is an empty square so the game goes on
    }
    return MINIMAX_DRAW_SCORE; //no winner, no empty squares, the game is a draw
}

// Init the board to all empty squares.
void minimax_initBoard(minimax_board_t* board){
    for(uint8_t i = 0; i < MINIMAX_BOARD_ROWS; i++){ //go through rows
        for(uint8_t j = 0; j < MINIMAX_BOARD_COLUMNS; j++){ //go through columns
            board->squares[i][j] = MINIMAX_EMPTY_SQUARE; //set all squares to empty
        }
    }
}
