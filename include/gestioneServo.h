/*
 * gestioneServo.h
 *
 *  Created on: 30 Aug 2018
 *      Author: pi
 */

#ifndef INCLUDE_GESTIONESERVO_H_
#define INCLUDE_GESTIONESERVO_H_


void StartGestioneServo();
void setSpeedServo(unsigned short servoSpeed_0, unsigned short servoSpeed_1);
void setFrequencyServo(unsigned short servoFrequency);

#define PCA9685_SUBADR1 0x2
#define PCA9685_SUBADR2 0x3
#define PCA9685_SUBADR3 0x4

#define PCA9685_MODE1 0x0
#define PCA9685_PRESCALE 0xFE

#define LED0_ON_L 0x6
#define LED0_ON_H 0x7
#define LED0_OFF_L 0x8
#define LED0_OFF_H 0x9

#define ALLLED_ON_L 0xFA
#define ALLLED_ON_H 0xFB
#define ALLLED_OFF_L 0xFC
#define ALLLED_OFF_H 0xFD



#endif /* INCLUDE_GESTIONESERVO_H_ */
