/*
 * compass.h
 *
 *  Created on: 29 Aug 2018
 *      Author: pi
 */

#ifndef INCLUDE_COMPASS_H_
#define INCLUDE_COMPASS_H_


void StartGestioneCompass();
void initI2C_Compass();
void elaborateCompassData(char * buf);


#define		START_REG_I2C_COMPASS	0
#define		COUNT_BYTE_I2C_COMPASS	6


//REG CONTROL

//0x09

#define Mode_Standby    0b00000000
#define Mode_Continuous 0b00000001

#define ODR_10Hz        0b00000000
#define ODR_50Hz        0b00000100
#define ODR_100Hz       0b00001000
#define ODR_200Hz       0b00001100

#define RNG_2G          0b00000000
#define RNG_8G          0b00010000

#define OSR_512         0b00000000
#define OSR_256         0b01000000
#define OSR_128         0b10000000
#define OSR_64          0b11000000


#endif /* INCLUDE_COMPASS_H_ */
