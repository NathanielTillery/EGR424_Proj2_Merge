/*
 * SysTick.c
 *
 *  Created on: Jun 16, 2026
 *      Author: natet
 *      This C source file was created by Nathaniel C. Tillery for EGR424 Project 2.
 *      It is an Application Programming Interface file meant to act as a versatile
 *      driver for the SysTick functions in this project.
 */


#include "SysTick.h"


void SysTick_Init(void){

    //SysTick_setPeriod(1500000); //MCLK is 3MHZ so this will toggle every 0.5s
    //Pulse for trig is 10uS -> 1*10^-5
    //3MHZ / x = 0.00001
    SysTick_setPeriod(375000); //MCLK is 3MHZ so this will toggle every 0.125s
    //SysTick_setPeriod(30); //MCLK is 3*10^6 so this will toggle 3*10^6 / 1*10^5
    SysTick_enableModule(); //Start the timer
    SysTick_enableInterrupt(); //Enable SysTick Interrupt

}

void SysTick_Handler(void)
{
    //MAP_GPIO_toggleOutputOnPin(GPIO_PORT_P1, GPIO_PIN0);
    P1->OUT ^= BIT0;    // LED toggle
    SysTickTimeout = 1;
}


/* Following code taken from Texas Instruments driverlib systick.c */
void SysTick_enableModule(void)
{
    //
    // Enable SysTick.
    //
    SysTick->CTRL |= SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk;
}

void SysTick_disableModule(void)
{
    //
    // Disable SysTick.
    //
    SysTick->CTRL &= ~(SysTick_CTRL_ENABLE_Msk);
}


void SysTick_enableInterrupt(void)
{
    //
    // Enable the SysTick interrupt.
    //
    SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;
}

void SysTick_disableInterrupt(void)
{
    //
    // Disable the SysTick interrupt.
    //
    SysTick->CTRL &= ~(SysTick_CTRL_TICKINT_Msk);
}

void SysTick_setPeriod(uint32_t period)
{
    //
    // Check the arguments.
    //
    //ASSERT((period > 0) && (period <= 16777216));

    // Here's how we did it in the other lab for reference.
    // 250 is the ms and 3000 is the clock... So 250 * 3000 is
    // the period for that one. (not necessarily this one)
    //SysTick->LOAD = (3000 * 250) - 1;

    //
    // Set the period of the SysTick counter.
    //
    SysTick->LOAD = period - 1;
}

uint32_t SysTick_getPeriod(void)
{
    //
    // Return the period of the SysTick counter.
    //
    return (SysTick->LOAD + 1);
}

uint32_t SysTick_getValue(void)
{
    //
    // Return the current value of the SysTick counter.
    //
    return (SysTick->VAL);
}
