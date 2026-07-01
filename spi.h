/*
 * spi.h
 *
 *  Created on: Jun 16, 2026
 *      Author: natet
 *      This header file was created by Nathaniel C. Tillery for EGR424 Project 2.
 *      It is an Application Programming Interface file meant to act as a versatile
 *      driver for the SPI communications in this project.
 */


/*
 * To set up a driver, we first do EUSCI_A0/B0 then choose the pins, then do the other EUSCI stuff
 * Then we can do the actual functions for UART, SPI, I2C
 * All 3 of these communication styles use EUSCI; SPI uses either of the two
 *
 */
#ifndef SPI_H_
#define SPI_H_

#include <stdbool.h>
#include <stdint.h>
#include <msp.h>


/* eusci.h stuff */
#define EUSCI_A_CMSIS(x) ((EUSCI_A_Type *) x)
#define EUSCI_B_CMSIS(x) ((EUSCI_B_Type *) x)


void SPI_Init(void);

/* Nonblocking SPI interface used by device drivers. */
bool SPI_StartTransfer(const uint8_t *bytes, uint8_t length);
bool SPI_isBusy(void);
bool SPI_transferDone(void);
void SPI_clearTransferDone(void);

void EUSCIB0_IRQHandler(void);



#endif /* SPI_H_ */
