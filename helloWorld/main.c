//runTest function??

#define UPPER_CORNER 0 //upper corner is at 0, 0
#define TEXT_SIZE 2 //change this to adjust text size
#define SCREEN_SPLIT 2 //splitting the screen in half
#define TRIANGLE_SIDE 30 //the length of a side of a triangle
#define TRIANGLE_MEDIAN 80 //the length of the median of the triangle, vertically
// Print out "hello world" on both the console and the LCD screen.

#include <stdio.h>
#include <limits.h>
#include "supportFiles/display.h"

uint16_t func(uint16_t a, uint16_t b){
    printf("%d\n\r", a);

    printf("%d\n\r", b);

    return a * b;
}

int main() {
//	display_init();  // Must init all of the software and underlying hardware for LCD.
//	display_fillScreen(DISPLAY_BLACK);  // Blank the screen.
//	display_setCursor(UPPER_CORNER, UPPER_CORNER);  // The upper left of the LCD screen.
//	display_setTextColor(DISPLAY_RED);  // Make the text red.
//	display_setTextSize(TEXT_SIZE);   // Make the text a little larger.
//	//display_println("hello world (on the LCD)!");    // This actually prints the string to the LCD.
//	printf("hello world!\n\r");  // This prints on the console. When \n\r is removed it no longer prints on console
//	//lines - this will display the green lines running diagonally along the screen
//    display_drawLine(UPPER_CORNER, display_height(), display_width(), UPPER_CORNER, DISPLAY_GREEN);
//    display_drawLine(UPPER_CORNER, UPPER_CORNER, display_width(), display_height(), DISPLAY_GREEN);
//    //triangles - this displays the two yellow triangles
//    display_fillTriangle(((display_width()/SCREEN_SPLIT) - TRIANGLE_SIDE), ((display_height()/SCREEN_SPLIT) - TRIANGLE_MEDIAN), (display_width()/SCREEN_SPLIT), ((display_height()/SCREEN_SPLIT) - TRIANGLE_SIDE), ((display_width()/SCREEN_SPLIT) + TRIANGLE_SIDE), (display_height()/SCREEN_SPLIT) - TRIANGLE_MEDIAN, DISPLAY_YELLOW);
//    display_drawTriangle(((display_width()/SCREEN_SPLIT) - TRIANGLE_SIDE), ((display_height()/SCREEN_SPLIT) + TRIANGLE_MEDIAN), (display_width()/SCREEN_SPLIT), ((display_height()/SCREEN_SPLIT) + TRIANGLE_SIDE), ((display_width()/SCREEN_SPLIT) + TRIANGLE_SIDE), (display_height()/SCREEN_SPLIT) + TRIANGLE_MEDIAN, DISPLAY_YELLOW);
//    //circles - this displays the two orange circles
//    display_drawCircle(((display_width()/SCREEN_SPLIT) - TRIANGLE_MEDIAN), (display_height()/SCREEN_SPLIT), TRIANGLE_SIDE , DISPLAY_RED);
//    display_fillCircle(((display_width()/SCREEN_SPLIT) + TRIANGLE_MEDIAN), (display_height()/SCREEN_SPLIT), TRIANGLE_SIDE , DISPLAY_RED);

   uint16_t x = 14;
   uint16_t y = 0;
   y = func(++x, x++);
   printf("%d\n\r", y);

}

// This function must be defined but can be left empty for now.
// You will use this function in a later lab.
// It is called in the timer interrupt service routine (see interrupts.c in supportFiles).
void isr_function() {
    // Empty for now.
}
