/*
 * switches.c
 *
 *  Created on: Mar 21, 2018
 *      Author: logan
 */

#include "switches.h"
#include "xil_io.h"
#include "supportFiles/leds.h"
#include <stdio.h>

//constants set for lab report
#define SWITCHES_GPIO_DEVICE_BASE_ADDRESS XPAR_SLIDE_SWITCHES_BASEADDR //SWITCHES BASE ADDRESS
#define GPIO_DATA_OFFSET 0x0 //DATA_REGISTERS BASELINE OFFSET
#define GPIO_TRI_OFFSET 0x4 //TRI_OFFSET

//constants to initialize switches and LEDs for usage
#define BLANK_LEDS 0x0 //TURN LEDS OFF
#define SWITCHES_ON 0xF //TURN SWITCHES ON
#define ALL_INPUT 0xF //CHECK TO MAKE SURE GPIO_TRI IS INPUT

void switches_writeGpioRegister(int32_t offset, int32_t value){ //helper function to write to GPIO registers
    Xil_Out32(SWITCHES_GPIO_DEVICE_BASE_ADDRESS + offset, value); //lets us read the GPIO registers in C
}

// Helper function to read GPIO registers.
int32_t switches_readGpioRegister(int32_t offset) {
//   Just use the low-level Xilinx call.
   return Xil_In32(SWITCHES_GPIO_DEVICE_BASE_ADDRESS + offset); //lets us read the GPIO registers in C
}

// Initializes the SWITCHES driver software and hardware. Returns one of the STATUS values defined above.
int32_t switches_init(){
    leds_init(BLANK_LEDS); //initiate the leds to 0
    switches_writeGpioRegister(GPIO_TRI_OFFSET, ALL_INPUT); //write the registers as inputs
    if(switches_readGpioRegister(GPIO_TRI_OFFSET) != ALL_INPUT){ //checking to make sure there registers are inputs
        return SWITCHES_INIT_STATUS_FAIL; //written improperly
    }
    return SWITCHES_INIT_STATUS_OK; //the GPIO TRI was written properly
}

// Returns the current value of all 4 switches as the lower 4 bits of the returned value.
// bit3 = SW3, bit2 = SW2, bit1 = SW1, bit0 = SW0.
int32_t switches_read(){
    return switches_readGpioRegister(GPIO_DATA_OFFSET);
}

// Runs a test of the switches. As you slide the switches, LEDs directly above the switches will illuminate.
// The test will run until all switches are slid upwards. When all 4 slide switches are slid upward,
// this function will return.
void switches_runTest(){
    if(switches_init() == SWITCHES_INIT_STATUS_FAIL){
        printf("switches_init failed!\n\r"); //print switches init failed
        return;
    }
    switches_init(); //init the switches
    //create an int32_t and read from the switches
    int32_t switches = switches_read();
   //put in while loop: while not all switches are pressed
    while(switches != SWITCHES_ON){ //will break out of while loop when all switches are pressed
        leds_write(switches); //write to the leds based on switch selection that comes from reading the switches
        switches = switches_read(); //continue reading the switches as the while loop iterates to update LEDs
    }
    //blanks LEDs when the test is done running
    leds_write(BLANK_LEDS); //leds_write is included from the leds.h file
}



