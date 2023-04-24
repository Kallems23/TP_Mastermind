/******************************************************************************/
/*							Application: Mastermind					 	      */
/******************************************************************************/
/*									      									  */
/*						   	   Programme  CLIENT							  */
/*									      									  */
/******************************************************************************/
/*									 									      */
/*			Auteurs : CHERBLANC Noah - ROSANO Romain - ROUBIA Akram			  */
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
	/*********************************************************************************/
	/*************************Démarrage connexion client******************************/
	/*********************************************************************************/
	int SOCKET = h_socket(PF_INET, SOCK_STREAM); //Crée socket TCP locale
	
	struct sockaddr_in *config_socket;
	adr_socket(service, serveur, SOCK_STREAM, &config_socket); //Initialise une adresse de socket (ici TCP) pour communiquer avec le serveur

	//h_bind(SOCKET, config_socket); // Inutile car local

	h_connect(SOCKET, config_socket); //Réalisation de la connexion avec le serveur
	/*********************************************************************************/
	/*********************************************************************************/

	/*********************************************************************************/
	/****************************Démarrage communication******************************/
	/*********************************************************************************/
	
	/*Initialisation buffers, essais et l'entier pour la future difficulte donnée*/
	char *buffer_read = malloc(100*INTSIZE);
 	char *buffer_write = malloc(100*INTSIZE);
	int int_write;
	int essais_restants = 10;
	
	buffer_read[0]=(char)1;
	buffer_read[1]=(char)0;

	/*On donne la difficulté (le nombre de couleurs à deviner) dans le int difficulte*/
	printf("Bienvenue au Mastermind !\nChoisissez votre difficulté :\n");
	scanf("%d", &int_write);
	int difficulte = int_write;
	buffer_write[0] = (char)int_write;
	h_writes(SOCKET, buffer_write, CHARSIZE); //Et on l'envoie au serveur pour qu'il génère le code aléatoire

	while (true) {
		/*On traite la tentative client*/
		printf("Vous pouvez écrire le code secret :\n");
		scanf("%s", (buffer_write)); //On récupère la combinaison tentée (sans compter les retours à la ligne, d'où le "%s")
		h_writes(SOCKET, buffer_write, CHARSIZE*difficulte); //On envoie la tentative au serveur
		h_reads(SOCKET,buffer_read,CHARSIZE*2); //On attend le couple (Couleurs mal placées, Couleurs bien placées)

		/*Si le client gagne, alors il 4 couleurs bien placées et 0 mal placées et il termine la partie*/
		if ((int) buffer_read[0] == 0 && (int) buffer_read[1] == difficulte) {
			printf("Bravo !!!\nVous avez gagné en utilisant %d essais, bien joué !\n",11-essais_restants);

			/*On libère les pointeurs*/
			free(buffer_read);
			free(buffer_write);

			/*Et on ferme la socket de communication avec le serveur*/
			h_close(SOCKET);
			return;
		}

		/*Si le client n'a pas toutes les couleurs de bien placées, il perd un essai en obtenant les informations de sa tentative*/
		if ((int)buffer_read[1]!= difficulte) {
			printf("Mauvaise réponse, vous avez %d bonnes positions et %d couleurs mal placées\n",(int)buffer_read[1],(int)buffer_read[0]);
			essais_restants--;
			printf("Il vous reste %d essais\n", essais_restants);
		}

		/*Si le client n'a plus d'essai, la partie se termine et la combinaison attendue est renvoyée*/
		if(essais_restants == 0) {
			printf("Dommage, vous n'avez plus d'essais\n");
			for (int j=0 ; j<difficulte ; j++) { //On écrit un caractère non-reproduisable par le client (NULL) au serveur pour le notifier que la partie est terminée
				buffer_write[j] = '\0';
			}
			h_writes(SOCKET,buffer_write,difficulte*CHARSIZE);

			/*On récupère et affiche la solution attendue du Mastermind*/
			h_reads(SOCKET,buffer_read, difficulte*CHARSIZE);
			printf("Le code était : \n");
			for (int k=0; k<difficulte; k++) {
				printf("%c", (char)buffer_read[k]);

				if (k==difficulte-1) printf("\n");
			}

			/*On libère les pointeurs*/
			free(buffer_read);
			free(buffer_write);

			/*Et on ferme la socket de communication avec le serveur*/
			h_close(SOCKET);
			return;
		}
	}
	/*********************************************************************************/
	/*********************************************************************************/
 }

/*****************************************************************************/
