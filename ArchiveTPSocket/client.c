/******************************************************************************/
/*			Application: Mastermind									 	      */
/******************************************************************************/
/*									      */
/*			 programme  CLIENT				      */
/*									      */
/******************************************************************************/
/*									 									      */
/*		Auteurs : CHERBLANC Noah - ROSANO Romain - ROUBIA Akram				  */
/*									   									      */
/******************************************************************************/	


#include <stdio.h>
#include <curses.h> 		/* Primitives de gestion d'ecran */
#include <sys/signal.h>
#include <sys/wait.h>
#include<stdlib.h>

#include "fon.h"   		/* primitives de la boite a outils */

#define SERVICE_DEFAUT "1111"
#define SERVEUR_DEFAUT "127.0.0.1"
#define INTSIZE 4

void client_appli (char *serveur, char *service);


/*****************************************************************************/
/*--------------- programme client -----------------------*/

int main(int argc, char *argv[])
{

	char *serveur= SERVEUR_DEFAUT; /* serveur par defaut */
	char *service= SERVICE_DEFAUT; /* numero de service par defaut (no de port) */


	/* Permet de passer un nombre de parametre variable a l'executable */
	switch(argc)
	{
 	case 1 :		/* arguments par defaut */
		  printf("serveur par defaut: %s\n",serveur);
		  printf("service par defaut: %s\n",service);
		  break;
  	case 2 :		/* serveur renseigne  */
		  serveur=argv[1];
		  printf("service par defaut: %s\n",service);
		  break;
  	case 3 :		/* serveur, service renseignes */
		  serveur=argv[1];
		  service=argv[2];
		  break;
    default:
		  printf("Usage:client serveur(nom ou @IP)  service (nom ou port) \n");
		  exit(1);
	}

	/* serveur est le nom (ou l'adresse IP) auquel le client va acceder */
	/* service le numero de port sur le serveur correspondant au  */
	/* service desire par le client */
	
	client_appli(serveur,service);
}

/*****************************************************************************/
void client_appli (char *serveur,char *service)

/* procedure correspondant au traitement du client de votre application */

{
  /*Démarrage client*/
	int SOCKET = h_socket(PF_INET, SOCK_STREAM); //Crée socket TCP locale
	
	struct sockaddr_in *config_socket;
	adr_socket(service, serveur, SOCK_STREAM, &config_socket);

	//h_bind(SOCKET, config_socket); // Inutile car local

	h_connect(SOCKET, config_socket);
	printf("Bienvenue au mastermind : Choisissez votre difficulté\n");
	char *buffer_read = malloc(100*INTSIZE);
 	char *buffer_write = malloc(100*INTSIZE);
	int int_write;
	int essais_restants = 10;
	
	buffer_read[0]=(char)0;
	buffer_read[1]=(char)0;
	//On donne la difficulté (le nombre de couleurs à deviner) dans le int difficulte
	scanf("%d", &int_write);
	int difficulte = int_write;
	*buffer_write = (char)int_write;
	//On envoie la difficulté au serveur pour qu'il génère le code aléatoire
	h_writes(SOCKET, buffer_write, INTSIZE);
	printf("Vous pouvez écrire le code\n");
	while (buffer_read[0]!=0 || buffer_read[1]!=difficulte) {
	for (int i = 0; i < difficulte ; i++) {
            scanf("%c", (buffer_write+i));
        }
	//On envoie la tentative au serveur
	h_writes(SOCKET, buffer_write, INTSIZE*difficulte );
	//On attend le couple(Bonnes couleurs mal placées, Couleurs bien placées)
	h_reads(SOCKET,buffer_read,INTSIZE*2);
	if (buffer_read[1]!= difficulte) {
		printf("Mauvaise réponse, vous avez %d bonnes positions et %d bonnes couleurs mal placées\n",buffer_read[1],buffer_read[0]);
		essais_restants--;
	}
	if(essais_restants == 0) {
		printf("Dommage, vous n'avez plus d'essais");
		buffer_write[0]='F';
		//On envoie au serveur le caractère "F" pour "Failure", ce qui signifie qu'on a atteint la limite d'essais
		h_writes(SOCKET,buffer_write,INTSIZE);
		//On attend la solution au Mastermind
		h_reads(SOCKET,buffer_read,difficulte*INTSIZE);
		printf("Le code était\n");
		for (int k=0; k<difficulte; k++) {
			//printf("%c", (buffer_read+k));
			printf(" ");
			}
		}
	}
	printf("Vous avez gagné en utilisant %d essais. Bien Joué",10-essais_restants);

 }

 
 



/*****************************************************************************/
