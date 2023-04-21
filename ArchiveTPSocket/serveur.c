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

char couleurs[8] = {'R','W','G','Y','O','P','F','B'};
char combinaison[4];

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
	int SOCKET = h_socket(PF_INET, SOCK_STREAM); //Crée socket TCP locale
	
	struct sockaddr_in *config_socket;
	adr_socket(service, NULL, SOCK_STREAM, &config_socket);

	h_bind(SOCKET,config_socket);
	
	h_listen(SOCKET,100); //On prévoit une "communication" à la fois

	h_accept(SOCKET,config_socket); //Accepte la connexion client (fonction bloquante s'il n'y en a pas)

	char *buffer_write;
	char *buffer_read;

	/*On attend une difficulté (int)*/
	h_reads(SOCKET, buffer_read, 4); //Lit la difficulté
	int taille_combinaison = (int) *buffer_read; 
	
	int *couleur = malloc(sizeof(int)*taille_combinaison);
	int *couleur_test = malloc(sizeof(int)*taille_combinaison);
	int ajout;
	/*Après entier reçu -> génération tableau aleatoire*/
	srand(clock());
	for (int i = 0; i < taille_combinaison; i++) {	
		ajout = rand()%7;
		combinaison[i] = couleurs[ajout];
		couleur[ajout]++;
	}
	int bien_place = 0;
	int mal_place = 0;

	while(mal_place != 0 || bien_place != taille_combinaison){
		/*Attente/Lecture notre proposition*/
		h_reads(SOCKET, buffer_read, taille_combinaison*1);

		/*test resultat*/
		*couleur_test = *couleur;
		for (int i = 0; i < taille_combinaison; i++)
			{	
			bien_place += (buffer_read[i] == combinaison[i]) ? 1 : 0;
			if(couleur_test[buffer_read[i]] > 0){
				mal_place ++;
				couleur_test[buffer_read[i]]--; 
			}
		}
	}
		/*On envoie le couple (bien placé, mal placé)*/
		buffer_write[0] = mal_place-bien_place;
		buffer_write[1] = bien_place;
		h_writes (SOCKET, buffer_write, 8);
	}


/******************************************************************************/	

