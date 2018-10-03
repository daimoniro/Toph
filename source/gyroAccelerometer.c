/*
 * gyroAccelerometer.c
 *
 *  Created on: 28 Aug 2018
 *      Author: pi
 */



#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pigpio.h>
#include <math.h>

#include "gyroAccelerometer.h"
#include "pin_raspberry.h"
#include "debug.h"

#define	MPU_6050_ADDR 			0x68


//--------------------------------------------------
// variabili globali
//--------------------------------------------------
int i2cHandleMPU6050 = -1;
float gyro_xout_scaled = 0;
float gyro_yout_scaled = 0;
float gyro_zout_scaled = 0;

float accel_xout_scaled = 0;
float accel_yout_scaled = 0;
float accel_zout_scaled = 0;

float x_rotation = 0;
float y_rotation = 0;


int errorGyro = 0;

//--------------------------------------------------
// variabili extern
//--------------------------------------------------
extern char debugSTR[];

//--------------------------------------------------
// Function declaration
//--------------------------------------------------
void *gestioneGyroAccelerometer();
float dist(float a, float b);
float get_x_rotation(float x, float y, float z );
float get_y_rotation(float x, float y, float z );
int i2cReadWordData_2c(int handle,int i2creg);
void mpu6050_init();
static int8_t mpu6050_readByte(uint8_t regAddr);

//-----------------------------------------------------------------------------
//	StartGestioneGyroAccelerometer
//-----------------------------------------------------------------------------
void StartGestioneGyroAccelerometer()
{
    // Demone Configurazione Board
    pthread_t ThCapture;

    pthread_create(&ThCapture, NULL, &gestioneGyroAccelerometer, NULL);

}

//--------------------------------------------------
// initI2C_GyroAccelerometer
//--------------------------------------------------
void initI2C_GyroAccelerometer()
{
	i2cHandleMPU6050 = i2cOpen(1,MPU_6050_ADDR,0);


	if(i2cHandleMPU6050 >= 0)
	{
		sprintf(debugSTR,"I2C Gyro i2cHandleMPU6050: %d",i2cHandleMPU6050);
		TRACE4(1,"GYRO",VERDE,NERO_BG,debugSTR,0);

		mpu6050_init();
	}
	else
	{
		sprintf(debugSTR,"Errore I2C Gyro i2cHandleMPU6050: %d",i2cHandleMPU6050);
		TRACE4(1,"GYRO",ROSSO,NERO_BG,debugSTR,0);

		return ;
	}

}


