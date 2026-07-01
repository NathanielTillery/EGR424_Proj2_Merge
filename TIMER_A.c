/*
 * TIMER_A.c
 *
 *  Created on: Jun 27, 2026
 *      Author: natet
 *      This C source file was created by Nathaniel C. Tillery for EGR424 Project 2.
 *      It is an Application Programming Interface file meant to act as a versatile
 *      driver for the TIMER_A on the MSP432 Launchpad.
 */

#include "TIMER_A.h"

extern volatile int echoRiseCaptured;
extern volatile int echoFallCaptured;
extern volatile int echoRiseTime;
extern volatile int echoFallTime;
extern volatile int timeDiff;
extern volatile int distCalcAvailable;

/* Initialize TIMER_A to support the functionality of the HCSR04 module */
void TIMER_A_Init(void){             //MSP432 Technical Reference 17.3.1


    TIMER_A0->CCR[0] = 45000-1;    //Set period (10uS) 60ms / 1.333uS = 45000 ticks

    TIMER_A0->CCR[1] = 8;          // Give 10uS high-time with 8 ticks (Duty Cycle for PWM)

    //Set up TA0.1 for P2.4 PWM trigger output
    //No capture; output mode toggle; no interrupt
    TIMER_A0->CCTL[1] = 0x00E0;    //0b 0000 0000 1110 0000

    //Set up TA0.2 for P2.5 capture compare
    //Both edges; synchronous; interrupts; capture mode
    TIMER_A0->CCTL[2] = 0xC910;    //0b 1100 1001 0001 0000

    NVIC->ISER[0] |= 1 << ((TA0_N_IRQn) & 31);   // Enable TA0 CCR2 interrupt

    /* Enable TIMER A0 at 3MHZ / 4 = 750kHz; 1tick ~= 1.33us; UP mode SMCLK */
    TIMER_A0->CTL = 0x0290;      // 0b 0000 0010 1001 0000

}

/* Handles interrupts for both PWM and capture compare */
void TA0_N_IRQHandler(void){

    /* Set flags for P2.5 rising / falling edge */
    if((TIMER_A0->CCTL[2] & CCIFG) == 1){            // Check TA0CCR2 CCIFG (MSP tech ref 17.3.5)
        TIMER_A0->CCTL[2] &= ~CCIFG;                   // Clear TA0CCR2 CCIFG (Capture Compare IFG for TA02)
        if((echoRiseCaptured == 0) && (echoFallCaptured == 0)){
            echoRiseTime = TIMER_A0->CCR[2];      // Set echoFallTime to timer value (MSP tech tef 17.2.4.1)
            echoRiseCaptured = 1;                        // Set echo status flag to high
            echoFallCaptured = 0;                       // Set echo status flag to low
        }
        else if(echoRiseCaptured){
            echoFallTime = TIMER_A0->CCR[2];      // Set echoRiseTime to timer value
            echoRiseCaptured = 1;                       // Set echo status flag to high
            echoFallCaptured = 1;                        // Set echo status flag to high
        }
        if(echoRiseCaptured  && echoFallCaptured != 0){
            distCalcAvailable = 1;                  // If there is a rise and fall time recorded, then we can calculate distance.
            echoRiseCaptured = 0;                       // Set echo status flag to low
            echoFallCaptured = 0;                        // Set echo status flag to low
        }
    }

}
