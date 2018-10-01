/*
 * tempHumSensor.c
 *
 *  Created on: 16 Aug 2018
 *      Author: pi
 */

#include <pigpio.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>

#include "debug.h"
#include "pin_raspberry.h"

#define MAX_TIMINGS	85



#define ID_SENDORE_INTERNO	0
#define ID_SENDORE_ESTERNO	1
#define ID_SENDORE_FERMENTATORE	2

#define FREQUENZA_LETTURA_TEMPERATURE	5
#define FREQUENZA_SCRITTURA_TEMPERATURE	60

//-----------------------------------------------------------------------------
//	variabili globali
//-----------------------------------------------------------------------------
int data[5] = { 0, 0, 0, 0, 0 };
static const char delim[] = "=";

int valoriTemperatura[3];
char pathDeviceTemperatureSensor[64];

float tempDS18D20 = 0;
float tempDHT22 = 0;
float humDHT22 = 0;

//*****************************************************************************
//	extern Variable
//*****************************************************************************
extern char debugSTR[];


//-----------------------------------------------------------------------------
//	function declaration
//-----------------------------------------------------------------------------
void read_dht_data();
void *TemperatureHumManagement();
int letturaTemperatura(int idSensore);

//-----------------------------------------------------------------------------
//	StartTemperatureHumManagement
//-----------------------------------------------------------------------------
void StartTemperatureHumManagement()
{
    // Demone Configurazione Board
    pthread_t ThCapture;

    pthread_create(&ThCapture, NULL, &TemperatureHumManagement, NULL);

}



//-----------------------------------------------------------------------------
//	TemperatureManagement
//-----------------------------------------------------------------------------
void *TemperatureHumManagement()
{
	time_t now;
	int temperatureLette = 0;


	int returnFunz = 0;


	system("sudo modprobe w1-gpio");
	system("sudo modprobe w1-therm");


	sprintf(pathDeviceTemperatureSensor,"/sys/bus/w1/devices/28-00000620f601/w1_slave");
	//sprintf(pathDeviceTemperatureSensor,"/sys/bus/w1/devices/%s/w1_slave",cfg.idTemperatureSensor);

	valoriTemperatura[0] = 0xFFFF;
	valoriTemperatura[1] = 0xFFFF;
	valoriTemperatura[2] = 0xFFFF;

	letturaTemperatura(ID_SENDORE_INTERNO);
	//letturaTemperatura(ID_SENDORE_ESTERNO);
	//letturaTemperatura(ID_SENDORE_FERMENTATORE);
//	scritturaValoriTemperatura();

	//--------------------
	// 		main loop
	//--------------------
	while(1)
	{
		usleep(100000);
		time(&now);


		if((now % FREQUENZA_LETTURA_TEMPERATURE) == 0)
		{

			if(temperatureLette == 0)
			{
				returnFunz = letturaTemperatura(ID_SENDORE_INTERNO);

				if(returnFunz >0 )
				{
					sprintf(debugSTR,"valoreTemperatura %d",valoriTemperatura[0]);
					TRACE4(1,"TEMPERATURE",BIANCO,NERO_BG,debugSTR,0);

					tempDS18D20 = (float)((short)valoriTemperatura[0])/((float)1000);
				}
				else
				{
					sprintf(debugSTR,"Errore lettura temperatura. path: %s",pathDeviceTemperatureSensor);
					TRACE4(1,"TEMPERATURE",NERO,ROSSO_BG,debugSTR,0);
				}




				temperatureLette = 1;
			}


			//read_dht_data();


		}
		else
		{
			temperatureLette = 0;
		}


	}

}




