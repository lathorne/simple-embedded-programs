/*
 * switchAndButtonMain.c
 *
 *  Created on: Mar 21, 2018
 *      Author: logan
 */

#include "switches.h"
#include "buttons.h"

int main() {
    switches_runTest(); //run test for switches
    buttons_runTest(); //run test for buttons
    //that's it!
}

void isr_function() {
    // Empty for now.
}




