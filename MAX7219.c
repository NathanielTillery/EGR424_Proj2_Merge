/*
 * MAX7219.c
 *
 *  Created on: Jun 16, 2026
 *      Author: natet
 *      This C source file was created by Nathaniel C. Tillery for EGR424 Project 2.
 *      It is an Application Programming Interface file meant to act as a versatile
 *      driver for the MAX7219 7-segment 8 digit display.
 */


/* Macros for MAX7219 address codes */
//First is always 0 because D12-D15 are don't cares so I send a 0
#define ADDR_NOOP           0x00
#define ADDR_DIG0           0x01
#define ADDR_DIG1           0x02
#define ADDR_DIG2           0x03
#define ADDR_DIG3           0x04
#define ADDR_DIG4           0x05
#define ADDR_DIG5           0x06
#define ADDR_DIG6           0x07
#define ADDR_DIG7           0x08
#define ADDR_DECODE_MODE    0x09
#define ADDR_INTENSITY      0x0A
#define ADDR_SCAN_LIMIT     0x0B
#define ADDR_SHUTDOWN       0x0C
#define ADDR_DISPLAY_TEST   0x0F

/* Code B data codes */
#define DATA_0              0x00
#define DATA_1              0x01
#define DATA_2              0x02
#define DATA_3              0x03
#define DATA_4              0x04
#define DATA_5              0x05
#define DATA_6              0x06
#define DATA_7              0x07
#define DATA_8              0x08
#define DATA_9              0x09
#define DATA_DASH           0x0A
#define DATA_E              0x0B
#define DATA_H              0x0C
#define DATA_L              0x0D
#define DATA_P              0x0E
#define DATA_BLANK          0x0F
#define DATA_DP             0x80



#include "MAX7219.h"
#include "msp.h"

#define MAX7219_QUEUE_SIZE 32

/* Private command type for queued MAX7219 writes.
 * A MAX7219 command is two SPI bytes: register address first, then data.
 */
struct MAX7219_Command{
    uint8_t addr;
    uint8_t data;
};


/* 16-Bit Number sent to the MAX7219 hardware as an SPI command */
// Bits 0-7 DATA (0 LSB; 7 MSB)
// Bits 8-11 ADDRESS
// BITS 14-15 RESERVED
uint16_t SPI_Command = 0;

/* Queue state for display commands waiting on the interrupt-driven SPI driver.
 * Head is the next command to send; tail is where the next command is stored.
 */
static struct MAX7219_Command max7219Queue[MAX7219_QUEUE_SIZE];
static int max7219QueueHead = 0;
static int max7219QueueTail = 0;
static volatile int max7219QueueOverflow = 0;

static bool MAX7219_queueIsEmpty(void);
static bool MAX7219_queueIsFull(void);
static bool MAX7219_queueCommand(uint8_t addr, uint8_t data);

void MAX7219_Init(void){

    /* Initialize CS pin (P2.3 as GPIO output) */
    P2->SEL0 &= ~BIT3;
    P2->SEL1 &= ~BIT3;
    P2->DIR |= BIT3;
    P2->OUT |= BIT3; //P2.3 HIGH

    /* Configure and enable SPI */
    SPI_Init(); // Takes care of CLK (P1.5), MISO (P1.7), MOSI (P1.6)

    MAX7219_write(ADDR_DISPLAY_TEST, 0x00);     //Turn off display test mode
    MAX7219_write(ADDR_DECODE_MODE, 0xFF);  // Turn on CODE B built-in font
    MAX7219_write(ADDR_SCAN_LIMIT, 0x07);   // Use all 8 digits
    MAX7219_write(ADDR_INTENSITY, 0x04);    // medium-low brightness
    MAX7219_write(ADDR_SHUTDOWN, 0x01);     // normal operation
    MAX7219_Clear();                        // Clear the display
}



void MAX7219_write(uint8_t addr, uint8_t data){

    /* Queue the register/data command instead of blocking on SPI.
     * MAX7219_Service() starts transfers when the SPI hardware is available.
     */
    if(!MAX7219_queueCommand(addr, data)){
        max7219QueueOverflow = 1;
    }

    MAX7219_Service();

}

