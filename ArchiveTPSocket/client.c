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
#define CHARSIZE 1

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
	
	buffer_read[0]=(char)1;
	buffer_read[1]=(char)0;
	//On donne la difficulté (le nombre de couleurs à deviner) dans le int difficulte
	scanf("%d", &int_write);
	int difficulte = int_write;
	buffer_write[0] = (char)int_write;
	
	//On envoie la difficulté au serveur pour qu'il génère le code aléatoire
	h_writes(SOCKET, buffer_write, CHARSIZE);
	printf("Vous pouvez écrire le code secret :\n");
	while (true) {
		scanf("%s", (buffer_write)); //on récupère la combianaison tentée
		printf("Le buffer_write contient ceci : \n");
		for (int j = 0; j<CHARSIZE*difficulte; j++) {
			printf("%c\n", *(buffer_write+j));
		}
		h_writes(SOCKET, buffer_write, CHARSIZE*difficulte); //On envoie la tentative au serveur
		h_reads(SOCKET,buffer_read,CHARSIZE*2); //On attend le couple (Couleurs mal placées, Couleurs bien placées)

		if ((int) buffer_read[0] == 0 && (int) buffer_read[1] == difficulte) {
			printf("Vous avez gagné en utilisant %d essais. Bien Joué !\n",11-essais_restants);

			/*Free pointeur*/
			free(buffer_read);
			free(buffer_write);

			/*Fermeture socket*/
			h_close(SOCKET);
			return;
		}

		if ((int)buffer_read[1]!= difficulte) {
			printf("Mauvaise réponse, vous avez %d bonnes positions et %d couleurs mal placées\n",(int)buffer_read[1],(int)buffer_read[0]);
			essais_restants--;
			printf("Il vous reste %d essais\n", essais_restants);
		}

		if(essais_restants == 0) {
			printf("Dommage, vous n'avez plus d'essais\n");
			for (int j=0 ; j<difficulte ; j++) { //On lit tjrs difficulte char dans le client
				buffer_write[j] = 'e';
			}
			h_writes(SOCKET,buffer_write,difficulte*CHARSIZE);
					
			//On envoie au serveur le caractère "e" pour "error", ce qui signifie qu'on a atteint la limite d'essais

			//On attend la solution au Mastermind
			h_reads(SOCKET,buffer_read, difficulte*CHARSIZE);
			printf("Le code était : \n");
			for (int k=0; k<difficulte; k++) {
				printf("%c", (char)buffer_read[k]);

				if (k==difficulte-1) printf("\n");
			}

			/*Free pointeur*/
			free(buffer_read);
			free(buffer_write);

			/*Fermeture socket*/
			h_close(SOCKET);

			return;
		}

		printf("\nVous pouvez écrire le code secret :\n");
	}
 }

 
 



/*****************************************************************************/