//--------------------------------------------------
// gestioneGyroAccelerometer
//--------------------------------------------------
void *gestioneGyroAccelerometer()
{
	int gyro_xout = 0;
	int gyro_yout = 0;
	int gyro_zout = 0;

	int accel_xout = 0;
	int accel_yout = 0;
	int accel_zout = 0;
	int probeByte = 0;

	usleep(100000L);

	initI2C_GyroAccelerometer();


	while(1)
	{
		usleep(10000);

		/*accel_xout = (((int16_t)mpu6050_readByte(MPU6050_RA_ACCEL_XOUT_H)) << 8) | mpu6050_readByte(MPU6050_RA_ACCEL_XOUT_L);
		accel_yout = (((int16_t)mpu6050_readByte(MPU6050_RA_ACCEL_YOUT_H)) << 8) | mpu6050_readByte(MPU6050_RA_ACCEL_YOUT_L);
		accel_zout = (((int16_t)mpu6050_readByte(MPU6050_RA_ACCEL_ZOUT_H)) << 8) | mpu6050_readByte(MPU6050_RA_ACCEL_ZOUT_L);
*/

		probeByte = mpu6050_readByte(MPU6050_RA_ACCEL_XOUT_H);

		if(probeByte < 0)
		{
			printf("probeByte Gyro: %d\n",probeByte);
			errorGyro++;
			continue;
		}
		errorGyro = 0;

		accel_xout = (short)((unsigned short)(probeByte << 8) + (unsigned short)mpu6050_readByte(MPU6050_RA_ACCEL_XOUT_L));
		accel_yout = (short)((unsigned short)(mpu6050_readByte(MPU6050_RA_ACCEL_YOUT_H) << 8) + (unsigned short)mpu6050_readByte(MPU6050_RA_ACCEL_YOUT_L));
		accel_zout = (short)((unsigned short)(mpu6050_readByte(MPU6050_RA_ACCEL_ZOUT_H) << 8) + (unsigned short)mpu6050_readByte(MPU6050_RA_ACCEL_ZOUT_L));


		gyro_xout = (short)((unsigned short)(mpu6050_readByte(MPU6050_RA_GYRO_XOUT_H) << 8) + (unsigned short)mpu6050_readByte(MPU6050_RA_GYRO_XOUT_L));
		gyro_yout = (short)((unsigned short)(mpu6050_readByte(MPU6050_RA_GYRO_YOUT_H) << 8) + (unsigned short)mpu6050_readByte(MPU6050_RA_GYRO_YOUT_L));
		gyro_zout = (short)((unsigned short)(mpu6050_readByte(MPU6050_RA_GYRO_ZOUT_H) << 8) + (unsigned short)mpu6050_readByte(MPU6050_RA_GYRO_ZOUT_L));



		gyro_xout_scaled = (float)gyro_xout / (float)MPU6050_GYRO_LSB_250;
		gyro_yout_scaled = (float)gyro_yout / (float)MPU6050_GYRO_LSB_250;
		gyro_zout_scaled = (float)gyro_zout / (float)MPU6050_GYRO_LSB_250;

		accel_xout_scaled = (float)accel_xout / (float)MPU6050_ACCEL_LSB_2;
		accel_yout_scaled = (float)accel_yout / (float)MPU6050_ACCEL_LSB_2;
		accel_zout_scaled = (float)accel_zout / (float)MPU6050_ACCEL_LSB_2;

		x_rotation = get_x_rotation(accel_xout_scaled,accel_yout_scaled,accel_zout_scaled);
		y_rotation = get_y_rotation(accel_xout_scaled,accel_yout_scaled,accel_zout_scaled);


		sprintf(debugSTR,"gyro_xout: %d scaled: %f",gyro_xout, gyro_xout_scaled);
		TRACE4(2,"GYRO",BIANCO,NERO_BG,debugSTR,0);
		sprintf(debugSTR,"gyro_yout: %d scaled: %f",gyro_yout, gyro_yout_scaled);
		TRACE4(2,"GYRO",BIANCO,NERO_BG,debugSTR,0);
		sprintf(debugSTR,"gyro_zout: %d scaled: %f",gyro_zout, gyro_zout_scaled);
		TRACE4(2,"GYRO",BIANCO,NERO_BG,debugSTR,0);

		sprintf(debugSTR,"accel_xout: %d scaled: %f",accel_xout, accel_xout_scaled);
		TRACE4(2,"GYRO",BIANCO,NERO_BG,debugSTR,0);
		sprintf(debugSTR,"accel_yout: %d scaled: %f",accel_yout, accel_yout_scaled);
		TRACE4(2,"GYRO",BIANCO,NERO_BG,debugSTR,0);
		sprintf(debugSTR,"accel_zout: %d scaled: %f",accel_zout, accel_zout_scaled);
		TRACE4(2,"GYRO",BIANCO,NERO_BG,debugSTR,0);

		sprintf(debugSTR,"x_rotation: %f",x_rotation);
		TRACE4(2,"GYRO",BIANCO,NERO_BG,debugSTR,0);
		sprintf(debugSTR,"y_rotation: %f",y_rotation);
		TRACE4(2,"GYRO",BIANCO,NERO_BG,debugSTR,0);

	}
}

//--------------------------------------------------
// dist
//--------------------------------------------------
float dist(float a, float b)
{
	return sqrt(a*a + b*b);
}

//--------------------------------------------------
// get_x_rotation
//--------------------------------------------------
float get_x_rotation(float x, float y, float z )
{
	float radians = atan2(y,dist(x,z));

	return radians * 180.0f / 3.14159265359f;

}

//--------------------------------------------------
// get_y_rotation
//--------------------------------------------------
float get_y_rotation(float x, float y, float z )
{
	float radians = atan2(x,dist(y,z));

	return radians * 180.0f / 3.14159265359f;

}


