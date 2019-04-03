/*
 * intervalTimer.c
 *
 *  Created on: Apr 1, 2018
 *      Author: logan
 */

//Logan Thorneloe
//ECEn 330 Lab 3

#include "intervalTimer.h"
#include "xparameters.h"
#include "xil_io.h"

//defining all base addresses
#define TIMER_0_BASE_ADDRESS XPAR_AXI_TIMER_0_BASEADDR //base address of timer 0
#define TIMER_1_BASE_ADDRESS XPAR_AXI_TIMER_1_BASEADDR //base address of timer 1
#define TIMER_2_BASE_ADDRESS XPAR_AXI_TIMER_2_BASEADDR //base address of timer 2
#define TIMER_FREQUENCY XPAR_AXI_TIMER_0_CLOCK_FREQ_HZ //timer frequency

//defining the individual bits that need to be set
#define LOAD_CASC_BIT 0x800 //CASC bit mask
#define LOAD_ENT_BIT 0x80 //ENT bit to 1 mask
#define LOAD_ENT_BIT_0 0xFFFFFF7F //ENT bit to 0 mask
#define LOAD_LOAD_BIT 0x20 //LOAD bit mask
#define CLEAR 0x0 //shows timer is clear
#define SHIFT_LEFT 0x20 //shifts the upper bits the appropriate amount

//simplifying timer names from intervalTimer.h
#define TIMER_2 INTERVAL_TIMER_TIMER_2 //simplified timer 2 name
#define TIMER_1 INTERVAL_TIMER_TIMER_1 //simplified timer 1 name
#define TIMER_0 INTERVAL_TIMER_TIMER_0 //simplified timer 0 name

//make #define using the offsets on page 11 for registers 0 and 1
#define TCSR0_OFFSET 0x00 //TCSR0 offset
#define TLR0_OFFSET 0x04 //TLR0 offset
#define TCR0_OFFSET 0x08 //TCR0 offset
#define TCSR1_OFFSET 0x10 //TCSR1 offset
#define TLR1_OFFSET 0x14 //TLR1 offset
#define TCR1_OFFSET 0x18 //TCR1 offset

#define MAX_COUNT 0xFFFFFFFFFFFFFFFF //max value for a 64-bit counter

//returns the base address based of the counter number passed in
uint32_t intervalTimer_getBaseAddress(uint32_t timerNumber){
    switch(timerNumber){ //switch statement to determine proper base address for each timer
    case TIMER_0: //working with Timer 0
        return TIMER_0_BASE_ADDRESS; //return base address of timer 0
        break;
    case TIMER_1: //working with Timer 1
        return TIMER_1_BASE_ADDRESS; //return base address of timer 1
        break;
    case TIMER_2: //working with Timer 2
        return TIMER_2_BASE_ADDRESS; //return base address of timer 2
        break;
    default: //requesting a base address for a non-existent timer
        return CLEAR; //clear the returned base address
    }
}

//********************************HELPER FUNCTIONS TO READ AND WRITE FROM GPIO REGISTERS***************************
void intervalTimer_writeGpioRegister(uint32_t address, int32_t offset, int32_t value){
   Xil_Out32(address + offset, value); // write to the address and the offset and put the value into it
}

int32_t intervalTimer_readGpioRegister(uint32_t address, int32_t offset) {
//   Just use the low-level Xilinx call.
   return Xil_In32(address + offset); //lets us read the GPIO registers in C
}
//********************************END OF HELPER FUNCTIONS TO READ AND WRITE****************************************


// You must initialize the timers before you use them the first time.
// It is generally only called once but should not cause an error if it
// is called multiple times.
// timerNumber indicates which timer should be initialized.
// returns INTERVAL_TIMER_STATUS_OK if successful, some other value otherwise.
intervalTimer_status_t intervalTimer_init(uint32_t timerNumber){
    uint32_t baseAddress = intervalTimer_getBaseAddress(timerNumber); //grab base address of timer we're working with
    //check if the timer number is 0, 1, 2 and if it is init that timer doing the three steps below and return a status okay, else return bad status
    if(baseAddress == CLEAR){ //check validity of timer request
        return INTERVAL_TIMER_STATUS_FAIL;
    } //this means it is a valid timerNumber
    intervalTimer_writeGpioRegister(baseAddress, TCSR0_OFFSET, CLEAR); //    write a 0 to the TCSR0 register, also clears the UDT0 bit.
    intervalTimer_writeGpioRegister(baseAddress, TCSR1_OFFSET, CLEAR); //    write a 0 to the TCSR1 register.
    intervalTimer_writeGpioRegister(baseAddress, TCSR0_OFFSET, LOAD_CASC_BIT); //put that CascBit (0x1) into TCSR0
    return INTERVAL_TIMER_STATUS_OK; //return properly working status
}

