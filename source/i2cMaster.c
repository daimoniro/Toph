/*
 * i2cMaster.c
 *
 *  Created on: 3 Oct 2018
 *      Author: pi
 */
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pigpio.h>
#include <math.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>

#include "compass.h"
#include "gyroAccelerometer.h"
#include "gestioneServo.h"
#include "pin_raspberry.h"
#include "panTiltServo.h"
#include "debug.h"


//-----------------------------------------------------------------------------
//	Variabili globali
//-----------------------------------------------------------------------------
char bufCompass[COUNT_BYTE_I2C_COMPASS];
char bufServo[COUNT_BYTE_I2C_SERVO];
char bufGyro[COUNT_BYTE_I2C_GYRO];
char bufPanTilt[COUNT_BYTE_I2C_PANTILT];

//-----------------------------------------------------------------------------
//	variabili estern
//-----------------------------------------------------------------------------
extern int alreadyClose;
extern int i2cHandleHMC5883l;
//extern int i2cHandle_pca6585;
extern int i2cHandleMPU6050;
extern int i2cHandle_pantilt;

extern int errorServo;
extern int errorGyro;
extern int errorCompass;
extern int errorPanTilt;
//-----------------------------------------------------------------------------
//	StartGestioneDeviceI2C
//-----------------------------------------------------------------------------
void *gestioneDevicesI2C();



//-----------------------------------------------------------------------------
//	StartGestioneDeviceI2C
//-----------------------------------------------------------------------------
void StartGestioneDeviceI2C()
{
    // Demone Configurazione Board
    pthread_t ThCapture;

    pthread_create(&ThCapture, NULL, &gestioneDevicesI2C, NULL);

}



//--------------------------------------------------
// gestioneDevicesI2C
//--------------------------------------------------
void *gestioneDevicesI2C()
{
	int countLettureI2C = 0;

	int returnFunz =0 ;


	int FREQUENZA_LETTURA_GYRO = 1;
	int FREQUENZA_LETTURA_COMPASS = 5;
//	int FREQUENZA_LETTURA_SERVO = 0;
	int FREQUENZA_LETTURA_PANTILT = 10;

	initI2C_GyroAccelerometer();
	initI2C_Compass();
	//initI2C_Servo();
	initI2C_PanTiltServo();

	while(1)
	{
		usleep(100000);

		if(alreadyClose == 1)
			break;

		//-----------------------------------------------------------------------------------
		if(FREQUENZA_LETTURA_GYRO)
		{
			if((countLettureI2C % FREQUENZA_LETTURA_GYRO) == 0)
			{
				TRACE4(2,"GYRO",BIANCO,NERO_BG,"Lettura GYRO",0);

				memset(bufGyro,0,COUNT_BYTE_I2C_GYRO);
				returnFunz = i2cReadI2CBlockData(i2cHandleMPU6050,START_REG_I2C_GYRO,bufGyro,COUNT_BYTE_I2C_GYRO);

				if(returnFunz > 0)
				{
					elaborateGyroData(bufGyro);
				}
				else
				{
					TRACE4(2,"GYRO",ROSSO,NERO_BG,"Errore Lettura GYRO",0);
					errorGyro++;
				}
			}
		}
		//-----------------------------------------------------------------------------------


		//-----------------------------------------------------------------------------------
		if(FREQUENZA_LETTURA_COMPASS)
		{
			if((countLettureI2C % FREQUENZA_LETTURA_COMPASS) == 0)
			{
				TRACE4(2,"COMPASS",BIANCO,NERO_BG,"Lettura COMPASS",0);
				memset(bufCompass,0,COUNT_BYTE_I2C_COMPASS);
				returnFunz = i2cReadI2CBlockData(i2cHandleHMC5883l,START_REG_I2C_COMPASS,bufCompass,COUNT_BYTE_I2C_COMPASS);

				if(returnFunz > 0)
				{
					elaborateCompassData(bufCompass);
				}
				else
				{
					TRACE4(2,"COMPASS",ROSSO,NERO_BG,"Errore Lettura COMPASS",0);
					errorCompass++;
				}
			}
		}
		//-----------------------------------------------------------------------------------

		//-----------------------------------------------------------------------------------
		/*if(FREQUENZA_LETTURA_SERVO)
		{
			if((countLettureI2C % FREQUENZA_LETTURA_SERVO) == 0)
			{
				TRACE4(2,"SERVO",BIANCO,NERO_BG,"Lettura SERVO",0);
				memset(bufServo,0,COUNT_BYTE_I2C_SERVO);
				returnFunz = i2cReadI2CBlockData(i2cHandle_pca6585,START_REG_I2C_SERVO,bufServo,COUNT_BYTE_I2C_SERVO);

				if(returnFunz > 0)
				{
					elaborateServoData(bufServo);
				}
				else
				{
					TRACE4(2,"SERVO",ROSSO,NERO_BG,"Errore Lettura SERVO",0);
					errorServo++;
				}
			}
		}*/
		//-----------------------------------------------------------------------------------

		//-----------------------------------------------------------------------------------
		if(FREQUENZA_LETTURA_PANTILT)
		{
			if((countLettureI2C % FREQUENZA_LETTURA_PANTILT) == 0)
			{
				TRACE4(2,"PANTILT",BIANCO,NERO_BG,"Lettura PanTilt",0);
				memset(bufPanTilt,0,COUNT_BYTE_I2C_PANTILT);
				returnFunz = i2cReadI2CBlockData(i2cHandle_pantilt,START_REG_I2C_PANTILT,bufPanTilt,COUNT_BYTE_I2C_PANTILT);

				if(returnFunz > 0)
				{
					elaboratePanTilt(bufPanTilt);
				}
				else
				{
					TRACE4(2,"PANTILT",ROSSO,NERO_BG,"Errore Lettura PANTILT",0);
					errorPanTilt++;
				}
			}
		}
		//-----------------------------------------------------------------------------------


		countLettureI2C++;

		if(countLettureI2C >= 20000)
			countLettureI2C = 0;

	}

	return NULL;
}




