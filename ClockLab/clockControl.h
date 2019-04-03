/*
 * clockControl.h
 *
 *  Created on: Apr 10, 2018
 *      Author: logan
 */

#ifndef CLOCKCONTROL_H_
#define CLOCKCONTROL_H_

//THIS IS WHERE I CREATE THE STATE MACHINE

// Standard tick function.
void clockControl_tick();

// Call this before you call clockControl_tick().
void clockControl_init();



#endif /* CLOCKCONTROL_H_ */
