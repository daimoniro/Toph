/*
 * gestioneIO.c
 *
 *  Created on: 14 Aug 2018
 *      Author: pi
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pigpio.h>

#include "debug.h"
#include "pin_raspberry.h"
#include "main.h"

#define		SOGLIA_ERRORI		50
//--------------------------------------------------
// variabili extern
//--------------------------------------------------
extern char debugSTR[];

extern int errorServo;
extern int errorGyro;
extern int errorCompass;
extern int alreadyClose;
//--------------------------------------------------
// Function declaration
//--------------------------------------------------
void *gestioneIO();



//-----------------------------------------------------------------------------
//	StartGestioneIO
//-----------------------------------------------------------------------------
void StartGestioneIO()
{
    // Demone Configurazione Board
    pthread_t ThCapture;

    pthread_create(&ThCapture, NULL, &gestioneIO, NULL);

}



//--------------------------------------------------
// gestioneIO
//--------------------------------------------------
void *gestioneIO()
{
	int statusLedRosso = 0;
	int oldStatusLedRosso = -1;

	gpioSetMode(PIN_LED_VERDE, PI_OUTPUT);
	gpioSetMode(PIN_LED_ROSSO, PI_OUTPUT);
	gpioSetMode(BUTTON_PIN, PI_INPUT);

	gpioSetPullUpDown(BUTTON_PIN, PI_PUD_UP);   // Sets a pull-up.

	gpioWrite(PIN_LED_VERDE, 1);   //led on
	gpioWrite(PIN_LED_ROSSO, 0);   //led off

	while(1)
	{
		usleep(100000);

		if (alreadyClose == 1)
			break;

		sprintf(debugSTR,"Stato PIN %d --> %d", BUTTON_PIN,gpioRead(BUTTON_PIN));
		TRACE4(1,"IO",BIANCO,NERO_BG,debugSTR,0);

		if(gpioRead(BUTTON_PIN) == 0)
		{
			closePigpioLybrary();
			exit(0);
		}


		if((errorServo > SOGLIA_ERRORI) || (errorGyro > SOGLIA_ERRORI) || (errorCompass > SOGLIA_ERRORI))
		{
			statusLedRosso = 1;
			if(statusLedRosso != oldStatusLedRosso)
			{
				sprintf(debugSTR,"Stato LED Rosso --> %d", statusLedRosso);
				TRACE4(1,"IO",BIANCO,NERO_BG,debugSTR,0);

				gpioWrite(PIN_LED_ROSSO, 1);   //led on
			}
		}
		else
		{	statusLedRosso = 0;
			if(statusLedRosso != oldStatusLedRosso)
			{
				sprintf(debugSTR,"Stato LED Rosso --> %d", statusLedRosso);
				TRACE4(1,"IO",BIANCO,NERO_BG,debugSTR,0);

				gpioWrite(PIN_LED_ROSSO, 0);   //led off

			}
		}

		sprintf(debugSTR,"errorServo: %d errorGyro: %d errorCompass: %d", errorServo,errorGyro,errorCompass);
		TRACE4(2,"IO",BIANCO,NERO_BG,debugSTR,0);


		oldStatusLedRosso = statusLedRosso;


	}

	return NULL;
}



