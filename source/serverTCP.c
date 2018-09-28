/*
 * serverTCP.c
 *
 *  Created on: 15 Aug 2018
 *      Author: pi
 */





//*****************************************************************************
//	Include
//*****************************************************************************
#include <pthread.h>
#include <stdio.h>
#include <semaphore.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>

#include <sys/ioctl.h>
#include <net/if.h>
#include <pigpio.h>

#include "debug.h"
#include "serverTCP.h"
#include "gestioneMotoriDC.h"

//*****************************************************************************
//	Global Variable
//*****************************************************************************
// Identificatori del canale pubblico e del canale privato
int sd_server4Engine;
int sd_client4Engine;

// Indirizzo del Server
struct sockaddr_in server_addr;
// Indirizzo del client
struct sockaddr_in client_addr;

in_addr_t myIPHex;

// Dimensione dell’indirizzo client
socklen_t client_len=sizeof(client_addr);

unsigned char bufRx[256];





//*****************************************************************************
//	Extern Variable
//*****************************************************************************

extern char debugSTR[];



//*****************************************************************************
//	Function declaration
//*****************************************************************************
void *TCPServerManagement();
void InitTCPServer();

//*****************************************************************************
//	Function
//*****************************************************************************

//-----------------------------------------------------------------------------
//	StartTCPServerManagement
//-----------------------------------------------------------------------------

void StartTCPServerManagement()
{
    // Demone Configurazione Board
    pthread_t ThServer;


    InitTCPServer();


    // create tread
    pthread_create(&ThServer, NULL, &TCPServerManagement, NULL);

} // StartServerManagement



//-----------------------------------------------------------------------------
//	addr_initialize_tcp
//-----------------------------------------------------------------------------
void addr_initialize_tcp(struct sockaddr_in *indirizzo, int port, long IPaddr)
{
   indirizzo->sin_family = AF_INET;
   indirizzo->sin_port = htons((u_short) port);
            // htons: host to network conversion, short
   indirizzo->sin_addr.s_addr = IPaddr;
}



//-----------------------------------------------------------------------------
//	InitServer()
//-----------------------------------------------------------------------------
void InitTCPServer()
{

    int yes = 1;
    int returnFunz = 0;
    struct ifreq ifr;
    char strIP[40];



    //addr_initialize_tcp(&server_addr,cfg.listenTCPServer_port,INADDR_ANY);
    addr_initialize_tcp(&server_addr,8000,INADDR_ANY);


    // Crea il canale pubblico
    sd_server4Engine = socket(AF_INET,SOCK_STREAM,0);
    ioctl(sd_server4Engine, SIOCGIFADDR, &ifr);

    if(sd_server4Engine < 0)
    {
        sprintf(debugSTR,"InitServer: sd_server4Engine %d (errno: %d)", sd_server4Engine,errno);
        TRACE4(1,"SERVER",BIANCO,NERO_BG,debugSTR,0);
    }

    //impostazione socket  bloccante
    // fcntl(sd_server4Engine,F_SETFL,O_NONBLOCK);
    returnFunz = setsockopt(sd_server4Engine,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int)) ;

    if(returnFunz < 0)
    {
        sprintf(debugSTR,"InitServer: setsockopt %d (errno: %d)", returnFunz,errno);
        TRACE4(1,"SERVER",ROSSO,NERO_BG,debugSTR,0);
    }

    do
    {
        // Lega al socket l’indirizzo del server
        returnFunz = bind(sd_server4Engine,(struct sockaddr*) &server_addr,sizeof(server_addr));

        if(returnFunz < 0)
        {
            sprintf(debugSTR,"InitServer: bind %d (errno: %d)", returnFunz,errno);
            TRACE4(1,"SERVER",ROSSO,NERO_BG,debugSTR,0);
        }
    }
    while(returnFunz < 0);




    // Imposta il numero Massimo di connessioni in coda
    returnFunz = listen(sd_server4Engine,16);

    if(returnFunz < 0)
    {
        sprintf(debugSTR,"InitServer: listen %d (errno: %d)", returnFunz,errno);
        TRACE4(1,"SERVER",ROSSO,NERO_BG,debugSTR,0);
    }

    sprintf(debugSTR,"Server --> Mi pongo in attesa sul mio port: %d", ntohs(server_addr.sin_port));
    TRACE4(1,"SERVER",VERDE,NERO_BG,debugSTR,0);

    sprintf(strIP,"%s", inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));

    sprintf(debugSTR,"InitServer MY IP:%s", strIP);
    TRACE4(1,"SERVER",VERDE,NERO_BG,debugSTR,0);


}