//---------------------------------------------------------------------------------------
// letturaTemperatura
//---------------------------------------------------------------------------------------
int letturaTemperatura(int idSensore)
{
	FILE * SensoreFile;
	char Line[256];
	char *svptr;						// puntatore aggiornato dalla funzione strtok_r
	char *s;

	int lineNumber = 0;
	int yesLetto = 0;


	switch(idSensore)
	{
		case ID_SENDORE_INTERNO:
			SensoreFile = fopen(pathDeviceTemperatureSensor,"r");
			break;
		/*case ID_SENDORE_ESTERNO:
			SensoreFile = fopen(FILE_SENSORE_1W_ESTERNO,"r");
			break;
		case ID_SENDORE_FERMENTATORE:
			SensoreFile = fopen(FILE_SENSORE_1W_FERMENTATORE,"r");
			break;*/
	}


	if(SensoreFile==NULL)
	{
		//printf("Impossibile leggere file con ID %d\n", idSensore);

		valoriTemperatura[idSensore] = 0xFFFF;
		return -1;
	}

	while(fgets(Line,256,SensoreFile)!=NULL)
	{
		printf("Leggo %s",Line);

		//Leggo 15 00 4b 46 7f ff 0b 10 a4 : crc=a4 YES
		//Leggo 15 00 4b 46 7f ff 0b 10 a4 t=1312

		if((Line[0]!='#')&&(Line[0]!='\0')&&(Line[0]!='\n'))
		{


			if(lineNumber == 0)
			{
				if(strstr(Line,"YES") != NULL)
				{
					yesLetto = 1;
				}
			}

			if(lineNumber == 1)
			{
				if(yesLetto == 1)
				{
					//printf("YES letto\n");
					s = strtok_r(Line,delim,&svptr);
					s=strtok_r(NULL,delim,&svptr);
					//printf("Dopo = --> %d\n",atol(s));

					valoriTemperatura[idSensore] = atol(s);

				}
				else
				{
					valoriTemperatura[idSensore] = 0xFFFF;
				}


			}

			lineNumber++;
		}

	}

	fclose(SensoreFile);

	return 1;

}




void read_dht_data()
{
	uint8_t laststate	= 1;
	uint8_t counter		= 0;
	uint8_t j			= 0, i;

	data[0] = data[1] = data[2] = data[3] = data[4] = 0;

	/* pull pin down for 18 milliseconds */
	gpioSetMode( DHT_PIN, PI_OUTPUT );
	gpioWrite( DHT_PIN, 0 );
	gpioDelay( 18000 );

	/* prepare to read the pin */
	gpioSetMode( DHT_PIN, PI_INPUT );

	/* detect change and read data */
	for ( i = 0; i < MAX_TIMINGS; i++ )
	{
		counter = 0;
		while ( gpioRead( DHT_PIN ) == laststate )
		{
			counter++;
			gpioDelay( 1 );
			if ( counter == 255 )
			{
				break;
			}
		}
		laststate = gpioRead( DHT_PIN );

		if ( counter == 255 )
			break;

		/* ignore first 3 transitions */
		if ( (i >= 4) && (i % 2 == 0) )
		{
			/* shove each bit into the storage bytes */
			data[j / 8] <<= 1;
			if ( counter > 16 )
				data[j / 8] |= 1;
			j++;
		}
	}

	/*
	 * check we read 40 bits (8bit x 5 ) + verify checksum in the last byte
	 * print it out if data is good
	 */
	if ( (j >= 40) &&
	     (data[4] == ( (data[0] + data[1] + data[2] + data[3]) & 0xFF) ) )
	{
		float h = (float)((data[0] << 8) + data[1]) / 10;
		if ( h > 100 )
		{
			h = data[0];	// for DHT11
		}
		float c = (float)(((data[2] & 0x7F) << 8) + data[3]) / 10;
		if ( c > 125 )
		{
			c = data[2];	// for DHT11
		}
		if ( data[2] & 0x80 )
		{
			c = -c;
		}


		sprintf(debugSTR,"DHT22 --> Humidity = %.1f %% Temperature = %.1f *C\n", h, c);
		TRACE4(1,"TEMPERATURE",BIANCO,NERO_BG,debugSTR,0);

		tempDHT22 = c;
		humDHT22 = h;
	}
	else
	{
		//printf( "Data not good, skip\n" );
	}
}
