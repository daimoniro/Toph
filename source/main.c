/*
 * main.c
 *
 *  Created on: 14 Aug 2018
 *      Author: pi
 */


#include <stdio.h>
#include <time.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <pigpio.h>

#include "debug.h"
#include "gestioneIO.h"
#include "udp.h"
#include "serverTCP.h"
#include "version.h"
#include "distance.h"
#include "tempHumSensor.h"
#include "gestioneMotoriDC.h"
#include "gestioneMotoriStepper.h"
#include "udpServer.h"
#include "pin_raspberry.h"
#include "gyroAccelerometer.h"
#include "compass.h"
#include "gestioneServo.h"
#include "i2cMaster.h"
#include "panTiltServo.h"

//--------------------------------------------------
// variabili globali
//--------------------------------------------------
char debugSTR[256];
int alreadyClose =0 ;
//--------------------------------------------------
// variabili extern
//--------------------------------------------------
extern int i2cHandleMPU6050;
extern int i2cHandleHMC5883l;
extern int i2cHandle_pca6585;
extern int i2cHandle_pantilt;
//--------------------------------------------------
// Function declaration
//--------------------------------------------------
void sig_handler(int signo);
void closePigpioLybrary();

//--------------------------------------------------
// main
//--------------------------------------------------
int main(int argc, char **argv)
{

	printf("\033[H\033[2J");

	printf("---------------------------------\n");
	printf("---------  Toph  ver: %s ------\n",VERSION);
	printf("---------------------------------\n\n");

	if (gpioInitialise() < 0)
	{
	   TRACE4(1,"MAIN",ROSSO,NERO_BG,"Setup pigpio failed ",0);
	   return -1;
	}
	else
	{
		 TRACE4(1,"MAIN",BIANCO,NERO_BG,"Setup pigpio OK ",0);
	}

	gpioSetSignalFunc(SIGINT, sig_handler);


	StartUDPServerManagement();
	StartUDPClientManagement();

	StartGestioneIO();

	StartTemperatureHumManagement();
	StartDistanceSonarManagement();
	StartGestioneMotoriDC();


	//StartGestioneGyroAccelerometer();
	//StartGestioneCompass();
	//StartGestioneServo();
	 StartGestioneDeviceI2C();


	while(1)
	{
		sleep(2);
	}

	return 0;
}

//--------------------------------------------------
// sig_handler
//--------------------------------------------------
void sig_handler(int signo)
{
	 closePigpioLybrary();
	 exit(0);

}

//--------------------------------------------------
// closePigpioLybrary
//--------------------------------------------------
void closePigpioLybrary()
{

	if(alreadyClose == 1)
	{
		 TRACE4(1,"MAIN",ROSSO,NERO_BG,"Gia' chiuso !!!",0);
		return ;
	}

	alreadyClose = 1;

	gpioWrite(PIN_LED_VERDE, 0);   //led off
	gpioWrite(PIN_LED_ROSSO, 0);   //led off

	gpioPWM(PIN_MOTOR_0_PWM,0);
	gpioPWM(PIN_MOTOR_1_PWM,0);


	panTilt_atExit();

	//printf("FINE: i2cHandleMPU6050: %d i2cHandleHMC5883l: %d i2cHandle_pca6585: %d\n",i2cHandleMPU6050,i2cHandleHMC5883l,i2cHandle_pca6585);

	if(i2cHandleMPU6050 > 0)
	 i2cClose(i2cHandleMPU6050);

	if(i2cHandleHMC5883l > 0)
	 i2cClose(i2cHandleHMC5883l);

	if(i2cHandle_pca6585 > 0)
	 i2cClose(i2cHandle_pca6585);

	if(i2cHandle_pantilt > 0)
	 i2cClose(i2cHandle_pantilt);


	sleep(1);

	gpioTerminate();
}