//-----------------------------------------------------------------------------
//	TCPServerManagement
//-----------------------------------------------------------------------------
void *TCPServerManagement()
{

    int return_recv = 0;
    int return_send = 0;
    int i = 0;
    unsigned char lenPayload = 0;
    char tmp[8];

    unsigned short bytedaricevere  = 0;
    unsigned short byteRicevuti  = 0;
    char bufTx_server[32];




    TRACE4(1,"SERVER",BIANCO,NERO_BG,"Start TCPServerManagement",0);


    for(;;)
    {

            usleep(10000);



            byteRicevuti = 0;
            bytedaricevere  = 0;

            //TRACE4(1,"SERVER",BIANCO,NERO_BG,"Server in ascolto....",0);


            sd_client4Engine = accept(sd_server4Engine,(struct sockaddr*) &client_addr, &client_len);


        /*	sprintf(debugSTR,"sd_server4Engine: %d",sd_server4Engine);
            TRACE4(1,"SERVER",BIANCO,NERO_BG,debugSTR,0);
            sprintf(debugSTR,"sd_client4Engine: %d",sd_client4Engine);
            TRACE4(1,"SERVER",BIANCO,NERO_BG,debugSTR,0);*/

            if(sd_client4Engine >=0)
            {

                // Se la accept() si è sbloccata significa che si è connesso un client

                //sprintf(debugSTR,"Ho accettato una connessione dal client con port: %d", ntohs(client_addr.sin_port));
                //TRACE4(1,"SERVER",BIANCO,NERO_BG,debugSTR,0);


                return_recv = recv(sd_client4Engine,bufRx,2,0);
                byteRicevuti += return_recv;

                bytedaricevere = bufRx[0]*256 + bufRx[1];


                return_recv = recv(sd_client4Engine,bufRx + 2,bytedaricevere,0);
                byteRicevuti += return_recv;

                if(byteRicevuti == (bytedaricevere + 2))
                {
                    //printf("Pacchetto Corretto!\n");
                }
                else
                {


                    sprintf(debugSTR,"Errore Pacchetto! byteRicevuti: %d bytedaricevere +2: %d",byteRicevuti,bytedaricevere + 2);
                    TRACE4(1,"SERVER",ROSSO,NERO_BG,debugSTR,0);



                    bufTx_server[0] = 0x00;
                    send(sd_client4Engine,bufTx_server,1,MSG_NOSIGNAL);
                    close(sd_client4Engine);

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

                bufTx_server[2] = bufRx[2];
                lenPayload = 1;

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

                    default:

                        sprintf(debugSTR,"Comando Server SCONOSCIUTO ---> bufRx[2] = %d",bufRx[2]);
                        TRACE4(1,"SERVER",ROSSO,NERO_BG,debugSTR,0);
                        break;

                }

                bufTx_server[0] = 0;
                bufTx_server[1] = lenPayload;

                //printf("lenPayload: %d\n",lenPayload);
                return_send = send(sd_client4Engine,bufTx_server,lenPayload + 2,MSG_NOSIGNAL);


                sprintf(debugSTR,"return_send %d",return_send);
                TRACE4(1,"SERVER",VERDE,NERO_BG,debugSTR,0);


                // Chiude il canale privato
                close(sd_client4Engine);
            }
            else
            {
                // Chiude il canale privato
                close(sd_client4Engine);



                TRACE4(1,"SERVER",ROSSO,NERO_BG,"Nessuna connessione",0);
            }





    }

    pthread_detach(pthread_self());
    pthread_exit(NULL);
    return NULL;

} // ServerManagement

