/*
 * spi.c
 *
 *  Created on: Jun 16, 2026
 *      Author: natet
 *      This C source file was created by Nathaniel C. Tillery for EGR424 Project 2.
 *      It is an Application Programming Interface file meant to act as a versatile
 *      driver for the SPI communications in this project.
 */


#include <spi.h>



/* eusci.h stuff */
#define EUSCI_A_CMSIS(x) ((EUSCI_A_Type *) x)
#define EUSCI_B_CMSIS(x) ((EUSCI_B_Type *) x)

#define SPI_BUFFER_SIZE 2


/* Transfer state for nonblocking SPI operation.
 * The buffer and counters must remain valid after SPI_StartTransfer() returns
 * because the interrupt handler completes the transfer later.
 */
volatile uint8_t spiBuffer[2];
volatile int spiIndex = 0;
volatile int spiLength = 0;
volatile int spiRxCount = 0;
volatile int spiBusy = 0;
volatile int spiDone = 0;


/*
    The recommended eUSCI initialization/reconfiguration process is:
    1. Set UCSWRST.
    2. Initialize all eUSCI registers with UCSWRST = 1 (including UCxCTL1).
    3. Configure ports.
    4. Clear UCSWRST through software. (enables SPI)
    5. Enable interrupts (optional) with UCRXIE or UCTXIE.
*/
void SPI_Init(void){
    // Read from C:\Users\natet\OneDrive\EGR424\Resources\MSP432_TechnicalReference_Guide.pdf
    // Section 23.4 (p786); 23.5.1 (p.777)


    //UCWRST Software reset
    EUSCI_B0->CTLW0 = 0x0001;

    //Set baud rate to 9600 for SPI by using UCB0BRW
    EUSCI_B0->BRW = 312;
    //BRW = clock / baud rate; clk is 3MHZ

    //Configure control register for master, synchronous, 3-pin SPI, UCCKPH. 8-bit data, MSB first
    EUSCI_B0->CTLW0 = 0xA981;   // 0b 1010 1001 1000 0000 -> 0xA980 (+ reset bit = 0xAa981)

    //Set P1.5 (CLK), P1.6 (MOSI), P1.7 (MISO) to Primary Module Function (0b01)
    P1->SEL0 |= (7u << 5);
    P1->SEL1 &= ~(7u << 5);

    // Clear bit 0 of UCA0CTLW0 register to enable SPI (UCWRST BIT)
    EUSCI_B0->CTLW0 &= ~0x0001;

    //Enable interrupts
    EUSCI_B0-> IE &= ~0x000F;
}


/* EUSCI_B0 SPI ISR for nonblocking transfers.
 * TXIFG means TXBUF can accept another byte. RXIFG means one byte has fully
 * clocked through, so the RX count decides when the transfer is complete.
 */
void EUSCIB0_IRQHandler(void){
    if (EUSCI_B0->IFG & EUSCI_B_IFG_RXIFG){
        volatile uint8_t dummy = EUSCI_B0->RXBUF;
        (void)dummy;

        if(spiBusy && (spiRxCount < spiLength)){
            spiRxCount++;
        }

        if(spiBusy && (spiRxCount >= spiLength)){
            spiBusy = 0;
            spiDone = 1;
            EUSCI_B0->IE &= ~(EUSCI_B_IE_TXIE | EUSCI_B_IE_RXIE);
        }
    }

    if (EUSCI_B0->IFG & EUSCI_B_IFG_TXIFG){
        if (spiBusy && (spiIndex < spiLength)){
            EUSCI_B0->TXBUF = spiBuffer[spiIndex];
            spiIndex++;
        }
        else{
            EUSCI_B0->IE &= ~EUSCI_B_IE_TXIE;
        }
    }

}


/* Starts an SPI transfer without waiting for it to finish.
 * New work is rejected while SPI is busy.
 */
bool SPI_StartTransfer(const uint8_t *bytes, uint8_t length)
{
    int i;

    if(bytes == 0 || length == 0 || length > SPI_BUFFER_SIZE){
        return false;
    }

    NVIC_DisableIRQ(EUSCIB0_IRQn);

    if(spiBusy){
        NVIC_EnableIRQ(EUSCIB0_IRQn);
        return false;
    }

    for(i = 0; i < length; i++){
        spiBuffer[i] = bytes[i];
    }

    spiLength = length;
    spiIndex = 0;
    spiRxCount = 0;
    spiBusy = true;
    spiDone = false;

    if(EUSCI_B0->IFG & EUSCI_B_IFG_RXIFG){
        volatile uint8_t dummy = EUSCI_B0->RXBUF;
        (void)dummy;
    }

    EUSCI_B0->IE |= EUSCI_B_IE_RXIE;
    EUSCI_B0->IE |= EUSCI_B_IE_TXIE;

    NVIC_EnableIRQ(EUSCIB0_IRQn);

    return true;
}

bool SPI_isBusy(void)
{
    return spiBusy;
}

/* Status helpers used by higher-level drivers such as MAX7219.
 * SPI reports transport state; device-specific drivers control chip select.
 */
bool SPI_transferDone(void)
{
    return spiDone;
}

void SPI_clearTransferDone(void)
{
    spiDone = false;
}
