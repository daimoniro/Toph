/*
 * distance.c
 *
 *  Created on: 16 Aug 2018
 *      Author: pi
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pigpio.h>

#include "pin_raspberry.h"
#include "debug.h"

//*****************************************************************************
//	global Variable
//*****************************************************************************
float distanceSonar_0 = 0;
float distanceSonar_1 = 0;
float distanceSonar_2 = 0;


//*****************************************************************************
//	Extern Variable
//*****************************************************************************
extern char debugSTR[];
extern int alreadyClose;

//*****************************************************************************
//	dichiarazione funzioni
//*****************************************************************************
void *gestioneDistance();
void getDistance(unsigned char idSonarSendor);


//-----------------------------------------------------------------------------
//	StartTemperatureHumManagement
//-----------------------------------------------------------------------------
void StartDistanceSonarManagement()
{
    // Demone Configurazione Board
    pthread_t ThCapture;

    pthread_create(&ThCapture, NULL, &gestioneDistance, NULL);

}

//-----------------------------------------------------------------------------
//	gestioneDistance
//-----------------------------------------------------------------------------
void *gestioneDistance()
{


	gpioSetMode(TRIG_0, PI_OUTPUT);
	gpioSetMode(ECHO_0, PI_INPUT);

	gpioSetMode(TRIG_1, PI_OUTPUT);
	gpioSetMode(ECHO_1, PI_INPUT);

	gpioSetMode(TRIG_2, PI_OUTPUT);
	gpioSetMode(ECHO_2, PI_INPUT);



	gpioWrite(TRIG_0, 0);
	gpioWrite(TRIG_1, 0);
	gpioWrite(TRIG_2, 0);

    sleep(2);


    while (1)
    {

		if(alreadyClose == 1)
			break;

    	getDistance(0);
    	usleep(100000);

    	getDistance(1);
        usleep(100000);

        getDistance(2);
        usleep(100000);
    }

    return 0;
}


void getDistance(unsigned char idSonarSendor)
{

   struct timespec start, stop;
   double pulse_duration;
   double distance;
   long int count1 = 0;
   long int count2 = 0;
   int MAX_COUNT_DISTANCE = 30000;

   unsigned char localTrig = 0 ;
   unsigned char localEcho = 0 ;


   switch(idSonarSendor)
   {
   	   case 0:
   		   localTrig = TRIG_0 ;
   		   localEcho = ECHO_0 ;
   		   break;

   	   case 1:
   		   localTrig = TRIG_1 ;
   		   	localEcho = ECHO_1 ;
   	   		break;

   	 case 2:
   		 localTrig = TRIG_2 ;
   		 localEcho = ECHO_2 ;
   		 break;

   	   default:
   		  // printf("Errore idSensor sbagliato!!!\n");
   		   return;
   		   break;
   }



  gpioWrite(localTrig, 1);
  gpioDelay(10);
  gpioWrite(localTrig, 0);


 // printf("prima\n");
  while((gpioRead(localEcho) == 0) && (count1 < MAX_COUNT_DISTANCE))
  {

	  clock_gettime(CLOCK_REALTIME, &start);
	  count1 ++;

	 // printf("count: %d\n",count);
  }

//  printf("durante\n");
  count2 =0 ;
  while((gpioRead(localEcho) == 1) && (count2 < MAX_COUNT_DISTANCE))
  {
	  clock_gettime( CLOCK_REALTIME, &stop);
	  count2++;
  }
 // printf("dopo\n");

  if((count1 < MAX_COUNT_DISTANCE) && (count2 < MAX_COUNT_DISTANCE))
  {
	  stop.tv_nsec += (stop.tv_sec - start.tv_sec)*1000000000;

	  pulse_duration = ( stop.tv_nsec - start.tv_nsec );

	  //il pulse_duration è in ns quindi divido per 1000000 per arrivare ai ms
	  distance = pulse_duration/1000000 *(345/2)/10;
	 // printf("Sensor #%d --> pulse_duration: %f ms --> distance: %3.1f cm\n",idSonarSendor,pulse_duration/1000000,distance);

	   sprintf(debugSTR,"Sensor #%d --> pulse_duration: %f ms --> distance: %3.1f cm count1: %ld count2: %ld",idSonarSendor,pulse_duration/1000000,distance,count1,count2);
	   TRACE4(1,"SONAR",BIANCO,NERO_BG,debugSTR,0);

		switch(idSonarSendor)
		{
			case 0:
				distanceSonar_0 = distance;
				break;

			case 1:
				distanceSonar_1 = distance;
				break;

			case 2:
				distanceSonar_2 = distance;
				break;
	   }
  }
  else
  {
	  sprintf(debugSTR,"Sensor #%d --> ERRORE",idSonarSendor);
	   TRACE4(1,"SONAR",ROSSO,NERO_BG,debugSTR,0);

		/*switch(idSonarSendor)
		{
			case 0:
				distanceSonar_0 = -1;
				break;

			case 1:
				distanceSonar_1 = -1;
				break;

			case 2:
				distanceSonar_2 = -1;
				break;
	   }*/
  }

}
