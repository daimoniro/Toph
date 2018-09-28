/*
 * serverTCP.h
 *
 *  Created on: 15 Aug 2018
 *      Author: pi
 */

#ifndef INCLUDE_SERVERTCP_H_
#define INCLUDE_SERVERTCP_H_



#define MAXCONN 5 // Definisce il numero massimo di connessioni da lasciare in coda


#define	SET_OUTPUT					1
#define SET_SPEED_MOTORDC			2
#define SET_PWM_FREQUENCY_MOTORDC	3

void StartTCPServerManagement(void);



#endif /* INCLUDE_SERVERTCP_H_ */
