/******************************************************************************/
/*			Application: ....			              */
/******************************************************************************/
/*									      */
/*			 programme  SERVEUR 				      */
/*									      */
/******************************************************************************/
/*									      */
/*		Auteurs :  ....						      */
/*		Date :  ....						      */
/*									      */
/******************************************************************************/	

#include<stdio.h>
#include <curses.h>
#include <time.h>
#include<sys/signal.h>
#include<sys/wait.h>
#include<stdlib.h>

#include "fon.h"     		/* Primitives de la boite a outils */

#define SERVICE_DEFAUT "1111"

char *couleurs[8] = {"R","W","G","Y","O","P","F","B"};
char *combinaison[4];

void serveur_appli (char *service);   /* programme serveur */


/******************************************************************************/	
/*---------------- programme serveur ------------------------------*/

int main(int argc,char *argv[])
{

	char *service= SERVICE_DEFAUT; /* numero de service par defaut */


	/* Permet de passer un nombre de parametre variable a l'executable */
	switch (argc)
 	{
   	case 1:
		  printf("defaut service = %s\n", service);
		  		  break;
 	case 2:
		  service=argv[1];
            break;

   	default :
		  printf("Usage:serveur service (nom ou port) \n");
		  exit(1);
 	}

	/* service est le service (ou numero de port) auquel sera affecte
	ce serveur*/
	
	serveur_appli(service);
}


/******************************************************************************/	
void serveur_appli(char *service)

/* Procedure correspondant au traitemnt du serveur de votre application */

{	
	/*Démarrage serveur*/
	int SOCKET = h_socket(AF_UNIX, SOCK_STREAM); //Crée socket TCP
	
	struct sockaddr_in *config_socket;
	adr_socket((unsigned short) SERVICE_DEFAUT, NULL, SOCK_STREAM, &config_socket);

	h_bind(SOCKET,config_socket);
	
	while(/*pas de connexion client*/); //Ne fait rien en attendant la co

	int *buffer_write;
	int *buffer_read;

	/*On attend une difficulté (int)*/
	h_reads(SOCKET, buffer_read, 4);
	int taille_combinaison = *buffer; 
	
	/*Après entier reçu -> génération tableau aleatoire*/
	srand(clock());
	for (int i = 0; i < taille_combinaison; i++)
	{
		combinaison[i] = couleurs[rand()%7];
	}
	
	int proposition;

	while(true/*mdrr*/){
	/*Attente d'une proposition*/
	h_reads(SOCKET, buffer_read, 4);
	/*test resultat*/

	//buffer[0] = mal_place;
	//buffer[1] = bien_place;
	h_writes (SOCKET, buffer_write, 4);
	if (bien_place = taille_combinaison)
		break;
	}
	}


/******************************************************************************/	

