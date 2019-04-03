#include "minimax.h"
#include <stdio.h>
#include "../intervalTimerLab/intervalTimer.h"

void printBoard(minimax_board_t board){
    printf("%d %d %d\n", board.squares[0][0], board.squares[0][1], board.squares[0][2]);
    printf("%d %d %d\n", board.squares[1][0], board.squares[1][1], board.squares[1][2]);
    printf("%d %d %d\n", board.squares[2][0], board.squares[2][1], board.squares[2][2]);
}

int main() {

    //TEST CASES
  minimax_board_t board1;  // Board 1 is the main example in the web-tutorial that I use on the web-site.
  board1.squares[0][0] = MINIMAX_O_SQUARE;
  board1.squares[0][1] = MINIMAX_EMPTY_SQUARE;
  board1.squares[0][2] = MINIMAX_X_SQUARE;
  board1.squares[1][0] = MINIMAX_X_SQUARE;
  board1.squares[1][1] = MINIMAX_EMPTY_SQUARE;
  board1.squares[1][2] = MINIMAX_EMPTY_SQUARE;
  board1.squares[2][0] = MINIMAX_X_SQUARE;
  board1.squares[2][1] = MINIMAX_O_SQUARE;
  board1.squares[2][2] = MINIMAX_O_SQUARE;

  minimax_board_t board2;
  board2.squares[0][0] = MINIMAX_O_SQUARE;
  board2.squares[0][1] = MINIMAX_EMPTY_SQUARE;
  board2.squares[0][2] = MINIMAX_X_SQUARE;
  board2.squares[1][0] = MINIMAX_EMPTY_SQUARE;
  board2.squares[1][1] = MINIMAX_EMPTY_SQUARE;
  board2.squares[1][2] = MINIMAX_EMPTY_SQUARE;
  board2.squares[2][0] = MINIMAX_X_SQUARE;
  board2.squares[2][1] = MINIMAX_EMPTY_SQUARE;
  board2.squares[2][2] = MINIMAX_O_SQUARE;

  minimax_board_t board3;
  board3.squares[0][0] = MINIMAX_O_SQUARE;
  board3.squares[0][1] = MINIMAX_EMPTY_SQUARE;
  board3.squares[0][2] = MINIMAX_EMPTY_SQUARE;
  board3.squares[1][0] = MINIMAX_O_SQUARE;
  board3.squares[1][1] = MINIMAX_EMPTY_SQUARE;
  board3.squares[1][2] = MINIMAX_EMPTY_SQUARE;
  board3.squares[2][0] = MINIMAX_X_SQUARE;
  board3.squares[2][1] = MINIMAX_EMPTY_SQUARE;
  board3.squares[2][2] = MINIMAX_X_SQUARE;

  minimax_board_t board4;
  board4.squares[0][0] = MINIMAX_O_SQUARE;
  board4.squares[0][1] = MINIMAX_EMPTY_SQUARE;
  board4.squares[0][2] = MINIMAX_EMPTY_SQUARE;
  board4.squares[1][0] = MINIMAX_EMPTY_SQUARE;
  board4.squares[1][1] = MINIMAX_EMPTY_SQUARE;
  board4.squares[1][2] = MINIMAX_EMPTY_SQUARE;
  board4.squares[2][0] = MINIMAX_X_SQUARE;
  board4.squares[2][1] = MINIMAX_EMPTY_SQUARE;
  board4.squares[2][2] = MINIMAX_X_SQUARE;

  minimax_board_t board5;
  board5.squares[0][0] = MINIMAX_X_SQUARE;
  board5.squares[0][1] = MINIMAX_X_SQUARE;
  board5.squares[0][2] = MINIMAX_EMPTY_SQUARE;
  board5.squares[1][0] = MINIMAX_EMPTY_SQUARE;
  board5.squares[1][1] = MINIMAX_O_SQUARE;
  board5.squares[1][2] = MINIMAX_EMPTY_SQUARE;
  board5.squares[2][0] = MINIMAX_EMPTY_SQUARE;
  board5.squares[2][1] = MINIMAX_EMPTY_SQUARE;
  board5.squares[2][2] = MINIMAX_EMPTY_SQUARE;

  //end of test cases from wiki
  //start of X-first test cases

  minimax_board_t board6;  //tests empty board
  board6.squares[0][0] = MINIMAX_EMPTY_SQUARE;
  board6.squares[0][1] = MINIMAX_EMPTY_SQUARE;
  board6.squares[0][2] = MINIMAX_EMPTY_SQUARE;
  board6.squares[1][0] = MINIMAX_EMPTY_SQUARE;
  board6.squares[1][1] = MINIMAX_EMPTY_SQUARE;
  board6.squares[1][2] = MINIMAX_EMPTY_SQUARE;
  board6.squares[2][0] = MINIMAX_EMPTY_SQUARE;
  board6.squares[2][1] = MINIMAX_EMPTY_SQUARE;
  board6.squares[2][2] = MINIMAX_EMPTY_SQUARE;

  minimax_board_t board7; //tests placement into any open spot
  board7.squares[0][0] = MINIMAX_X_SQUARE;
  board7.squares[0][1] = MINIMAX_EMPTY_SQUARE;
  board7.squares[0][2] = MINIMAX_O_SQUARE;
  board7.squares[1][0] = MINIMAX_O_SQUARE;
  board7.squares[1][1] = MINIMAX_O_SQUARE;
  board7.squares[1][2] = MINIMAX_EMPTY_SQUARE;
  board7.squares[2][0] = MINIMAX_EMPTY_SQUARE;
  board7.squares[2][1] = MINIMAX_O_SQUARE;
  board7.squares[2][2] = MINIMAX_EMPTY_SQUARE;

  minimax_board_t board8; //tests automatic winning
  board8.squares[0][0] = MINIMAX_O_SQUARE;
  board8.squares[0][1] = MINIMAX_EMPTY_SQUARE;
  board8.squares[0][2] = MINIMAX_EMPTY_SQUARE;
  board8.squares[1][0] = MINIMAX_X_SQUARE;
  board8.squares[1][1] = MINIMAX_X_SQUARE;
  board8.squares[1][2] = MINIMAX_EMPTY_SQUARE;
  board8.squares[2][0] = MINIMAX_X_SQUARE;
  board8.squares[2][1] = MINIMAX_EMPTY_SQUARE;
  board8.squares[2][2] = MINIMAX_O_SQUARE;

  minimax_board_t board9; //tests going in any open spot
  board9.squares[0][0] = MINIMAX_X_SQUARE;
  board9.squares[0][1] = MINIMAX_EMPTY_SQUARE;
  board9.squares[0][2] = MINIMAX_O_SQUARE;
  board9.squares[1][0] = MINIMAX_EMPTY_SQUARE;
  board9.squares[1][1] = MINIMAX_X_SQUARE;
  board9.squares[1][2] = MINIMAX_EMPTY_SQUARE;
  board9.squares[2][0] = MINIMAX_O_SQUARE;
  board9.squares[2][1] = MINIMAX_EMPTY_SQUARE;
  board9.squares[2][2] = MINIMAX_O_SQUARE;

  minimax_board_t board10; //winning in two moves
  board10.squares[0][0] = MINIMAX_O_SQUARE;
  board10.squares[0][1] = MINIMAX_EMPTY_SQUARE;
  board10.squares[0][2] = MINIMAX_EMPTY_SQUARE;
  board10.squares[1][0] = MINIMAX_EMPTY_SQUARE;
  board10.squares[1][1] = MINIMAX_EMPTY_SQUARE;
  board10.squares[1][2] = MINIMAX_EMPTY_SQUARE;
  board10.squares[2][0] = MINIMAX_X_SQUARE;
  board10.squares[2][1] = MINIMAX_EMPTY_SQUARE;
  board10.squares[2][2] = MINIMAX_EMPTY_SQUARE;

  //end of x test cases
  //start of O test cases

  minimax_board_t board11; //winning in two moves
  board11.squares[0][0] = MINIMAX_X_SQUARE;
  board11.squares[0][1] = MINIMAX_EMPTY_SQUARE;
  board11.squares[0][2] = MINIMAX_EMPTY_SQUARE;
  board11.squares[1][0] = MINIMAX_EMPTY_SQUARE;
  board11.squares[1][1] = MINIMAX_O_SQUARE;
  board11.squares[1][2] = MINIMAX_EMPTY_SQUARE;
  board11.squares[2][0] = MINIMAX_EMPTY_SQUARE;
  board11.squares[2][1] = MINIMAX_EMPTY_SQUARE;
  board11.squares[2][2] = MINIMAX_X_SQUARE;

  minimax_board_t board12; //winning in two moves
  board12.squares[0][0] = MINIMAX_O_SQUARE;
  board12.squares[0][1] = MINIMAX_EMPTY_SQUARE;
  board12.squares[0][2] = MINIMAX_EMPTY_SQUARE;
  board12.squares[1][0] = MINIMAX_EMPTY_SQUARE;
  board12.squares[1][1] = MINIMAX_X_SQUARE;
  board12.squares[1][2] = MINIMAX_X_SQUARE;
  board12.squares[2][0] = MINIMAX_EMPTY_SQUARE;
  board12.squares[2][1] = MINIMAX_EMPTY_SQUARE;
  board12.squares[2][2] = MINIMAX_O_SQUARE;

  minimax_board_t board13; //winning in two moves
  board13.squares[0][0] = MINIMAX_EMPTY_SQUARE;
  board13.squares[0][1] = MINIMAX_EMPTY_SQUARE;
  board13.squares[0][2] = MINIMAX_X_SQUARE;
  board13.squares[1][0] = MINIMAX_EMPTY_SQUARE;
  board13.squares[1][1] = MINIMAX_X_SQUARE;
  board13.squares[1][2] = MINIMAX_O_SQUARE;
  board13.squares[2][0] = MINIMAX_EMPTY_SQUARE;
  board13.squares[2][1] = MINIMAX_X_SQUARE;
  board13.squares[2][2] = MINIMAX_O_SQUARE;

  minimax_board_t board14; //winning in two moves
  board14.squares[0][0] = MINIMAX_X_SQUARE;
  board14.squares[0][1] = MINIMAX_EMPTY_SQUARE;
  board14.squares[0][2] = MINIMAX_EMPTY_SQUARE;
  board14.squares[1][0] = MINIMAX_EMPTY_SQUARE;
  board14.squares[1][1] = MINIMAX_EMPTY_SQUARE;
  board14.squares[1][2] = MINIMAX_EMPTY_SQUARE;
  board14.squares[2][0] = MINIMAX_EMPTY_SQUARE;
  board14.squares[2][1] = MINIMAX_EMPTY_SQUARE;
  board14.squares[2][2] = MINIMAX_EMPTY_SQUARE;

  minimax_board_t board15; //winning in two moves
  board15.squares[0][0] = MINIMAX_X_SQUARE;
  board15.squares[0][1] = MINIMAX_O_SQUARE;
  board15.squares[0][2] = MINIMAX_X_SQUARE;
  board15.squares[1][0] = MINIMAX_X_SQUARE;
  board15.squares[1][1] = MINIMAX_O_SQUARE;
  board15.squares[1][2] = MINIMAX_EMPTY_SQUARE;
  board15.squares[2][0] = MINIMAX_EMPTY_SQUARE;
  board15.squares[2][1] = MINIMAX_EMPTY_SQUARE;
  board15.squares[2][2] = MINIMAX_EMPTY_SQUARE;

 uint8_t row, column;

 minimax_computeNextMove(&board1, true, &row, &column);      // true means X is current player.
 printBoard(board1);
 printf("next move for board1: (%d, %d)\n\r", row, column);
 minimax_computeNextMove(&board2, true, &row, &column);      // true means X is current player.
 printBoard(board2);
 printf("next move for board2: (%d, %d)\n\r", row, column);
 minimax_computeNextMove(&board3, true, &row, &column);      // true means X is current player.
 printBoard(board3);
 printf("next move for board3: (%d, %d)\n\r", row, column);
 minimax_computeNextMove(&board4, false, &row, &column);     // false means O is current player.
 printBoard(board4);
 printf("next move for board4: (%d, %d)\n\r", row, column);
 minimax_computeNextMove(&board5, false, &row, &column);     // false means O is current play5r.
 printBoard(board5);
 printf("next move for board5: (%d, %d)\n\r", row, column);

 minimax_computeNextMove(&board6, true, &row, &column);      // true means X is current player.
 printBoard(board6);
 printf("next move for board6: (%d, %d)\n\r", row, column);
 minimax_computeNextMove(&board7, true, &row, &column);      // true means X is current player.
 printBoard(board7);
 printf("next move for board7: (%d, %d)\n\r", row, column);
 minimax_computeNextMove(&board8, true, &row, &column);      // true means X is current player.
 printBoard(board8);
 printf("next move for board8: (%d, %d)\n\r", row, column);
 minimax_computeNextMove(&board9, true, &row, &column);     // false means O is current player.
 printBoard(board9);
 printf("next move for board9: (%d, %d)\n\r", row, column);
 minimax_computeNextMove(&board10, true, &row, &column);     // false means O is current play5r.
 printBoard(board10);
 printf("next move for board10: (%d, %d)\n\r", row, column);

 minimax_computeNextMove(&board11, false, &row, &column);     // false means O is current play5r.
 printBoard(board11);
 printf("next move for board11: (%d, %d)\n\r", row, column);
 minimax_computeNextMove(&board12, false, &row, &column);     // false means O is current play5r.
 printBoard(board12);
 printf("next move for board12: (%d, %d)\n\r", row, column);
 minimax_computeNextMove(&board13, false, &row, &column);     // false means O is current play5r.
 printBoard(board13);
 printf("next move for board13: (%d, %d)\n\r", row, column);
 minimax_computeNextMove(&board14, false, &row, &column);     // false means O is current play5r.
 printBoard(board14);
 printf("next move for board14: (%d, %d)\n\r", row, column);
 minimax_computeNextMove(&board15, false, &row, &column);     // false means O is current play5r.
 printBoard(board15);
 printf("next move for board15: (%d, %d)\n\r", row, column);
}


// This function must be defined but can be left empty for now.
// You will use this function in a later lab.
// It is called in the timer interrupt service routine (see interrupts.c in supportFiles).
void isr_function() {
    // Empty for now.
}
