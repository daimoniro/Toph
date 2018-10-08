/*
 * udp.c
 *
 *  Created on: 15 Aug 2018
 *      Author: pi
 */




#include <sys/types.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <memory.h>
#include <ifaddrs.h>
#include <net/if.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>

 #include <pthread.h>

//*****************************************************************************
//	function declaration
//*****************************************************************************
void *UDPServerManagement(void *threadid);
int udpOutcomingClientUDP(unsigned char *bufferOut,unsigned char lenght);
int sendMainValueUDP();




//*****************************************************************************
//	Variable globali
//*****************************************************************************
char ipUDPDestination[32];


//*****************************************************************************
//	Extern Variable
//*****************************************************************************

extern float distanceSonar_0;
extern float distanceSonar_1;
extern float distanceSonar_2;
extern float tempDS18D20;
extern float tempDHT22;
extern float humDHT22;
extern int velMotoreDC_0;
extern int velMotoreDC_1;

extern float gyro_xout_scaled;
extern float gyro_yout_scaled;
extern float gyro_zout_scaled;

extern float accel_xout_scaled;
extern float accel_yout_scaled;
extern float accel_zout_scaled;

extern float x_rotation;
extern float y_rotation;

extern float compass_xout_scaled;
extern float compass_yout_scaled;
extern float compass_zout_scaled;

extern unsigned short posServo0;
extern unsigned short posServo1;
extern float bearing;
extern short posServoPantilt_1;
extern short posServoPantilt_2;

//-----------------------------------------------------------------------------
//	StartUDPClientManagement
//-----------------------------------------------------------------------------
void StartUDPClientManagement()
{
    // Demone Configurazione Board
    pthread_t ThClientUDP;

    // create tread
    pthread_create(&ThClientUDP, NULL, &UDPServerManagement, NULL);

} // StartUDPClientManagement


//-----------------------------------------------------------------------------
//	UDPServerManagement
//-----------------------------------------------------------------------------
void *UDPServerManagement(void *threadid)
{

	sprintf(ipUDPDestination,"192.168.2.20");


	while(1)
	{

		usleep(100000);
		sendMainValueUDP();

		//printf("send: %d\n",returnSend);
	}


}


//-----------------------------------------------------------------------------
//	sendMainValueUDP
//----------------------------------------------------------------------------
int sendMainValueUDP()
{
	 unsigned char bufferTx[80];


	memset(bufferTx,0,sizeof(bufferTx));
	bufferTx[0] = 0x23;

	memcpy(bufferTx + 1,&tempDS18D20,sizeof(float));
	memcpy(bufferTx + 5,&distanceSonar_0,sizeof(float));
	memcpy(bufferTx + 9,&distanceSonar_1,sizeof(float));
	memcpy(bufferTx + 13,&distanceSonar_2,sizeof(float));
	memcpy(bufferTx + 17,&tempDHT22,sizeof(float));
	memcpy(bufferTx + 21,&humDHT22,sizeof(float));

	bufferTx[25] = (unsigned char)velMotoreDC_0;
	bufferTx[26] = (unsigned char)velMotoreDC_1;

	memcpy(bufferTx + 27,&gyro_xout_scaled,sizeof(float));
	memcpy(bufferTx + 31,&gyro_yout_scaled,sizeof(float));
	memcpy(bufferTx + 35,&gyro_zout_scaled,sizeof(float));

	memcpy(bufferTx + 39,&accel_xout_scaled,sizeof(float));
	memcpy(bufferTx + 43,&accel_yout_scaled,sizeof(float));
	memcpy(bufferTx + 47,&accel_zout_scaled,sizeof(float));

	memcpy(bufferTx + 51,&x_rotation,sizeof(float));
	memcpy(bufferTx + 55,&y_rotation,sizeof(float));

	memcpy(bufferTx + 59,&compass_xout_scaled,sizeof(float));
	memcpy(bufferTx + 63,&compass_yout_scaled,sizeof(float));
	memcpy(bufferTx + 67,&compass_zout_scaled,sizeof(float));


	bufferTx[71] = (unsigned char)((posServoPantilt_1 >> 8)&0x00FF);
	bufferTx[72] = (unsigned char)((posServoPantilt_1 >> 0)&0x00FF);
	bufferTx[73] = (unsigned char)((posServoPantilt_2 >> 8)&0x00FF);
	bufferTx[74] = (unsigned char)((posServoPantilt_2 >> 0)&0x00FF);

	memcpy(bufferTx + 75,&bearing,sizeof(float));

	bufferTx[79] = 0x37;

	return udpOutcomingClientUDP(bufferTx,80);

}




//-----------------------------------------------------------------------------
//		udpOutcomingClientUDP
//-----------------------------------------------------------------------------
int udpOutcomingClientUDP(unsigned char *bufferOut,unsigned char lenght)
{
   int sockfd;
   struct sockaddr_in servaddr;



   int t = 0;

   int returnValue = -1;

   sockfd=socket(AF_INET,SOCK_DGRAM,0);


   for(t = 0; t < 8;t++)
   {
        servaddr.sin_zero[t]= 0;
   }
   servaddr.sin_family = AF_INET;
   servaddr.sin_addr.s_addr=inet_addr(ipUDPDestination);
   servaddr.sin_port=htons(7001);

   //fcntl(sockfd, F_SETFL, O_NONBLOCK);
   //ioctlsocket(sockfd,FIONBIO,O_NDELAY);

   returnValue = sendto(sockfd,(char *) bufferOut, lenght, 0,(struct sockaddr *)&servaddr,sizeof(servaddr));


   close(sockfd);

   return returnValue;
}


