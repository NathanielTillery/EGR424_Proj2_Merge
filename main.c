/*
 * Author: Nathaniel C. Tillery
 * Date: 14, June 2026
 * Description: EGR424 Project 2; Low Level C Peripheral Driver API Development
 *
 */


/* Standard Includes */
#include "msp.h"
#include <stdbool.h>

//INCLUDE API DRIVERS
#include "MAX7219.h"
#include "HCSR04.h"
#include "spi.h"
#include "SysTick/SysTick.h"
#include "Motor_Ctrl/Motor_Ctrl.h"

/* Global variables */
volatile int SysTickTimeout = 0;
volatile int SysTickCount = 0;

volatile int echoRiseTime = 0;
volatile int echoFallTime = 0;
volatile int echoRiseCaptured = 0;
volatile int echoFallCaptured = 0;
volatile int timeDiff = 0;
volatile int distCalcAvailable = 0;


struct distance objectDistance;


/**
 * main.c
 */
void main(void)
{

	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer

	/* Initialize System */
	SysTick_Init();
    MAX7219_Init();
    HCSR04_Init();
    Motor_Init();           //initializing Motor pins

    __enable_irq();     // Enable global interrupts so program can really start

    int currCM = 0;
    int oldCM = 0;


	while(1){

	    if(distCalcAvailable){
	        distCalcAvailable = 0;                          // Reset the flag
	        calcDist(echoFallTime, echoRiseTime, &objectDistance);  // Call distance calculation function

	        /* Only update display when cm change */
	        oldCM = currCM;
	        currCM = objectDistance.distanceCentimeters;
	        if((oldCM != currCM) && MAX7219_IsReadyForFrame()){   // Only add new measurement if it has changed and we finished writing the last one
	            MAX7219_DisplayNumber(objectDistance.distanceCentimeters, objectDistance.distanceFeet, objectDistance.distanceInches);
	        }

	    }

	    /* Service pending display writes outside the SPI ISR. */
	    MAX7219_Service();


        if( gSysTickFlag )                        //If SysTick Interrupt occurs
        {
            MotorMove();                          //move the stepper motor accordingly
            gSysTickFlag = false;                 //reset SysTick interrupt flag
        }

	}

}