int i2cReadWordData_2c(int handle,int i2creg)
{
	int val = i2cReadWordData(handle,i2creg);
	if (val >= 0x8000)
	   return -((65535 - val) + 1);
	   else
	return val;
}


static void mpu6050_writeByte(uint8_t regAddr, uint8_t data)
{
  i2cWriteByteData(i2cHandleMPU6050, regAddr,data);
}

static int8_t mpu6050_readByte(uint8_t regAddr)
{
    return i2cReadByteData(i2cHandleMPU6050, regAddr);
}

static void mpu6050_writeBit(uint8_t regAddr, uint8_t bitNum, uint8_t data)
{
    uint8_t b;
    b = mpu6050_readByte(regAddr);
    b = (data != 0) ? (b | (1 << bitNum)) : (b & ~(1 << bitNum));
    mpu6050_writeByte(regAddr, b);
}


static void mpu6050_setSleepDisabled()
{
	mpu6050_writeBit(MPU6050_RA_PWR_MGMT_1, MPU6050_PWR1_SLEEP_BIT, 0);
}

static void mpu6050_writeBits(uint8_t regAddr, uint8_t bitStart, uint8_t length, uint8_t data)
{
    //      010 value to write
    // 76543210 bit numbers
    //    xxx   args: bitStart=4, length=3
    // 00011100 mask byte
    // 10101111 original value (sample)
    // 10100011 original & ~mask
    // 10101011 masked | value
	if(length > 0)
	{
		uint8_t b = 0;

		b = mpu6050_readByte(regAddr);
		//get current data
		uint8_t mask = ((1 << length) - 1) << (bitStart - length + 1);
		data <<= (bitStart - length + 1); // shift data into correct position
		data &= mask; // zero all non-important bits in data
		b &= ~(mask); // zero all important bits in existing byte
		b |= data; // combine data with existing byte
		mpu6050_writeByte(regAddr, b);

	}
}


void mpu6050_init()
{


	//allow mpu6050 chip clocks to start up
	usleep(100000L);

	//set sleep disabled
	mpu6050_setSleepDisabled();

	//wake up delay needed sleep disabled
	usleep(10000L);

	//set clock source
	//  it is highly recommended that the device be configured to use one of the gyroscopes
	// (or an external clock source) as the clock reference for improved stability
	mpu6050_writeBits(MPU6050_RA_PWR_MGMT_1, MPU6050_PWR1_CLKSEL_BIT, MPU6050_PWR1_CLKSEL_LENGTH, MPU6050_CLOCK_PLL_XGYRO);
	//set DLPF bandwidth to 42Hz
	mpu6050_writeBits(MPU6050_RA_CONFIG, MPU6050_CFG_DLPF_CFG_BIT, MPU6050_CFG_DLPF_CFG_LENGTH, MPU6050_DLPF_BW_42);
    //set sampe rate
	mpu6050_writeByte(MPU6050_RA_SMPLRT_DIV, 4); //1khz / (1 + 4) = 200Hz
	//set gyro range
	mpu6050_writeBits(MPU6050_RA_GYRO_CONFIG, MPU6050_GCONFIG_FS_SEL_BIT, MPU6050_GCONFIG_FS_SEL_LENGTH, MPU6050_GYRO_FS);
	//set accel range
	mpu6050_writeBits(MPU6050_RA_ACCEL_CONFIG, MPU6050_ACONFIG_AFS_SEL_BIT, MPU6050_ACONFIG_AFS_SEL_LENGTH, MPU6050_ACCEL_FS);

	//enable i2c bypass by default for sensors connected to aux i2c bus
	mpu6050_writeBit(MPU6050_RA_INT_PIN_CFG, MPU6050_INTCFG_I2C_BYPASS_EN_BIT, (1 << MPU6050_INTCFG_I2C_BYPASS_EN_BIT));
	// disable master mode on the i2c aux bus
	mpu6050_writeBit(MPU6050_RA_USER_CTRL, 5, 0);

}


