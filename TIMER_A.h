/*
 * TIMER_A.h
 *
 *  Created on: Jun 27, 2026
 *      Author: natet
 *      This C header file was created by Nathaniel C. Tillery for EGR424 Project 2.
 *      It is an Application Programming Interface file meant to act as a versatile
 *      driver for the TIMER_A on the MSP432 Launchpad.
 */

#ifndef TIMER_A_H_
#define TIMER_A_H_

#include "msp.h"


void TIMER_A_Init(void);

void TA0_N_IRQHandler(void);



#endif /* TIMER_A_H_ */
