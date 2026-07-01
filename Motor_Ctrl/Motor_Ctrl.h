/*
 * Motor_Ctrl.h
 *
 *  Created on: Jun 19, 2026
 *      Author: Max Fenwick
 */

#ifndef MOTOR_CTRL_MOTOR_CTRL_H_
#define MOTOR_CTRL_MOTOR_CTRL_H_

#include <stdint.h>
#include <stdbool.h>


#define MOTOR_SPEED_4      3          //Motor speed definitions
#define MOTOR_SPEED_3      8
#define MOTOR_SPEED_2      13
#define MOTOR_SPEED_1      18
#define MOTOR_SPEED_0      53

#define MOTOR_SPEED_RAMP   1          //Motor ramp up speed by adjusting systick by this many ms per movement
                                      //while adjusting speeds

void Motor_Init(void);                //Initializing Motor Pins
void setMotorDirection( bool pCW );   //Set motor direction true for clockwise false for anti-clockwise
void setMotorSpeed( uint8_t pSpeed ); //Set Speed 0 through 4, 0 not moving, 4 fastest
void rampSpeed(void);
void MotorMove(void);



#endif /* MOTOR_CTRL_MOTOR_CTRL_H_ */
