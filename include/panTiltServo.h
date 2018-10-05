/*
 * panTiltServo.h
 *
 *  Created on: 5 Oct 2018
 *      Author: pi
 */

#ifndef INCLUDE_PANTILTSERVO_H_
#define INCLUDE_PANTILTSERVO_H_



#define	PANTILT_ADDR 			0x15

#define	REG_CONFIG  0x00
#define	REG_SERVO1  0x01
#define	REG_SERVO2  0x03
#define	REG_WS2812  0x05
#define	REG_UPDATE  0x4E
#define	UPDATE_WAIT  0.03

#define WS2812	1


#define COUNT_BYTE_I2C_PANTILT	4

#define		START_REG_I2C_PANTILT	REG_SERVO1

void initI2C_PanTiltServo();
void panTilt_atExit();
int panTilt_setServo(int index, int angle);
void elaboratePanTilt(char * buf);

#endif /* INCLUDE_PANTILTSERVO_H_ */
