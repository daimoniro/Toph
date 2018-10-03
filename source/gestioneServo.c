/*
 * gestioneServo.c
 *
 *  Created on: 30 Aug 2018
 *      Author: pi
 */



#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pigpio.h>
#include <math.h>

#include "gestioneServo.h"
#include "pin_raspberry.h"
#include "debug.h"


#define	PCA6585_ADDR 			0x40
#define SERVOMIN  150 // this is the 'minimum' pulse length count (out of 4096)
#define SERVOMAX  600 // this is the 'maximum' pulse length count (out of 4096)

//--------------------------------------------------
// variabili globali
//--------------------------------------------------
int i2cHandle_pca6585 = -1;
uint8_t servonum = 0;
unsigned short freqPWMServo = 50;

unsigned short posServo0 = 0;
unsigned short posServo1 = 0;

int errorServo = 0;
//--------------------------------------------------
// variabili extern
//--------------------------------------------------
extern char debugSTR[];

//--------------------------------------------------
// Function declaration
//--------------------------------------------------
void *gestioneServo();
void pca6585_init();
void PCA9685_reset(void);
void PCA9685_setPWMFreq(float freq);
void PCA9685_setPWM(uint8_t num, uint16_t on, uint16_t off);
void loopPWMServo();
void setServoPulse(uint8_t n, double pulse);

//-----------------------------------------------------------------------------
//	StartGestioneServo
//-----------------------------------------------------------------------------
void StartGestioneServo()
{
    // Demone Configurazione Board
    pthread_t ThCapture;

    pthread_create(&ThCapture, NULL, &gestioneServo, NULL);

}

//--------------------------------------------------
// initI2C_Servo
//--------------------------------------------------
void initI2C_Servo()
{
	i2cHandle_pca6585 = i2cOpen(1,PCA6585_ADDR,0);


	if(i2cHandle_pca6585 >= 0)
	{
		sprintf(debugSTR,"I2C Servo i2cHandle_pca6585: %d",i2cHandle_pca6585);
		TRACE4(1,"SERVO",VERDE,NERO_BG,debugSTR,0);

		pca6585_init();
	}
	else
	{
		sprintf(debugSTR,"Errore I2C Servo i2cHandle_pca6585: %d",i2cHandle_pca6585);
		TRACE4(1,"SERVO",ROSSO,NERO_BG,debugSTR,0);

		return;
	}
}


//--------------------------------------------------
// gestioneServo
//--------------------------------------------------
void *gestioneServo()
{
	int probeByte = 0;

	usleep(100000L);


	initI2C_Servo();



	while(1)
	{
		usleep(100000);

		probeByte = i2cReadByteData(i2cHandle_pca6585,LED0_ON_L + 0);

		if(probeByte < 0)
		{
			//printf("probeByte Gyro: %d\n",probeByte);
			errorServo++;
			continue;
		}
		errorServo = 0;


		posServo0 = i2cReadWordData(i2cHandle_pca6585,LED0_ON_L+4*0 + 2); //leggo solo off dato chemetto on a 0
		posServo1 = i2cReadWordData(i2cHandle_pca6585,LED0_ON_L+4*1 + 2); //leggo solo off dato chemetto on a 0

		//printf("%d %d %d %d\n",i2cReadByteData(i2cHandle_pca6585,LED0_ON_L + 0),i2cReadByteData(i2cHandle_pca6585,LED0_ON_L+ 1),i2cReadByteData(i2cHandle_pca6585,LED0_ON_L+ 2),i2cReadByteData(i2cHandle_pca6585,LED0_ON_L + 3));
	//	printf("posServo0: %d\n",posServo0);

		//loopPWMServo();
	}
}

//--------------------------------------------------
// pca6585_init
//--------------------------------------------------
void pca6585_init()
{
	PCA9685_reset();

	PCA9685_setPWMFreq(freqPWMServo) ;
}

//--------------------------------------------------
// PCA9685_reset
//--------------------------------------------------
void PCA9685_reset(void)
{
  i2cWriteByteData(i2cHandle_pca6585, PCA9685_MODE1,0x80);

  usleep(10000);
}

