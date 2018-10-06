/*
 * panTiltServo.c
 *
 *  Created on: 5 Oct 2018
 *      Author: pi
 */




#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pigpio.h>
#include <math.h>

#include "panTiltServo.h"
#include "pin_raspberry.h"
#include "debug.h"



//--------------------------------------------------
// variabili globali
//--------------------------------------------------
int i2cHandle_pantilt = -1;

int servo1_min=575;
int servo1_max=2325;
int servo2_min=575;
int servo2_max=2325;
int idle_timeout= 2;

unsigned char is_setup = 0;
unsigned char enable_servo1 = 0;
unsigned char enable_servo2 = 0;
unsigned char enable_lights = 0;
unsigned char light_mode = WS2812;
unsigned char light_on = 0;


short posServoPantilt_1 = 0;
short posServoPantilt_2 = 0;

//--------------------------------------------------
// variabili extern
//--------------------------------------------------
extern char debugSTR[];

//--------------------------------------------------
// Function declaration
//--------------------------------------------------
void panTilt_setup();
void panTilt_set_config();

//--------------------------------------------------
// initI2C_PanTiltServo
//--------------------------------------------------
void initI2C_PanTiltServo()
{
	i2cHandle_pantilt = i2cOpen(1,PANTILT_ADDR,0);


	if(i2cHandle_pantilt >= 0)
	{
		sprintf(debugSTR,"I2C Servo i2cHandle_pantilt: %d",i2cHandle_pantilt);
		TRACE4(1,"PANTILT",VERDE,NERO_BG,debugSTR,0);

		panTilt_setup();
	}
	else
	{
		sprintf(debugSTR,"Errore I2C Servo i2cHandle_pantilt: %d",i2cHandle_pantilt);
		TRACE4(1,"PANTILT",ROSSO,NERO_BG,debugSTR,0);

		return;
	}
}

//--------------------------------------------------
// panTilt_setup
//--------------------------------------------------
void panTilt_setup()
{

	if(is_setup == 1)
		return;

	enable_servo1 = 1;
	enable_servo2 = 1;
	panTilt_set_config();

	is_setup = 1;

}

//--------------------------------------------------
// panTilt_set_config
//--------------------------------------------------
// """Generate config value for PanTilt HAT and write to device."""
void panTilt_set_config()
{
	unsigned char config =0 ;
	config |= enable_servo1;
	config |= (enable_servo2 << 1);
	config |= (enable_lights << 2);
	config |= (light_mode    << 3);
	config |= (light_on      << 4);

	i2cWriteByteData(i2cHandle_pantilt, REG_CONFIG,config);
}


//--------------------------------------------------
// panTilt_atExit
//--------------------------------------------------
void panTilt_atExit()
{
	enable_servo1 = 0;
	enable_servo2 = 0;
	panTilt_set_config();
}


//--------------------------------------------------
// panTilt_idleTimeout
//--------------------------------------------------
void panTilt_idleTimeout(int value)
{
	idle_timeout = value;
}

//--------------------------------------------------
// panTilt_us_to_degrees
//--------------------------------------------------
/* """Converts pulse time in microseconds to degrees
:param us: Pulse time in microseconds
:param us_min: Minimum possible pulse time in microseconds
:param us_max: Maximum possible pulse time in microseconds*/
int panTilt_servo_us_to_degrees(int us, int us_min,int us_max)
{
	int servo_range = us_max - us_min;
	float angle = ((float)(us - us_min) / (float)(servo_range)) * 180.0;
	return (int)(round(angle)) - 90;
}


//--------------------------------------------------
// panTilt_servo_degrees_to_us
//--------------------------------------------------
// """Converts degrees into a servo pulse time in microseconds
int panTilt_servo_degrees_to_us(int angle, int us_min,int us_max)
{
	angle += 90;
	int servo_range = us_max - us_min;
	int us = (servo_range / 180.0) * angle;
	return us_min + (int)(us);
}


//--------------------------------------------------
// panTilt_servo_enable
//--------------------------------------------------
//Disabling a servo turns off the drive signal.
void panTilt_servo_enable(int index, int state)
{
	switch(index)
	{
		case 0:
			enable_servo1 = state;
			break;
		case 1:
			enable_servo2 = state;
			break;
	}

	panTilt_set_config();
}


//--------------------------------------------------
// panTilt_pulse_min
//--------------------------------------------------
//"""Set the minimum high pulse for a servo in microseconds.
void panTilt_pulse_min(int index, int value)
{
	switch(index)
	{
		case 0:
			servo1_min = value;
			break;
		case 1:
			servo2_min = value;
			break;
	}

}


//--------------------------------------------------
// panTilt_pulse_max
//--------------------------------------------------
//"""Set the maximum high pulse for a servo in microseconds.
void panTilt_pulse_max(int index, int value)
{
	switch(index)
	{
		case 0:
			servo1_max = value;
			break;
		case 1:
			servo2_max = value;
			break;
	}

}


//--------------------------------------------------
// panTilt_getServo
//--------------------------------------------------
//"""Get position of servo in degrees."""
int panTilt_getServo(int index)
{
	int us = 0;
	int returnValue = 0;

	switch(index)
	{
		case 0:
			us = i2cReadWordData(i2cHandle_pantilt,REG_SERVO1);
			returnValue = panTilt_servo_us_to_degrees(us,servo1_min,servo1_max);
			break;

		case 1:
			us = i2cReadWordData(i2cHandle_pantilt,REG_SERVO2);
			returnValue = panTilt_servo_us_to_degrees(us,servo2_min,servo2_max);
			break;
	}

	return returnValue;
}


//--------------------------------------------------
// panTilt_setServo
//--------------------------------------------------
//"""Set position of servo in degrees."""
int panTilt_setServo(int index, int angle)
{
	int us = 0;
	int returnValue = 0;

	switch(index)
	{
		case 0:
			us = panTilt_servo_degrees_to_us(angle,servo1_min,servo1_max);
			i2cWriteWordData(i2cHandle_pantilt,REG_SERVO1,us);
			break;

		case 1:
			us = panTilt_servo_degrees_to_us(angle,servo2_min,servo2_max);
			i2cWriteWordData(i2cHandle_pantilt,REG_SERVO2,us);
			break;
	}

	return returnValue;
}



//--------------------------------------------------
// elaboratePanTilt
//--------------------------------------------------
void elaboratePanTilt(char * buf)
{
	unsigned short us1 = buf[0] + (buf[1]<<8);
	unsigned short us2 = buf[2] + (buf[3]<<8);


	posServoPantilt_1 =  panTilt_servo_us_to_degrees(us1,servo1_min,servo1_max);
	posServoPantilt_2 =  panTilt_servo_us_to_degrees(us2,servo2_min,servo2_max);


}


