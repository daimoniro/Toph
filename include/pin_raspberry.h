/*
 * pin_raspberry.h
 *
 *  Created on: 17 Aug 2018
 *      Author: pi
 */

#ifndef INCLUDE_PIN_RASPBERRY_H_
#define INCLUDE_PIN_RASPBERRY_H_



// io
#define PIN_LED_VERDE 		17
#define PIN_LED_ROSSO 		18
#define BUTTON_PIN		27


//temperature sensor
#define DHT_PIN		22
#define DS1820_PIN	4

// sonar
#define TRIG_0 23
#define ECHO_0 24
#define TRIG_1 25
#define ECHO_1 5
#define TRIG_2 19 //da provare
#define ECHO_2 20 //da provare


//motor DC
#define PIN_MOTOR_0_IN1 26
#define PIN_MOTOR_0_IN2 16
#define PIN_MOTOR_0_PWM 12

#define PIN_MOTOR_1_IN1 6
#define PIN_MOTOR_1_IN2 10
#define PIN_MOTOR_1_PWM 13

//motore stepper
#define PIN_MOTOR_STEPPER_IN1 7
#define PIN_MOTOR_STEPPER_IN2 8
#define PIN_MOTOR_STEPPER_IN3 9
#define PIN_MOTOR_STEPPER_IN4 11


#endif /* INCLUDE_PIN_RASPBERRY_H_ */
