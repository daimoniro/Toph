/*
 * camera.cpp
 *
 *  Created on: 8 Oct 2018
 *      Author: pi
 */


#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <pthread.h>


#include "camera.h"
#include "debug.h"

//*****************************************************************************
//	Extern Variable
//*****************************************************************************
extern char debugSTR[];


void* cameraConnection();

//-----------------------------------------------------------------------------
//	StartCameraConnection
//-----------------------------------------------------------------------------
void StartCameraConnection()
{
	pthread_t ThUDPCamera;

	pthread_create(&ThUDPCamera, NULL, cameraConnection, NULL);

}


//-----------------------------------------------------------------------------
//	cameraConnection
//-----------------------------------------------------------------------------
void* cameraConnection()
{

	sprintf(debugSTR,"Prima di raspistill");
	TRACE4(1,"CAMERA",BIANCO,NERO_BG,debugSTR,0);

	system("sudo /usr/bin/raspistill -fps 20 -hf -vf -w 640 -h 480 -t 0 -l -o tcp://0.0.0.0:2222");


	sprintf(debugSTR,"Dopo raspistill");
	TRACE4(1,"CAMERA",BIANCO,NERO_BG,debugSTR,0);

	return NULL;
}