//--------------------------------------------------
// elaborateGyroData
//--------------------------------------------------
void elaborateGyroData(char * buf)
{
	int gyro_xout = 0;
	int gyro_yout = 0;
	int gyro_zout = 0;

	int accel_xout = 0;
	int accel_yout = 0;
	int accel_zout = 0;

	accel_xout = (short)((unsigned short)(buf[0] << 8) + (unsigned short)buf[1]);
	accel_yout = (short)((unsigned short)(buf[2] << 8) + (unsigned short)buf[3]);
	accel_zout = (short)((unsigned short)(buf[4] << 8) + (unsigned short)buf[5]);


//	gyro_xout = (short)((unsigned short)(mpu6050_readByte(MPU6050_RA_GYRO_XOUT_H) << 8) + (unsigned short)mpu6050_readByte(MPU6050_RA_GYRO_XOUT_L));
//	gyro_yout = (short)((unsigned short)(mpu6050_readByte(MPU6050_RA_GYRO_YOUT_H) << 8) + (unsigned short)mpu6050_readByte(MPU6050_RA_GYRO_YOUT_L));
//	gyro_zout = (short)((unsigned short)(mpu6050_readByte(MPU6050_RA_GYRO_ZOUT_H) << 8) + (unsigned short)mpu6050_readByte(MPU6050_RA_GYRO_ZOUT_L));

	gyro_xout = (short)((unsigned short)(buf[8] << 8) + (unsigned short)buf[9]);
	gyro_yout = (short)((unsigned short)(buf[10] << 8) + (unsigned short)buf[11]);
	gyro_zout = (short)((unsigned short)(buf[12] << 8) + (unsigned short)buf[13]);


	gyro_xout_scaled = (float)gyro_xout / (float)MPU6050_GYRO_LSB_250;
	gyro_yout_scaled = (float)gyro_yout / (float)MPU6050_GYRO_LSB_250;
	gyro_zout_scaled = (float)gyro_zout / (float)MPU6050_GYRO_LSB_250;

	accel_xout_scaled = (float)accel_xout / (float)MPU6050_ACCEL_LSB_2;
	accel_yout_scaled = (float)accel_yout / (float)MPU6050_ACCEL_LSB_2;
	accel_zout_scaled = (float)accel_zout / (float)MPU6050_ACCEL_LSB_2;

	x_rotation = get_x_rotation(accel_xout_scaled,accel_yout_scaled,accel_zout_scaled);
	y_rotation = get_y_rotation(accel_xout_scaled,accel_yout_scaled,accel_zout_scaled);


	sprintf(debugSTR,"gyro_xout: %d scaled: %f",gyro_xout, gyro_xout_scaled);
	TRACE4(2,"GYRO",BIANCO,NERO_BG,debugSTR,0);
	sprintf(debugSTR,"gyro_yout: %d scaled: %f",gyro_yout, gyro_yout_scaled);
	TRACE4(2,"GYRO",BIANCO,NERO_BG,debugSTR,0);
	sprintf(debugSTR,"gyro_zout: %d scaled: %f",gyro_zout, gyro_zout_scaled);
	TRACE4(2,"GYRO",BIANCO,NERO_BG,debugSTR,0);

	sprintf(debugSTR,"accel_xout: %d scaled: %f",accel_xout, accel_xout_scaled);
	TRACE4(2,"GYRO",BIANCO,NERO_BG,debugSTR,0);
	sprintf(debugSTR,"accel_yout: %d scaled: %f",accel_yout, accel_yout_scaled);
	TRACE4(2,"GYRO",BIANCO,NERO_BG,debugSTR,0);
	sprintf(debugSTR,"accel_zout: %d scaled: %f",accel_zout, accel_zout_scaled);
	TRACE4(2,"GYRO",BIANCO,NERO_BG,debugSTR,0);

	sprintf(debugSTR,"x_rotation: %f",x_rotation);
	TRACE4(2,"GYRO",BIANCO,NERO_BG,debugSTR,0);
	sprintf(debugSTR,"y_rotation: %f",y_rotation);
	TRACE4(2,"GYRO",BIANCO,NERO_BG,debugSTR,0);


}
