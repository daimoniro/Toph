/*
 * gestioneMotoriStepper.c
 *
 *  Created on: 28 Aug 2018
 *      Author: pi
 */


#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pigpio.h>

#include "gestioneMotoriDC.h"
#include "pin_raspberry.h"
#include "debug.h"

#define	NUM_SEQ_STEPPER	8

//--------------------------------------------------
// variabili globali
//--------------------------------------------------

unsigned char halfstep_seq[NUM_SEQ_STEPPER][4] = {
   {1, 0, 0, 0} ,   /*  initializers for row indexed by 0 */
   {1, 1, 0, 0} ,   /*  initializers for row indexed by 1 */
   {0, 1, 0, 0} ,  /*  initializers for row indexed by 2 */
   {0, 1, 1, 0} ,
   {0, 0, 1, 0} ,
   {0, 0, 1, 1} ,
   {0, 0, 0, 1} ,
   {1, 0, 0, 1}
};


unsigned char runStepperMotor = 0;
unsigned int delayIntraStepStepperMotor =100000;
unsigned char directionStepperMotor = 0;
int stepsTodo = -1;
//--------------------------------------------------
// Function declaration
//--------------------------------------------------
void *gestioneMotoreStepper();
void initPin4MotoriStepper();
void doSingleStep(unsigned char v0,unsigned char v1,unsigned char v2,unsigned char v3);



//-----------------------------------------------------------------------------
//	StartGestioneMotoreStepper
//-----------------------------------------------------------------------------
void StartGestioneMotoreStepper()
{
    // Demone Configurazione Board
    pthread_t ThCapture;

    pthread_create(&ThCapture, NULL, &gestioneMotoreStepper, NULL);

}

//--------------------------------------------------
// initPin4MotoriStepper
//--------------------------------------------------
void initPin4MotoriStepper()
{

	//----------------- MOTOR Stepper -------------------
	gpioSetMode(PIN_MOTOR_STEPPER_IN1,PI_OUTPUT);
	gpioSetMode(PIN_MOTOR_STEPPER_IN2,PI_OUTPUT);
	gpioSetMode(PIN_MOTOR_STEPPER_IN3,PI_OUTPUT);
	gpioSetMode(PIN_MOTOR_STEPPER_IN4,PI_OUTPUT);
	//----------------------------------------------

}


//--------------------------------------------------
// gestioneMotoreStepper
//--------------------------------------------------
void *gestioneMotoreStepper()
{
	int countStep =0 ;

	initPin4MotoriStepper();

	while(1)
	{
		if(stepsTodo != -1)
		{
			stepsTodo--;

			if(stepsTodo == 0)
			{
				runStepperMotor = 0;
			}
		}

		if(runStepperMotor == 1)
		{
			doSingleStep(halfstep_seq[countStep][0],halfstep_seq[countStep][1],halfstep_seq[countStep][1],halfstep_seq[countStep][1]);
			usleep(delayIntraStepStepperMotor);

			if(directionStepperMotor == 0) //avanti
			{
				countStep++;
				if(countStep == NUM_SEQ_STEPPER)
					countStep = 0;
			}
			else //indietro
			{
				countStep--;

				if(countStep == -1)
					countStep = NUM_SEQ_STEPPER - 1;
			}
		}
		else
		{
			usleep(500000);
		}

	}
}

//--------------------------------------------------
// doSingleStep
//--------------------------------------------------
void doSingleStep(unsigned char v0,unsigned char v1,unsigned char v2,unsigned char v3)
{
	gpioWrite(PIN_MOTOR_STEPPER_IN1, v0);
	gpioWrite(PIN_MOTOR_STEPPER_IN2, v1);
	gpioWrite(PIN_MOTOR_STEPPER_IN3, v2);
	gpioWrite(PIN_MOTOR_STEPPER_IN4, v3);
}

//--------------------------------------------------
// sequenceStepMotor
//--------------------------------------------------
void sequenceStepMotor(unsigned char runStepMotor, unsigned char direction,unsigned int delay, int steps)
{
	runStepperMotor = runStepMotor;
	directionStepperMotor = direction;
	delayIntraStepStepperMotor = delay;
	stepsTodo = steps;
}