/* Takes two two digit and one four digit numbers and displays them */
void MAX7219_DisplayNumber(int leftNum, int rightNum1, int rightNum2){
    int tempNumLeft = leftNum;
    int tempNumRight1 = rightNum1;
    int tempNumRight2 = rightNum2;
    int tempDisplayNum = 0;
    int tempDig = 0x05;

    /* Display centimeters on the left side*/
    while(tempNumLeft > 0 && tempNumLeft <= 9999){                    // Make tempNum not run again after 0
        tempDisplayNum = tempNumLeft % 10;          // Get rightmost digit
        tempNumLeft = tempNumLeft / 10;                 // Remove rightmost digit
        MAX7219_write(tempDig, tempDisplayNum); // Write the rightmost digit to its place
        tempDig++;                              // Increment place
    }
    /* If the number doesn't need all of the digits, write the remaining digits to blank */
    while(tempDig <= 0x08){
        MAX7219_write(tempDig, DATA_BLANK);     // Write the rightmost digit to its place
        tempDig++;                              // Increment place
    }


    /* Display feet.inches on the right side*/
    /* Feet here */
    tempDig = 0x03;                                 // Set tempDig so that we're working on the 'right' side
    while(tempNumRight1 > 0 && tempNumRight1 <= 99){                   // Make tempNum not more than 2 digits
        tempDisplayNum = tempNumRight1 % 10;          // Get rightmost digit
        tempNumRight1 = tempNumRight1 / 10;                 // Remove rightmost digit
        if(tempDig == 3){
            MAX7219_write(tempDig, (tempDisplayNum | DATA_DP)); // Write the rightmost digit to its place WITH DECIMAL
        }else{
            MAX7219_write(tempDig, tempDisplayNum); // Write the rightmost digit to its place without decimal
        }
        tempDig++;                              // Increment place
    }

    /* If the number doesn't need all of the digits, write the remaining digits to blank */
    while(tempDig <= 0x04){
        MAX7219_write(tempDig, DATA_BLANK);     // Write the rightmost digit to its place
        tempDig++;                              // Increment place
    }


    /* Inches here */
    tempDig = 0x01;                                 // Set tempDig so that we're working on the 'right' side of the 'right' side
    while(tempNumRight2 > 0 && tempNumRight2 <= 99){                           // Make tempNum not run again after 0
        tempDisplayNum = tempNumRight2 % 10;          // Get rightmost digit
        tempNumRight2 = tempNumRight2 / 10;                 // Remove rightmost digit
        MAX7219_write(tempDig, tempDisplayNum); // Write the rightmost digit to its place
        tempDig++;                              // Increment place

    }

    /* If the number doesn't need all of the digits, write the remaining digits to blank */
    while(tempDig <= 0x02){
        MAX7219_write(tempDig, DATA_BLANK);     // Write the rightmost digit to its place
        tempDig++;                              // Increment place
    }




}

void MAX7219_Clear(void){
    int i;
    for(i = 0x01; i <= 0x08; i++){
        MAX7219_write(i, DATA_BLANK);     // Write the rightmost digit to its place
    }
}


/* Helper functions to deal with CS pin */
void MAX7219_CS_low(void){P2->OUT &= ~BIT3;} //P2.3 OFF
void MAX7219_CS_high(void){P2->OUT |= BIT3;} //P2.3 ON



/* Services the MAX7219 queue using the nonblocking SPI driver.
 * CS stays low through the full two-byte command and goes high when SPI is done.
 */
void MAX7219_Service(void)
{
    static bool transferActive = false;
    static uint8_t txBytes[2];

    if(transferActive && SPI_transferDone()){
        MAX7219_CS_high();
        SPI_clearTransferDone();
        transferActive = false;
    }

    if(!transferActive && !MAX7219_queueIsEmpty() && !SPI_isBusy()){
        txBytes[0] = max7219Queue[max7219QueueHead].addr;
        txBytes[1] = max7219Queue[max7219QueueHead].data;

        MAX7219_CS_low();

        if(SPI_StartTransfer(txBytes, 2)){
            max7219QueueHead = (max7219QueueHead + 1) % MAX7219_QUEUE_SIZE;
            transferActive = true;
        }
        else{
            MAX7219_CS_high();
        }
    }
}

static bool MAX7219_queueIsEmpty(void)
{
    return (max7219QueueHead == max7219QueueTail);
}

/* Queue helpers for the MAX7219 command buffer.
 * These helpers are private to MAX7219.c.
 */
static bool MAX7219_queueIsFull(void)
{
    return (((max7219QueueTail + 1) % MAX7219_QUEUE_SIZE) == max7219QueueHead);
}

static bool MAX7219_queueCommand(uint8_t addr, uint8_t data)
{
    if(MAX7219_queueIsFull()){
        return false;
    }

    max7219Queue[max7219QueueTail].addr = addr;
    max7219Queue[max7219QueueTail].data = data;
    max7219QueueTail = (max7219QueueTail + 1) % MAX7219_QUEUE_SIZE;

    return true;
}
