/*
 * SysTick.h
 *
 *  Created on: Jun 16, 2026
 *      Author: natet
 *      This header file was created by Nathaniel C. Tillery for EGR424 Project 2.
 *      It is an Application Programming Interface file meant to act as a versatile
 *      driver for the SysTick functions in this project.
 */

#ifndef SYSTICK_H_
#define SYSTICK_H_

/* Includes */
#include <stdint.h>
#include <msp.h>


/* Variables */
extern volatile int SysTickTimeout;


/* Function Declarations */



void SysTick_Init(void); // Initializes SysTick
void SysTick_Handler(void); // Handles it

/* Following code taken from Texas Instruments driverlib systick.h */
//*****************************************************************************
//
//! Enables the SysTick counter.
//!
//! This function starts the SysTick counter.  If an interrupt handler has been
//! registered, it is called when the SysTick counter rolls over.
//!
//! \note Calling this function causes the SysTick counter to (re)commence
//! counting from its current value.  The counter is not automatically reloaded
//! with the period as specified in a previous call to SysTick_setPeriod().  If
//! an immediate reload is required, the \b NVIC_ST_CURRENT register must be
//! written to force the reload.  Any write to this register clears the SysTick
//! counter to 0 and causes a reload with the supplied period on the next
//! clock.
//!
//! \return None.
//
//*****************************************************************************
extern void SysTick_enableModule(void);

//*****************************************************************************
//
//! Disables the SysTick counter.
//!
//! This function stops the SysTick counter.  If an interrupt handler has been
//! registered, it is not called until SysTick is restarted.
//!
//! \return None.
//
//*****************************************************************************
extern void SysTick_disableModule(void);

//*****************************************************************************
//
//! Enables the SysTick interrupt.
//!
//! This function enables the SysTick interrupt, allowing it to be
//! reflected to the processor.
//!
//! \note The SysTick interrupt handler is not required to clear the SysTick
//! interrupt source because it is cleared automatically by the NVIC when the
//! interrupt handler is called.
//!
//! \return None.
//
//*****************************************************************************
extern void SysTick_enableInterrupt(void);

//*****************************************************************************
//
//! Disables the SysTick interrupt.
//!
//! This function disables the SysTick interrupt, preventing it from being
//! reflected to the processor.
//!
//! \return None.
//
//*****************************************************************************
extern void SysTick_disableInterrupt(void);

//*****************************************************************************
//
//! Sets the period of the SysTick counter.
//!
//! \param period is the number of clock ticks in each period of the SysTick
//! counter and must be between 1 and 16,777,216, inclusive.
//!
//! This function sets the rate at which the SysTick counter wraps, which
//! equates to the number of processor clocks between interrupts.
//!
//! \note Calling this function does not cause the SysTick counter to reload
//! immediately.  If an immediate reload is required, the \b NVIC_ST_CURRENT
//! register must be written.  Any write to this register clears the SysTick
//! counter to 0 and causes a reload with the \e period supplied here on
//! the next clock after SysTick is enabled.
//!
//! \return None.
//
//*****************************************************************************
extern void SysTick_setPeriod(uint32_t period);

//*****************************************************************************
//
//! Gets the period of the SysTick counter.
//!
//! This function returns the rate at which the SysTick counter wraps, which
//! equates to the number of processor clocks between interrupts.
//!
//! \return Returns the period of the SysTick counter.
//
//*****************************************************************************
extern uint32_t SysTick_getPeriod(void);

//*****************************************************************************
//
//! Gets the current value of the SysTick counter.
//!
//! This function returns the current value of the SysTick counter, which is
//! a value between the period - 1 and zero, inclusive.
//!
//! \return Returns the current value of the SysTick counter.
//
//*****************************************************************************
extern uint32_t SysTick_getValue(void);


#endif /* SYSTICK_H_ */
