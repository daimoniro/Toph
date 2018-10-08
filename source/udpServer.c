/*
 * udpServer.cpp
 *
 *  Created on: 17 Aug 2018
 *      Author: pi
 */




#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <pigpio.h>
#include "debug.h"
#include "gestioneMotoriDC.h"
#include "gestioneMotoriStepper.h"
#include "gestioneServo.h"
#include "panTiltServo.h"
#include "udpServer.h"
#include "camera.h"

#define BUFSIZE 64

unsigned char bufRx[256];


//*****************************************************************************
//	Extern Variable
//*****************************************************************************

extern char debugSTR[];
extern char ipUDPDestination[];
extern char correzioneVelMotoreDC_0;

void* UDPServer();


//-----------------------------------------------------------------------------
//	StartUDPServer_Management
//-----------------------------------------------------------------------------
void StartUDPServerManagement()
{
	pthread_t ThUDPServer;

	pthread_create(&ThUDPServer, NULL, UDPServer, NULL);

}

//-----------------------------------------------------------------------------
//	UDPServer
//-----------------------------------------------------------------------------
void* UDPServer()
{
	int sockfd;		/* socket */
	int portno;		/* port to listen on */
	int clientlen;		/* byte size of client's address */
	struct sockaddr_in serveraddr;	/* server's addr */
	struct sockaddr_in clientaddr;	/* client addr */

	int optval;		/* flag value for setsockopt */
	int n;			/* message byte size */

	int i = 0;

	char tmp[8];

	unsigned short bytedaricevere  = 0;
	unsigned short byteRicevuti  = 0;


	unsigned char runStepMotor = 0;
	unsigned char direction = 0;
	unsigned int delay = 0;
	int steps  = 0;

	short angle1 = 0;
	short angle2 = 0;

	portno = 12000;

	//socket: create the parent socket
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sockfd < 0)
		printf("ERROR opening socket\n");

	/* setsockopt: Handy debugging trick that lets
	 * us rerun the server immediately after we kill it;
	 * otherwise we have to wait about 20 secs.
	 * Eliminates "ERROR on binding: Address already in use" error.
	 */
	optval = 1;
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR,(const void *)&optval, sizeof(int));

	bzero((char *)&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons((unsigned short)portno);

	//bind: associate the parent socket with a port

	if (bind(sockfd, (struct sockaddr *)&serveraddr,sizeof(serveraddr)) < 0)
		printf("ERROR on binding\n");

	//main loop: wait for a datagram
	clientlen = sizeof(clientaddr);

	while (1)
	{
		byteRicevuti = 0;
		// recvfrom: receive a UDP datagram from a client
		n = recvfrom(sockfd, bufRx, BUFSIZE, 0,(struct sockaddr *)&clientaddr, (socklen_t *)&clientlen);

		if (n < 0)
		{
			printf("ERROR in recvfrom\n");
		}
		else
		{
			//printf("Arrived %d UDP bytes\n",n);


			byteRicevuti += n;

			bytedaricevere = bufRx[0]*256 + bufRx[1];

			if(byteRicevuti == (bytedaricevere + 2))
			{
				//printf("Pacchetto Corretto!\n");
			}
			else
			{


				sprintf(debugSTR,"Errore Pacchetto! byteRicevuti: %d bytedaricevere +2: %d",byteRicevuti,bytedaricevere + 2);
				TRACE4(1,"SERVER",ROSSO,NERO_BG,debugSTR,0);




				continue;
			}

			debugSTR[0] = 0;
			tmp[0] = 0;
			for(i = 0; i < byteRicevuti;i++)
			{
					sprintf(tmp,"[%.2X]",bufRx[i]);

					strcat(debugSTR,tmp);
			}
			TRACE4(1,"SERVER",BIANCO,NERO_BG,debugSTR,0);


			//00 0b 01 00 01 2d 00 19 00 4e 71 96 90



			switch(bufRx[2])
			{

				case SET_OUTPUT:

					sprintf(debugSTR,"SET_OUTPUT --> pin: %d state: %d",bufRx[3],bufRx[4]);
					TRACE4(1,"SERVER",VERDE,NERO_BG,debugSTR,0);

					gpioSetMode(bufRx[3], PI_OUTPUT);  // Set GPIO17 as input.
					gpioWrite(bufRx[3], bufRx[4]);

					break;

				case SET_SPEED_MOTORDC:
				   sprintf(debugSTR,"SET_SPEED_MOTORDC --> vel_0: %d dir_0: %d vel_1: %d dir_1: %d",bufRx[3],bufRx[4],bufRx[5],bufRx[6]);
				   TRACE4(1,"SERVER",VERDE,NERO_BG,debugSTR,0);
				   setSpeedMotorDC(bufRx[3], bufRx[4],bufRx[5], bufRx[6]);
				   break;

				case SET_PWM_FREQUENCY_MOTORDC:
				   sprintf(debugSTR,"SET_PWM_FREQUENCY_MOTORDC --> frequency: %d",bufRx[3] +(bufRx[4] <<8));
				   TRACE4(1,"SERVER",VERDE,NERO_BG,debugSTR,0);

				   setFrequencyPWMMotorDC(bufRx[3] +(bufRx[4] <<8));
				   break;

				case SET_PARAM_STEPPER_MOTOR:

					runStepMotor = bufRx[3];
					direction = bufRx[4];
					delay = bufRx[5] + (bufRx[6] <<8) +(bufRx[7] <<16) + (bufRx[8] <<24);
					steps  = bufRx[9] + (bufRx[10] <<8) +(bufRx[11] <<16) + (bufRx[12] <<24);

					sprintf(debugSTR,"SET_PARAM_STEPPER_MOTOR --> runStepMotor: %d direction: %d delay: %d steps: %d",runStepMotor,direction,delay,steps);
					TRACE4(1,"SERVER",VERDE,NERO_BG,debugSTR,0);

					sequenceStepMotor(runStepMotor,direction,delay,steps);

				   break;

				case SET_SPEED_SERVO:
				   sprintf(debugSTR,"SET_SPEED_SERVO --> vel_0: %d  vel_1: %d",bufRx[3] +(bufRx[4] <<8),bufRx[5] +(bufRx[6] <<8));
				   TRACE4(1,"SERVER",VERDE,NERO_BG,debugSTR,0);
				   setSpeedServo(bufRx[3] +(bufRx[4] <<8),bufRx[5] +(bufRx[6] <<8));
				   break;

				case SET_PWM_FREQUENCY_SERVO:
				   sprintf(debugSTR,"SET_PWM_FREQUENCY_SERVO --> frequency: %d",bufRx[3] +(bufRx[4] <<8));
				   TRACE4(1,"SERVER",VERDE,NERO_BG,debugSTR,0);
				   setFrequencyServo(bufRx[3] +(bufRx[4] <<8));


				   break;


				case SET_PANTILT_SERVO:

					 angle1 = (short)(bufRx[3] +(bufRx[4] <<8));
					 angle2 = (short)(bufRx[5] +(bufRx[6] <<8));

					sprintf(debugSTR,"SET_PANTILT_SERVO --> angle1: %d angle1: %d",angle1,angle2);
					TRACE4(1,"SERVER",VERDE,NERO_BG,debugSTR,0);

					panTilt_setServo(0,angle1);
					panTilt_setServo(1,angle2);
					break;

				case OPEN_CAMERA_CONNECTION:

					sprintf(debugSTR,"OPEN_CAMERA_CONNECTION");
					TRACE4(1,"SERVER",VERDE,NERO_BG,debugSTR,0);

					StartCameraConnection();

					break;


				case SET_UDPIPDESTINATION:


					 sprintf(ipUDPDestination,"%d.%d.%d.%d",bufRx[3],bufRx[4],bufRx[5],bufRx[6]);

					 sprintf(debugSTR,"SET_UDPIPDESTINATION --> %s",ipUDPDestination);
					 TRACE4(1,"SERVER",VERDE,NERO_BG,debugSTR,0);

					 break;


				case SET_CORR_VELDC_0:
					correzioneVelMotoreDC_0 = (char)bufRx[3];

					sprintf(debugSTR,"SET_CORR_VELDC_0 --> %d",correzioneVelMotoreDC_0);
					TRACE4(1,"SERVER",VERDE,NERO_BG,debugSTR,0);
					break;



				default:

					sprintf(debugSTR,"Comando Server SCONOSCIUTO ---> bufRx[2] = %d",bufRx[2]);
					TRACE4(1,"SERVER",ROSSO,NERO_BG,debugSTR,0);
					break;

			}
		}

	}
}


