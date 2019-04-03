
#include "simonDisplay.h" //include display functions
#include "buttonHandler.h" //include button handler functions
#include <stdio.h>
#include <limits.h>
#include "supportFiles/display.h" //include display functions
#include "supportFiles/utils.h" //include util functions
#include "flashSequence.h" //include flashing functions
#include "verifySequence.h" //include verification functions
#include "simonControl.h" //include control functions
#include "../intervalTimerLab/intervalTimer.h" //include timers
#include "xparameters.h"
#include "supportFiles/globalTimer.h" //include global timer
#include "supportFiles/interrupts.h" //include interrupts

#define TIMER_PERIOD 120.0E-3 // You can change this value to a value that you select.
#define TIMER_CLOCK_FREQUENCY (XPAR_CPU_CORTEXA9_0_CPU_CLK_FREQ_HZ / 2)
#define TIMER_LOAD_VALUE ((TIMER_PERIOD * TIMER_CLOCK_FREQUENCY) - 1.0)

#define TOTAL_SECONDS 60 //seconds the test will run for before showing the number of missed interrupts
#define CLEAR 0 //value to clear

typedef void (functionPointer_t)();
#define MAX_DURATION_TIMER_INDEX 2
static double maxDuration_g;                    // The max duration across all ticks.
static const char* maxDurationFunctionName_g;   // Name of the function with longest duration is here.
#define SIMON_CONTROL_TICK "simonControl_tick()"
#define VERIFY_SEQUENCE_TICK "verifySequence_tick()"
#define FLASH_SEQUENCE_TICK "flashSequency_tick()"
#define BUTTON_HANDLER_TICK "buttonHandler_tick()"

// This assumes that the interval timer was initialized.
static void tickTimer(functionPointer_t* fp,
                      const char* functionName) {
    intervalTimer_reset(MAX_DURATION_TIMER_INDEX);  // Reset the timer.
    intervalTimer_start(MAX_DURATION_TIMER_INDEX);  // Start the timer.
    fp();                                           // Invoke the tick function passed in by argument.
    intervalTimer_stop(MAX_DURATION_TIMER_INDEX);   // Stop the timer.
    double duration;                                // Keep track of tick duration.
    duration = intervalTimer_getTotalDurationInSeconds(MAX_DURATION_TIMER_INDEX);  // Get the duration.
    if (maxDuration_g < duration) {                 // Keep the max duration.
        maxDuration_g = duration;
        maxDurationFunctionName_g = functionName;   // Keep track of which function had the max. duration.
    }
}

int main() {
    //init the display
    display_init();
    // Init all interrupts (but does not enable the interrupts at the devices).
    // Prints an error message if an internal failure occurs because the argument = true.
    interrupts_initAll(true);
    interrupts_setPrivateTimerLoadValue(TIMER_LOAD_VALUE);
    u32 privateTimerTicksPerSecond = interrupts_getPrivateTimerTicksPerSecond();
    printf("private timer ticks per second: %ld\n\r", privateTimerTicksPerSecond); //print ticks per second
    // Allow the timer to generate interrupts.
    interrupts_enableTimerGlobalInts();
    //INIT ALL STATE MACHINES
    buttonHandler_init(); //init buttons
    simonControl_init(); //init control
    flashSequence_init(); //init flasher
    verifySequence_init(); //init verifier
    // Keep track of your personal interrupt count. Want to make sure that you don't miss any interrupts.
    int32_t personalInterruptCount = CLEAR;
    // Start the private ARM timer running.
    interrupts_startArmPrivateTimer();
    // Enable interrupts at the ARM.
    interrupts_enableArmInts();
    intervalTimer_initAll(); //init the interval timers
    // interrupts_isrInvocationCount() returns the number of times that the timer ISR was invoked.
    // This value is maintained by the timer ISR. Compare this number with your own local
    // interrupt count to determine if you have missed any interrupts.
    while (interrupts_isrInvocationCount() < (TOTAL_SECONDS * privateTimerTicksPerSecond)) {
        if (interrupts_isrFlagGlobal) {  // This is a global flag that is set by the timer interrupt handler.
            // Count ticks.
            personalInterruptCount++;

            tickTimer(flashSequence_tick, FLASH_SEQUENCE_TICK);    // Tick a state machine.
            tickTimer(verifySequence_tick, VERIFY_SEQUENCE_TICK);  // Tick a state machine.
            tickTimer(buttonHandler_tick, BUTTON_HANDLER_TICK);    // Tick a state machine.
            tickTimer(simonControl_tick, SIMON_CONTROL_TICK);      // Tick a state machine.

            interrupts_isrFlagGlobal = CLEAR; //reset flag
        }
    }
    interrupts_disableArmInts(); //disable interrupts
    printf("isr invocation count: %ld\n\r", interrupts_isrInvocationCount()); //print out interrupts count
    printf("internal interrupt count: %ld\n\r", personalInterruptCount); //confirm with my counted interrupts
    return CLEAR; //return 0 to finish


}

// This function must be defined but can be left empty for now.
// You will use this function in a later lab.
// It is called in the timer interrupt service routine (see interrupts.c in supportFiles).
void isr_function() {
    // Empty for now.
}
