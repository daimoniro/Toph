/*
 * gestioneMotoriStepper.h
 *
 *  Created on: 28 Aug 2018
 *      Author: pi
 */

#ifndef INCLUDE_GESTIONEMOTORISTEPPER_H_
#define INCLUDE_GESTIONEMOTORISTEPPER_H_


void StartGestioneMotoreStepper();
void sequenceStepMotor(unsigned char runStepMotor, unsigned char direction,unsigned int delay, int steps);

#endif /* INCLUDE_GESTIONEMOTORISTEPPER_H_ */