//--------------------------------------------------
// PCA9685_setPWMFreq
//--------------------------------------------------
void PCA9685_setPWMFreq(float freq)
{

  float prescaleval = 25000000;

  freq *= 0.9;  // Correct for overshoot in the frequency setting (see issue #11).


  prescaleval /= 4096;
  prescaleval /= freq;
  prescaleval -= 1;

  uint8_t prescale = floor(prescaleval + 0.5);
  uint8_t oldmode = i2cReadByteData(i2cHandle_pca6585,PCA9685_MODE1);
  uint8_t newmode = (oldmode&0x7F) | 0x10; // sleep

  i2cWriteByteData(i2cHandle_pca6585,PCA9685_MODE1, newmode); // go to sleep
  i2cWriteByteData(i2cHandle_pca6585,PCA9685_PRESCALE, prescale); // set the prescaler
  i2cWriteByteData(i2cHandle_pca6585,PCA9685_MODE1, oldmode);
  usleep(5000);
  i2cWriteByteData(i2cHandle_pca6585,PCA9685_MODE1, oldmode | 0xa0);  //  This sets the MODE1 register to turn on auto increment.

}


/**************************************************************************/
/*!
    @brief  Sets the PWM output of one of the PCA9685 pins
    @param  num One of the PWM output pins, from 0 to 15
    @param  on At what point in the 4096-part cycle to turn the PWM output ON
    @param  off At what point in the 4096-part cycle to turn the PWM output OFF
*/
/**************************************************************************/
void PCA9685_setPWM(uint8_t num, uint16_t on, uint16_t off)
{
	i2cWriteWordData(i2cHandle_pca6585,LED0_ON_L+4*num + 0 , on);
	i2cWriteWordData(i2cHandle_pca6585,LED0_ON_L+4*num + 2, off);

}

//--------------------------------------------------
// setServoPulse
//--------------------------------------------------
// you can use this function if you'd like to set the pulse length in seconds
// e.g. setServoPulse(0, 0.001) is a ~1 millisecond pulse width. its not precise!
void setServoPulse(uint8_t n, double pulse)
{
  double pulselength;

  pulselength = 1000000;   // 1,000,000 us per second
  pulselength /= freqPWMServo;   // 60 Hz
 // Serial.print(pulselength); Serial.println(" us per period");
  printf("us per period: %f\n",pulselength);
  pulselength /= 4096;  // 12 bits of resolution
 // Serial.print(pulselength); Serial.println(" us per bit");
  printf("us per bit: %f\n",pulselength);
  pulse *= 1000000;  // convert to us
  pulse /= pulselength;

  PCA9685_setPWM(n, 0, pulse);
}

//--------------------------------------------------
// loopPWMServo
//--------------------------------------------------
void loopPWMServo()
{
  // Drive each servo one at a time

  for (uint16_t pulselen = SERVOMIN; pulselen < SERVOMAX; pulselen++)
  {
	  PCA9685_setPWM(servonum, 0, pulselen);
  }

  usleep(500000);
  for (uint16_t pulselen = SERVOMAX; pulselen > SERVOMIN; pulselen--)
  {
	  PCA9685_setPWM(servonum, 0, pulselen);
  }

  usleep(500000);

  servonum ++;
  if (servonum > 2)
	  servonum = 0;
}

//--------------------------------------------------
// setSpeedServo
//--------------------------------------------------
void setSpeedServo(unsigned short servoSpeed_0, unsigned short servoSpeed_1)
{
	 PCA9685_setPWM(0, 0, servoSpeed_0);
	 PCA9685_setPWM(1, 0, servoSpeed_1);
	 PCA9685_setPWM(2, 0, servoSpeed_0);
	 PCA9685_setPWM(3, 0, servoSpeed_1);
}

//--------------------------------------------------
// setFrequencyServo
//--------------------------------------------------
void setFrequencyServo(unsigned short servoFrequency)
{
	PCA9685_setPWMFreq((float)servoFrequency);
	freqPWMServo = servoFrequency;
}

//--------------------------------------------------
// elaborateServoData
//--------------------------------------------------
void elaborateServoData(char * buf)
{
	posServo0 = buf[0] + (buf[1]<<8);
	posServo1 = buf[4] + (buf[5]<<8);

}
