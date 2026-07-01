/*
 * HC-SR04.c
 *
 *  Created on: Jun 16, 2026
 *      Author: natet
 *      This C source file was created by Nathaniel C. Tillery for EGR424 Project 2.
 *      It is an Application Programming Interface file meant to act as a versatile
 *      driver for the HC-SR04 Ultrasonic Distance Sensor.
 */


#include <HCSR04.h>
#include "Motor_Ctrl/Motor_Ctrl.h"

/* Initialize HC-SR04 distance sensor module */
void HCSR04_Init(){
    //Set up trig and pulse pin
    //Set P2.4 to PWM mode TA0.1 for trig
    //Set P2.5 to TA0.2 for input capture

    //Set P2.4, P2.5 both to primary function (0b01)
    P2->SEL0 |= (BIT4 | BIT5);
    P2->SEL1 &= ~(BIT4 | BIT5);
    P2->DIR |= BIT4;                // P2.4 OUTPUT
    P2->DIR &= ~BIT5;               // P2.4 INPUT
    P2->OUT &= ~BIT4;               // LP2.4 OFF to start

    TIMER_A_Init();                 // Initialize P2.4 and P2.5 for their TA functionality
}



/* Returns distance of object in centimeters */
void calcDist(int echoFallTime, int echoRiseTime, struct distance *dist){

    int timeDiff, distIN_TOTAL;

    /* Calculate CM */
    if(echoFallTime <= echoRiseTime){
        //If rollover, compensate with period value
        timeDiff = (((45000 - 1) - echoRiseTime + echoFallTime + 1)) * 4 / 3;
    }
    else{
        timeDiff = (echoFallTime - echoRiseTime) * 4 / 3; // Calculate time of flight (750kHz)
    }

    //(*dist).distanceCentimeters
    dist->distanceCentimeters = timeDiff / 58;         // Calculate CM (source HCSR04 datasheet)
    distIN_TOTAL = timeDiff / 148;  // Calculate total inches (source HCSR04 datasheet)
    dist->distanceFeet = distIN_TOTAL / 12;     // Get feet broken off from inches
    dist->distanceInches = distIN_TOTAL % 12;     // Figure out how many inches are left over

    if( dist->distanceInches >= 0 && dist->distanceInches < 6 )
    {
        setMotorDirection( true );
        setMotorSpeed( 2 );
    }
    else if( dist->distanceInches >= 6 && dist->distanceInches < 12 )
    {
        setMotorDirection( true );
        setMotorSpeed( 4 );
    }
    else if( dist->distanceInches >= 12 && dist->distanceInches < 18 )
    {
        setMotorDirection( false );
        setMotorSpeed( 2 );
    }
    else if( dist->distanceInches >= 18 && dist->distanceInches < 12 )
    {
        setMotorDirection( false );
        setMotorSpeed( 4 );
    }
    else
    {
        setMotorDirection( true );
        setMotorSpeed( 0 );
    }
}
