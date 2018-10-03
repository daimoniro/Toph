/*
 * gestioneMotoriDC.c
 *
 *  Created on: 16 Aug 2018
 *      Author: pi
 */




#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pigpio.h>

#include "gestioneMotoriDC.h"
#include "pin_raspberry.h"

void initPin4MotoriDC();
void *gestioneMotoriDC();



//-----------------------------------------------------------------------------
//	Variabili globali
//-----------------------------------------------------------------------------
int velMotoreDC_0 = 0;
int velMotoreDC_1 = 0;

//-----------------------------------------------------------------------------
//	variabili estern
//-----------------------------------------------------------------------------
extern int alreadyClose;


//-----------------------------------------------------------------------------
//	StartGestioneMotoriDC
//-----------------------------------------------------------------------------
void StartGestioneMotoriDC()
{
    // Demone Configurazione Board
    pthread_t ThCapture;

    pthread_create(&ThCapture, NULL, &gestioneMotoriDC, NULL);

}

//--------------------------------------------------
// initPin4MotoriDC
//--------------------------------------------------
void *gestioneMotoriDC()
{

	initPin4MotoriDC();

	while(1)
	{
		usleep(100000);

		if(alreadyClose == 1)
			break;

		velMotoreDC_0 = gpioGetPWMdutycycle(PIN_MOTOR_0_PWM);

		velMotoreDC_1 = gpioGetPWMdutycycle(PIN_MOTOR_1_PWM);
	}


	return NULL;
}



//--------------------------------------------------
// initPin4MotoriDC
//--------------------------------------------------
void initPin4MotoriDC()
{

	//----------------- MOTOR 0 -------------------
	gpioSetMode(PIN_MOTOR_0_IN1,PI_OUTPUT);
	gpioSetMode(PIN_MOTOR_0_IN2,PI_OUTPUT);
	gpioSetMode(PIN_MOTOR_0_PWM,PI_ALT0);


	gpioWrite(PIN_MOTOR_0_IN1,1);
	gpioWrite(PIN_MOTOR_0_IN2,0);
	gpioSetPWMfrequency(PIN_MOTOR_0_PWM,500);
	gpioSetPWMrange(PIN_MOTOR_0_PWM, 100);
	gpioPWM(PIN_MOTOR_0_PWM,0);

	//----------------------------------------------

	//----------------- MOTOR 0 -------------------
	gpioSetMode(PIN_MOTOR_1_IN1,PI_OUTPUT);
	gpioSetMode(PIN_MOTOR_1_IN2,PI_OUTPUT);
	gpioSetMode(PIN_MOTOR_1_PWM,PI_ALT0);


	gpioWrite(PIN_MOTOR_1_IN1,1);
	gpioWrite(PIN_MOTOR_1_IN2,0);

	gpioSetPWMfrequency(PIN_MOTOR_1_PWM,500);
	gpioSetPWMrange(PIN_MOTOR_1_PWM, 100);
	gpioPWM(PIN_MOTOR_1_PWM,0);


	//----------------------------------------------

}




void setSpeedMotorDC(unsigned char vel_0, unsigned char dir_0,unsigned char vel_1, unsigned char dir_1)
{
	gpioPWM(PIN_MOTOR_0_PWM, vel_0);

	if(dir_0 == 1)
	{
		gpioWrite(PIN_MOTOR_0_IN1,1);
		gpioWrite(PIN_MOTOR_0_IN2,0);
	}
	else
	{
		gpioWrite(PIN_MOTOR_0_IN1,0);
		gpioWrite(PIN_MOTOR_0_IN2,1);
	}


	gpioPWM(PIN_MOTOR_1_PWM, vel_1);

	if(dir_1 == 1)
	{
		gpioWrite(PIN_MOTOR_1_IN1,1);
		gpioWrite(PIN_MOTOR_1_IN2,0);
	}
	else
	{
		gpioWrite(PIN_MOTOR_1_IN1,0);
		gpioWrite(PIN_MOTOR_1_IN2,1);
	}
}

void setFrequencyPWMMotorDC(unsigned short frequency)
{
	gpioSetPWMfrequency(PIN_MOTOR_0_PWM,frequency);
	gpioSetPWMfrequency(PIN_MOTOR_1_PWM,frequency);

}
