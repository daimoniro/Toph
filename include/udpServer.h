/*
 * udpServer.h
 *
 *  Created on: 17 Aug 2018
 *      Author: pi
 */

#ifndef INCLUDE_UDPSERVER_H_
#define INCLUDE_UDPSERVER_H_

#define	SET_OUTPUT					1
#define SET_SPEED_MOTORDC			2
#define SET_PWM_FREQUENCY_MOTORDC	3
#define SET_PARAM_STEPPER_MOTOR		4
#define SET_SPEED_SERVO				5
#define SET_PWM_FREQUENCY_SERVO		6
#define	SET_PANTILT_SERVO			7
#define	OPEN_CAMERA_CONNECTION		8
#define	SET_UDPIPDESTINATION		9
#define	SET_CORR_VELDC_0			10

void StartUDPServerManagement();



#endif /* INCLUDE_UDPSERVER_H_ */
