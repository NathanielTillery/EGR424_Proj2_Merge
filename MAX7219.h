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
#include <stdint.h>
#include "spi.h"



/* Public function declarations */
void MAX7219_Init(void); // Initialize
void MAX7219_Clear(void); // Clear the entire display
void MAX7219_DisplayNumber(int leftNum, int rightNum1, int rightNum2); // Send an number to display
void MAX7219_write(uint8_t addr, uint8_t data); //Sends 16 bits; address and then data (plus CS low and CS high)
/* Main calls this often so queued MAX7219 writes can progress. */
void MAX7219_Service(void);
bool MAX7219_IsReadyForFrame(void);

void MAX7219_CS_low(void);
void MAX7219_CS_high(void);


#endif /* MAX7219_H_ */
