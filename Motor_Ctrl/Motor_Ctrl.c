#include "msp.h"
#include "SysTick/SysTick.h"
#include "Motor_Ctrl.h"


static bool mTargetCW = true;                  //motor targeted direction global to module, true is clockwise, false is anti-clockwise
static bool mCW = true;                        //motor current direction global to module, true = clockwise, false = anti-clockwise
static uint8_t mSpeed = MOTOR_SPEED_0;         //current motor speed global to module, 0 is not moving, 1 is slow - 4 is fastest
static uint8_t mTargetSpeed = MOTOR_SPEED_0;   //target motor speed global to moduel, 0 is not movine, 1 is slow, 4 is fastest

/*=========================================================================
 * Name:      Motor_Init
 * Inputs:    None
 * Outputs:   None
 * Purpose:   This function initializes the stepper motor pins 0, 1, 2, and
 *            3 on Port 4
 =========================================================================*/
void Motor_Init(void)
{
    P4SEL0 &= ~(BIT0|BIT1|BIT2|BIT3);     //declaring pins 0, 1, 2, and 3 of Port 4 as general GPIO
    P4SEL1 &= ~(BIT0|BIT1|BIT2|BIT3);
    P4DIR |= (BIT0|BIT1|BIT2|BIT3);         //declaring pins 0, 1, 2, and 3 of Port 4 as output
    P4OUT &= ~(BIT0|BIT1|BIT2|BIT3);      //initializing all pins on Port 4 to zero
}

/*=========================================================================
 * Name:      setMotorDirection
 * Inputs:    the desired motor direction as a boolean
 * Outputs:   None
 * Purpose:   Sets the motor direction as C (Clockwise) or A Anti-clockwise
 =========================================================================*/
void setMotorDirection( bool pCW )
{
    mCW = mTargetCW;                //Saving the current direction for use in the set motor speed function
    mTargetCW = pCW;                //Saving the target direction
}

/*=========================================================================
 * Name:      setMotorSpeed
 * Inputs:    None
 * Outputs:   None
 * Purpose:   Sets desired target speed and saves the current speed
 =========================================================================*/
void setMotorSpeed( uint8_t pSpeed )
{
    mSpeed = mTargetSpeed;                 //updating speed to previous target speed

    switch( pSpeed )                       //Updating motor target speed
    {
        case 0:
            mTargetSpeed =  MOTOR_SPEED_0;
            break;

        case 1:
            mTargetSpeed =  MOTOR_SPEED_1;
            break;

        case 2:
            mTargetSpeed =  MOTOR_SPEED_2;
            break;

        case 3:
            mTargetSpeed =  MOTOR_SPEED_3;
            break;

        case 4:
            mTargetSpeed =  MOTOR_SPEED_4;
            break;

        default:
            mTargetSpeed =  MOTOR_SPEED_4;
            break;
    }
}

/*=========================================================================
 * Name:      setMotorSpeed
 * Inputs:    None
 * Outputs:   None
 * Purpose:   Sets how the motor moves.  It is triggered by the gSysTickFlag
 *            It sets the firing order of bits to go one direction or the other
 *            and sets the reload value of the SysTick timer.
 =========================================================================*/
void MotorMove( void )
{
    static int j = 0;                      //initializing static variable used as counter
    static uint16_t cycle = 0;             //variable used to hold the port 2 position that is to be
                                           //removed on next cycle
    if( mTargetCW != mCW )                 //If previous direction is not the current targeted direction
    {
        mSpeed = mSpeed + MOTOR_SPEED_RAMP;    //then start ramping down
        SysTickSetReload( mSpeed );            //Adjust the reload time on next interrupt
        if( MOTOR_SPEED_0 <= mSpeed )          //if current motor speed is greater than the number of ms for speed 0
        {
            mCW = mTargetCW;                   //then change directions
        }
    }
    else if( mSpeed < mTargetSpeed )       //if it is headed in the correct direction and speed time is less than targeted
    {
        mSpeed += MOTOR_SPEED_RAMP;        //slow down to make speed time longer
        SysTickSetReload( mSpeed );
    }
    else if( mSpeed > mTargetSpeed)        //if it is headed in the correct direction and speed time is greater than targeted
    {
        mSpeed -= MOTOR_SPEED_RAMP;        //speed up by making time shorter
        SysTickSetReload( mSpeed );
    }
    else
    {
        mSpeed = mTargetSpeed;             //if at speed, make target speed equal to current/previous speed
    }

    if( mCW && MOTOR_SPEED_0 != mSpeed )   //if clockwise and motor speed is not 0
    {
        if( 0 == j )                    //if counter is 0 (0th half step in CW direction)
        {
            cycle = 0;                  //0 out cycle
            P4OUT = BIT3;               //and reset output pins
            j++;                        //increment counter
        }

        if( (j%2) && (j < 7) )          //1st 3rd and 5th half step in CW direction
        {
            cycle = P4OUT;              //cycle is used as a temporary variable
            P4OUT = P4OUT+(P4OUT >> 1); //Output plus the bit next to it turned on.
            j++;                        //increment counter
        }
        else if( !(j%2) && (0 != j) )   //2nd, 4th, and 6th half step in CW direction
        {
            P4OUT = P4OUT - cycle;      //subtracting the previous value of P2out from the wave
            j++;                        //increment counter
        }
        else
        {
            P4OUT = P4OUT + (P4OUT << 3);  //Final (7th) half step of half step in anti-clockwise direction
            j = 0;                      // 0 out counter
        }
    }
    else if( MOTOR_SPEED_0 != mSpeed )     //if counter clockwise and speed is not 0
    {
        if( 0 == j )                    //if counter is 0 (0th half step in CCW direction)
        {
            P4OUT = BIT0;               //reset output pins
            j++;                        //increment counter
            cycle = 0;                  //0 out cycle variable
        }

        if( (j%2) && (j < 7) )          //1st 3rd and 5th half step in CCW direction
        {
            cycle = P4OUT;              //cycle is used as a temporary variable
            P4OUT = P4OUT+(P4OUT << 1); //Output plus the bit next to it turned on.
            j++;                        //increment counter
        }
        else if( !(j%2) && (0 != j) )   //2nd, 4th, and 6th half step in CW direction
        {
            P4OUT = P4OUT - cycle;      //subtracting the previous value of P2out from the wave
            j++;                        //increment counter
        }
        else
        {
            P4OUT = P4OUT + (P4OUT >> 3);  //Final half step (7th) of half step in anti-clockwise direction
            j = 0;                      // 0 out counter
        }
    }

}
