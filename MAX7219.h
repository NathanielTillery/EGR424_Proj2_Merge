/*
 * MAX7219.h
 *
 *  Created on: Jun 16, 2026
 *      Author: natet
 *      This header file was created by Nathaniel C. Tillery for EGR424 Project 2.
 *      It is an Application Programming Interface file meant to act as a versatile
 *      driver for the MAX7219 7-segment 8 digit display.
 */

#ifndef MAX7219_H_
#define MAX7219_H_

/* Includes */
#include <msp.h>
#include <stdbool.h>
#include <stdint.h>
#include "spi.h"



/* Public function declarations */

/* *****************************************************************
 * Initializes the MAX7219 driver, chip-select pin, and SPI module.
 * Initialization commands are queued and sent by MAX7219_Service().
 * *****************************************************************/
void MAX7219_Init(void);

/* Queues blank writes for all eight digits. */
void MAX7219_Clear(void);

/* **************************************
 * Queues one full display update.
 * leftNum is written to digits 5-8.
 * rightNum1 is written to digits 3-4.
 * rightNum2 is written to digits 1-2.
 * **************************************/
void MAX7219_DisplayNumber(int leftNum, int rightNum1, int rightNum2);

/* ***************************************************************
 * Queues one raw MAX7219 register/data command using
 * MAX7219_queueCommand(); returns false if queue is overflowing
 * addr is the MAX7219 register address.
 * data is the value to write to that register.
 * ***************************************************************/
void MAX7219_write(uint8_t addr, uint8_t data);

/* ********************************************************************
 * Services queued MAX7219 commands using the nonblocking SPI driver.
 * Main should call this often so queued display writes can progress.
 * ********************************************************************/
void MAX7219_Service(void);

/* Returns true when the display queue is empty and SPI is available. */
bool MAX7219_IsReadyForFrame(void);


#endif /* MAX7219_H_ */
