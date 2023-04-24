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

int trouve_couleur(char c){
	for (int i = 0; i < 8; i++){
		if(couleurs[i] == c){
			return i;
		}
	}
	return -1;
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

	int SOCKET_LIAISON_CLIENT = h_accept(SOCKET,config_socket); //Accepte la connexion client (fonction bloquante s'il n'y en a pas)

	//initialisation des buffers
	char *buffer_write = malloc(1000);
	char *buffer_read = malloc(1000);

	/*On attend une difficulté (int)*/
	h_reads(SOCKET_LIAISON_CLIENT, buffer_read, 1); //Lit la difficulté
	//h_reads(SOCKET_LIAISON_CLIENT, buffer_read, 1); 
	int taille_combinaison = (int)*buffer_read;
	//printf("debug print\n");
	
	//On initialise les tableaux pour les tests
	int *occurence_couleurs = malloc(sizeof(int)*8);
	for (int i = 0; i < 8; i++) {	
		occurence_couleurs[i]=0;
	}
	int *occurence_couleurs_boucle = malloc(sizeof(int)*taille_combinaison);
	int ajout;
	char combinaison[taille_combinaison];

	/*Après entier reçu -> génération tableau aleatoire*/
	srand(clock());
	for (int i = 0; i < taille_combinaison; i++) {	
		ajout = rand()%7;
		combinaison[i] = couleurs[ajout];
		occurence_couleurs[ajout]++;
	}

	int bien_place = 0;
	int bonne_couleur = 0;
	while(bonne_couleur != 0 || bien_place != taille_combinaison){
		int bien_place = 0;
		int bonne_couleur = 0;
		
		/*Attente/lecture proposition*/
		h_reads(SOCKET_LIAISON_CLIENT, buffer_read, taille_combinaison*1);

		/*test resultat*/
		for (int i = 0; i < 8; i++){
			occurence_couleurs_boucle[i] = occurence_couleurs[i];
		}
		for (int i = 0; i < taille_combinaison; i++){

			/*Si on reçoit le caractère de fin de partie*/
			if(buffer_read[0] == 'e') {
				for (int j=0 ; j<taille_combinaison ; j++) {
					buffer_write[j] = combinaison[j];
					printf("%c", combinaison[j]);
				}
				h_writes (SOCKET_LIAISON_CLIENT, buffer_write, taille_combinaison);
			}

		//on calcule le nombre de bien placé et de mal placé
			bien_place += (buffer_read[i] == combinaison[i]) ? 1 : 0; 
			if(occurence_couleurs_boucle[trouve_couleur(buffer_read[i])] > 0){
				bonne_couleur ++;
				occurence_couleurs_boucle[trouve_couleur(buffer_read[i])]--; 
			}
		}

		/*On envoie le couple (bien placé, mal placé)*/
		buffer_write[0] = (char)(bonne_couleur-bien_place); //mal placé
		buffer_write[1] = (char)bien_place; //bien placé
		h_writes (SOCKET_LIAISON_CLIENT, buffer_write, 2);
	}


	/*Libération pointeur*/
	free(buffer_read);
	free(buffer_write);
	free(occurence_couleurs);
	free(occurence_couleurs_boucle);

	/*Fermeture socket*/
	h_close(SOCKET);
	h_close(SOCKET_LIAISON_CLIENT);
	}



/******************************************************************************/	

