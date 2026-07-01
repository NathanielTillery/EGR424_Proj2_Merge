/*
 * HC-SR04.h
 *
 *  Created on: Jun 16, 2026
 *      Author: natet
 *      This header file was created by Nathaniel C. Tillery for EGR424 Project 2.
 *      It is an Application Programming Interface file meant to act as a versatile
 *      driver for the HC-SR04 Ultrasonic Distance Sensor.
 */

#ifndef HCSR04_H_
#define HCSR04_H_


/* Includes */
#include "TIMER_A.h"

/* Public constants & variables*/

/* Holds all distance values */
struct distance{
    int distanceCentimeters;    // Total distance in centimeters
    int distanceFeet;       // Distance in feet as a component of feet.inches
    int distanceInches;     // Distance in inches as a component of feet.inches
};

extern struct distance objectDistance;

/* Public function declarations */

void HCSR04_Init(void);


void calcDist(int echoFallTime, int echoRiseTime, struct distance *dist);




#endif /* HCSR04_H_ */
