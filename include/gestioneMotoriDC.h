/*
 * gestioneMotoriDC.h
 *
 *  Created on: 16 Aug 2018
 *      Author: pi
 */

#ifndef INCLUDE_GESTIONEMOTORIDC_H_
#define INCLUDE_GESTIONEMOTORIDC_H_




void StartGestioneMotoriDC();

void setSpeedMotorDC(unsigned char vel_0, unsigned char dir_0,unsigned char vel_1, unsigned char dir_1);
void setFrequencyPWMMotorDC(unsigned short frequency);

#endif /* INCLUDE_GESTIONEMOTORIDC_H_ */
