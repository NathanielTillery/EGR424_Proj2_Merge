/*
 * SysTick.h
 *
 *  Created on: Jun 19, 2026
 *      Author: Max Fenwick
 */

#ifndef SYSTICK_SYSTICK_H_
#define SYSTICK_SYSTICK_H_

#include <stdint.h>
#include <stdbool.h>

extern bool gSysTickFlag;                 //Global Interrupt Flag used to signify that a SysTick interrupt has occurred

void SysTick_Init(void);                  //Initializes SysTick Timer to 3 MHz with interruts
void SysTickSetReload( uint16_t value );  //Sets the reload value for the SysTick Timer on reload for interrupt

#endif /* SYSTICK_SYSTICK_H_ */