// This is a convenience function that initializes all interval timers.
// Simply calls intervalTimer_init() on all timers.
// returns INTERVAL_TIMER_STATUS_OK if successful, some other value otherwise.
intervalTimer_status_t intervalTimer_initAll(){
    //call init on all timers
    if(intervalTimer_init(TIMER_0) && intervalTimer_init(TIMER_1) && intervalTimer_init(TIMER_2)){ //ALL timers return OK status
        return INTERVAL_TIMER_STATUS_OK; //return OK for all timers
    }
    return INTERVAL_TIMER_STATUS_FAIL; //otherwise, return failure
}

// This function starts the interval timer running.
// If the interval timer is already running, this function does nothing.
// timerNumber indicates which timer should start running.
void intervalTimer_start(uint32_t timerNumber){
    uint32_t baseAddress = intervalTimer_getBaseAddress(timerNumber); //grab base address of timer we're working with
    uint32_t setEntBitOne = (intervalTimer_readGpioRegister(baseAddress, TCSR0_OFFSET) | LOAD_ENT_BIT); //bitwise or TCSR0 with 0x80 to add a 1 where the ENT0 bit is
    intervalTimer_writeGpioRegister(baseAddress, TCSR0_OFFSET, setEntBitOne); //put that EntBit (0x1) into TCSR0 to start the timer
}

// This function stops a running interval timer.
// If the interval time is currently stopped, this function does nothing.
// timerNumber indicates which timer should stop running.
void intervalTimer_stop(uint32_t timerNumber){
    uint32_t baseAddress = intervalTimer_getBaseAddress(timerNumber); //grab base address of timer we're working with
    uint32_t setEntBitZero = (intervalTimer_readGpioRegister(baseAddress, TCSR0_OFFSET) & LOAD_ENT_BIT_0); //bitwise ands ~0x80 from TCSR0 to get rid of the one where the ENT0 bit is
    intervalTimer_writeGpioRegister(baseAddress, TCSR0_OFFSET, setEntBitZero); //put that EntBit (0x0) into TCSR0 to start the timer
}

//helper function for intervalTimer_reset: loads zeroes into TLR0 and TLR1 and puts them into the counter (writes 1 to LOAD0 in TCSR0) //do we ignore TCSR1?? maybe repeat this for TCSR1
void intervalTimer_loadcounter(uint32_t baseAddress){
    intervalTimer_writeGpioRegister(baseAddress, TLR0_OFFSET, CLEAR); //clears the two load registers, TLR0
    intervalTimer_writeGpioRegister(baseAddress, TLR1_OFFSET, CLEAR); //and TLR1
    uint32_t setLoadBit = (intervalTimer_readGpioRegister(baseAddress, TCSR0_OFFSET) | LOAD_LOAD_BIT); //sets a 1 to LOAD0 in the TCSR0 value to load the contents of TLR0 and TLR1 into the counter
    intervalTimer_writeGpioRegister(baseAddress, TCSR0_OFFSET, setLoadBit); //actually writes the load bit into the TSCR0 register
    uint32_t setLoadBit_2 = (intervalTimer_readGpioRegister(baseAddress, TCSR1_OFFSET) | LOAD_LOAD_BIT); //sets a 1 to LOAD0 in the TCSR1 value
    intervalTimer_writeGpioRegister(baseAddress, TCSR1_OFFSET, setLoadBit_2);  //writes load bit into TSCR1 register
}

// This function is called whenever you want to reuse an interval timer.
// For example, say the interval timer has been used in the past, the user
// will call intervalTimer_reset() prior to calling intervalTimer_start().
// timerNumber indicates which timer should reset.
void intervalTimer_reset(uint32_t timerNumber){
    uint32_t baseAddress = intervalTimer_getBaseAddress(timerNumber); //grab base address of timer we're working with
    intervalTimer_loadcounter(baseAddress); //throw some zeroes in TLR0 and TLR1 and then writes them in the counter
    intervalTimer_init(timerNumber); //reinitializes the timer, should stop the counter from loading TLR0 and TLR1 values by setting LOAD0 in TSCR0 to 0
}

// Convenience function for intervalTimer_reset().
// Simply calls intervalTimer_reset() on all timers.
void intervalTimer_resetAll(){
    //call reset on all three timers
    intervalTimer_reset(TIMER_0); //reset timer 0
    intervalTimer_reset(TIMER_1); //reset timer 1
    intervalTimer_reset(TIMER_2); //reset timer 2
}

