/*
 * compass.c
 *
 *  Created on: 29 Aug 2018
 *      Author: pi
 */


#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pigpio.h>
#include <math.h>

#include <stdlib.h>



#include "compass.h"
#include "pin_raspberry.h"
#include "debug.h"


#define	HMC5883l_ADDR 			0x0d
#define PI 3.14159265


//--------------------------------------------------
// variabili globali
//--------------------------------------------------
int i2cHandleHMC5883l = -1;


float compass_xout_scaled = 0;
float compass_yout_scaled = 0;
float compass_zout_scaled = 0;
float bearing = 0;

int errorCompass = 0;
float SCALE_COMPASS = 1;
//--------------------------------------------------
// variabili extern
//--------------------------------------------------
extern char debugSTR[];

//--------------------------------------------------
// Function declaration
//--------------------------------------------------
void *gestioneCompass();
void hmc5883l_init();
int init_i2c();
//float azimuth(uint16_t a, uint16_t b);
float azimuth(short a, short b);
//-----------------------------------------------------------------------------
//	StartGestioneGyroAccelerometer
//-----------------------------------------------------------------------------
void StartGestioneCompass()
{
    // Demone Configurazione Board
    pthread_t ThCapture;

    pthread_create(&ThCapture, NULL, &gestioneCompass, NULL);

}

//--------------------------------------------------
// initI2C_Compass
//--------------------------------------------------
void initI2C_Compass()
{

	i2cHandleHMC5883l = i2cOpen(1,HMC5883l_ADDR,0);


	if(i2cHandleHMC5883l >= 0)
	{
		sprintf(debugSTR,"I2C Compass initI2C_HMC5883l: %d",i2cHandleHMC5883l);
		TRACE4(1,"COMPASS",VERDE,NERO_BG,debugSTR,0);

		hmc5883l_init();
	}
	else
	{
		sprintf(debugSTR,"Errore I2C Compass initI2C_HMC5883l: %d",i2cHandleHMC5883l);
		TRACE4(1,"COMPASS",ROSSO,NERO_BG,debugSTR,0);

		return;
	}
}


//--------------------------------------------------
// gestioneCompass
//--------------------------------------------------
void *gestioneCompass()
{
	char buf[6];
	int compass_xout = 0;
	int compass_yout = 0;
	int compass_zout = 0;



	int probeByte = 0;



	usleep(100000L);


	initI2C_Compass();


	while(1)
	{
		usleep(100000);

		unsigned char statusRegister  = i2cReadByteData(i2cHandleHMC5883l,6);

		if (((statusRegister >> 0)&0x01) == 0)
		{

			probeByte = i2cReadI2CBlockData(i2cHandleHMC5883l,0,buf,6);
			printf("probeByte: %d\n",probeByte);
			continue;
		}


		probeByte = i2cReadByteData(i2cHandleHMC5883l,1);

		if(probeByte < 0)
		{
			printf("probeByte: %d\n",probeByte);

			errorCompass++;
			continue;
		}
		errorCompass = 0;



		compass_xout = (short)((unsigned short)(buf[1] << 8) + (unsigned short)buf[0]);
		compass_yout = (short)((unsigned short)(buf[3] << 8) + (unsigned short)buf[2]);
		compass_zout = (short)((unsigned short)(buf[5] << 8) + (unsigned short)buf[4]);



		sprintf(debugSTR,"compass_xout: %d", compass_xout);
		TRACE4(3,"COMPASS",BIANCO,NERO_BG,debugSTR,0);
		sprintf(debugSTR,"compass_yout: %d", compass_yout);
		TRACE4(3,"COMPASS",BIANCO,NERO_BG,debugSTR,0);
		sprintf(debugSTR,"compass_zout: %d", compass_zout);
		TRACE4(3,"COMPASS",BIANCO,NERO_BG,debugSTR,0);

		compass_xout_scaled = (float)compass_xout / (float)SCALE_COMPASS;
		compass_yout_scaled = (float)compass_yout / (float)SCALE_COMPASS;
		compass_zout_scaled = (float)compass_zout / (float)SCALE_COMPASS;


		bearing = azimuth(compass_yout,compass_xout);

		sprintf(debugSTR,"bearing: %f --> %d", bearing,(int)bearing);
		TRACE4(2,"COMPASS",BIANCO,NERO_BG,debugSTR,0);
	}
}


float azimuth(short a, short b)
{
  float azimuth = atan2((float)a,(float)b) * 180.0/PI;
  return azimuth < 0?360 + azimuth:azimuth;
  //return azimuth ;
}

//--------------------------------------------------
// hmc5883l_init
//--------------------------------------------------
void hmc5883l_init()
{

	//# Set to 8 samples @ 15Hz
	/*i2cWriteByteData(i2cHandleHMC5883l, 0,0b01110000);

	//1.3 gain LSb / Gauss 1090 (default)
	i2cWriteByteData(i2cHandleHMC5883l, 1,0b00100000);

	//Continuous sampling
	i2cWriteByteData(i2cHandleHMC5883l, 2,0b00000000);*/


	//soft reset
	i2cWriteByteData(i2cHandleHMC5883l,0x0A,0x80);

	usleep(100000);

	//Define Set/Reset period
	i2cWriteByteData(i2cHandleHMC5883l, 0x0B,0x01);

	usleep(100000);

	i2cWriteByteData(i2cHandleHMC5883l,0x09,Mode_Continuous|ODR_200Hz|RNG_8G|OSR_512);

	/*
	  Define
	  OSR = 512
	  Full Scale Range = 8G(Gauss)
	  ODR = 200HZ
	  set continuous measurement mode
	  */
}

//--------------------------------------------------
// elaborateCompassData
//--------------------------------------------------
void elaborateCompassData(char * buf)
{
	int compass_xout = 0;
	int compass_yout = 0;
	int compass_zout = 0;

	compass_xout = (short)((unsigned short)(buf[1] << 8) + (unsigned short)buf[0]);
	compass_yout = (short)((unsigned short)(buf[3] << 8) + (unsigned short)buf[2]);
	compass_zout = (short)((unsigned short)(buf[5] << 8) + (unsigned short)buf[4]);

	sprintf(debugSTR,"compass_xout: %d", compass_xout);
	TRACE4(3,"COMPASS",BIANCO,NERO_BG,debugSTR,0);
	sprintf(debugSTR,"compass_yout: %d", compass_yout);
	TRACE4(3,"COMPASS",BIANCO,NERO_BG,debugSTR,0);
	sprintf(debugSTR,"compass_zout: %d", compass_zout);
	TRACE4(3,"COMPASS",BIANCO,NERO_BG,debugSTR,0);

	compass_xout_scaled = (float)compass_xout / (float)SCALE_COMPASS;
	compass_yout_scaled = (float)compass_yout / (float)SCALE_COMPASS;
	compass_zout_scaled = (float)compass_zout / (float)SCALE_COMPASS;


	bearing = azimuth(compass_yout,compass_xout);

	sprintf(debugSTR,"bearing: %f --> %d", bearing,(int)bearing);
	TRACE4(2,"COMPASS",BIANCO,NERO_BG,debugSTR,0);
}


