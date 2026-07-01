
#include "SysTick/SysTick.h"
#include "msp.h"

bool gSysTickFlag = false;              //Global interrupt flag used to show that an interrupt has occurred, initialized to false

static uint16_t reloadValue = 0x00FF;
/*=========================================================================
 * Name:      SysTick_Init
 * Inputs:    None
 * Outputs:   None
 * Purpose:   To set the Systick Timer to 3 MHz with an interrupt
 =========================================================================*/
void SysTick_Init(void)            //initialize SysTick timer function
{
    SysTick -> CTRL = 0;           //Disable SysTick during setup
    SysTick -> LOAD = 0x00FFFFFF;  //maximum reload value
    SysTick -> VAL = 0;            //Any write clears timer
    SysTick -> CTRL = 0x00000007;  //enable SysTick, 3 MHz, no interrupts
    gSysTickFlag = false;          //reset interrupt flag whenever SysTick Timer is
}                                  //initialized or re-initialized

/*=========================================================================
 * Name:      SysTickSetReload
 * Inputs:    Updated reload value
 * Outputs:   None
 * Purpose:   Sets new reload value for the SysTick interrupt
 =========================================================================*/
void SysTickSetReload( uint16_t value )
{
    reloadValue = value;
}

/*=========================================================================
 * Name:      SysTickISR
 * Inputs:    None
 * Outputs:   None
 * Purpose:   SysTick Interrupt that sets gSysTickFlag
 =========================================================================*/
void SysTick_Handler(void)
{
    gSysTickFlag = 1;
    SysTick -> LOAD = ((reloadValue * 3000)-1);  //delay, if reloadValue equals 1, the total delay is
                                                 //equal to 1 ms
    SysTick -> VAL = 0;                          //any write to CVR clears COUNTFLAG in CSR
}