//helper function for test: tests restart portion of timer
intervalTimer_status_t intervalTimer_testRestart(uint32_t timerNumber){
    intervalTimer_reset(timerNumber); //call reset function
    if(intervalTimer_getTotalDurationInSeconds(timerNumber) == CLEAR){ //means the seconds that have passed is zero, so it is properly reset
        return INTERVAL_TIMER_STATUS_OK; //return good
    } //otherwise, return bad
    return INTERVAL_TIMER_STATUS_FAIL;
}

//helper function for test: tests start portion of timer
intervalTimer_status_t intervalTimer_testStart(uint32_t timerNumber){
    intervalTimer_start(timerNumber); //start the timer
    uint32_t timeInterval1 = intervalTimer_getTotalDurationInSeconds(timerNumber); //gets the first time interval
    uint32_t timeInterval2 = intervalTimer_getTotalDurationInSeconds(timerNumber); //gets the second time interval
    uint32_t timeInterval3 = intervalTimer_getTotalDurationInSeconds(timerNumber); //gets the third time interval
    if(timeInterval1 == timeInterval3 && timeInterval2 == timeInterval3){ //this means the timer did not increment when it should
        return INTERVAL_TIMER_STATUS_FAIL; //return failure to start
    } //otherwise, it is incrementing
    return INTERVAL_TIMER_STATUS_OK; //return good
}

//helper function for test: tests stopping the timer
intervalTimer_status_t intervalTimer_testStop(uint32_t timerNumber){
    intervalTimer_stop(timerNumber); //start the timer
    uint32_t timeInterval1 = intervalTimer_getTotalDurationInSeconds(timerNumber); //gets the first time interval
    uint32_t timeInterval2 = intervalTimer_getTotalDurationInSeconds(timerNumber); //gets the second time interval
    uint32_t timeInterval3 = intervalTimer_getTotalDurationInSeconds(timerNumber); //gets the third time interval
    if(timeInterval1 == timeInterval3 && timeInterval2 == timeInterval3){ //this means the timer did not increment, which is good
        return INTERVAL_TIMER_STATUS_OK; //return success of stop
    } //otherwise, it is incrementing
    return INTERVAL_TIMER_STATUS_FAIL; //return failure to stop
}

// Runs a test on a single timer as indicated by the timerNumber argument.
// Returns INTERVAL_TIMER_STATUS_OK if successful, something else otherwise.
intervalTimer_status_t intervalTimer_test(uint32_t timerNumber){
    //test the restart and then test the start for the timer number (when testing start, check values and see if they increment)
//    I start the counter and read it a couple of times to see if it is actually changing value.
//    I stop the counter and read it a couple of times to see that it is not changing.
    if(intervalTimer_testRestart(timerNumber) && intervalTimer_testStart(timerNumber) && intervalTimer_testStop(timerNumber)){ //all timer restart tests work
        return INTERVAL_TIMER_STATUS_OK; //returns OK status if all tests worked
    }
    return INTERVAL_TIMER_STATUS_FAIL; //otherwise, it failed
}

// Convenience function that invokes test on all interval timers.
// Returns INTERVAL_TIMER_STATUS_OK if successful, something else otherwise.
intervalTimer_status_t intervalTimer_testAll(){
    //call the test function on all timers and return OK if they work
    if(intervalTimer_test(TIMER_0) && intervalTimer_test(TIMER_1) && intervalTimer_test(TIMER_2)){ //if all timer tests are successful
        return INTERVAL_TIMER_STATUS_OK; //return OK status
    } //otherwise, return failure
    return INTERVAL_TIMER_STATUS_FAIL;
}

// Use this function to ascertain how long a given timer has been running.
// Note that it should not be an error to call this function on a running timer
// though it usually makes more sense to call this after intervalTimer_stop() has
// been called.
// The timerNumber argument determines which timer is read.
double intervalTimer_getTotalDurationInSeconds(uint32_t timerNumber){
    uint32_t baseAddress = intervalTimer_getBaseAddress(timerNumber); //grab base address of timer we're working with
    uint64_t upperBits =  intervalTimer_readGpioRegister(baseAddress, TCR1_OFFSET); //reads top 32 bits of counter
    uint32_t lowerBits =  intervalTimer_readGpioRegister(baseAddress, TCR0_OFFSET); //reads bottom 32 bits of counter
    uint64_t upperBits_second =  intervalTimer_readGpioRegister(baseAddress, TCR1_OFFSET); //reads top 32 bits again
    if(upperBits != upperBits_second){ //if those bits have changed, we need to reread bottom bits to make sure reading is correct
        lowerBits =  intervalTimer_readGpioRegister(baseAddress, TCR0_OFFSET); //reread bottom bits
    }
    uint64_t finalCount = (upperBits_second << SHIFT_LEFT | lowerBits); //concatenate and return 64 bit value
    return (double) finalCount/TIMER_FREQUENCY; //return final count cast to a double
}

