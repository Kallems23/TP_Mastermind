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

	char *buffer_write = malloc(100000);
	char *buffer_read = malloc(100000);

	/*On attend une difficulté (int)*/
	h_reads(SOCKET_LIAISON_CLIENT, buffer_read, 1); //Lit la difficulté
	//h_reads(SOCKET_LIAISON_CLIENT, buffer_read, 1); 
	int taille_combinaison = (int)*buffer_read;
	//printf("debug print\n");
	
	int *couleur = malloc(sizeof(int)*8);
	for (int i = 0; i < 8; i++) {	
		couleur[i]=0;
	}
	int *couleur_test = malloc(sizeof(int)*taille_combinaison);
	int ajout;

	char combinaison[taille_combinaison];
	/*Après entier reçu -> génération tableau aleatoire*/

	srand(clock());
	for (int i = 0; i < taille_combinaison; i++) {	
		ajout = rand()%7;
		combinaison[i] = couleurs[ajout];
		couleur[ajout]++;
	}

	 printf("Solution :\n");
	 for (int i = 0; i < taille_combinaison; i++) {	
		printf("%c",combinaison[i]);
	}

	int bien_place = 0;
	int mal_place = 0;
	while(mal_place != 0 || bien_place != taille_combinaison){
		int bien_place = 0;
		int mal_place = 0;
		
		/*Attente/lecture proposition*/
		h_reads(SOCKET_LIAISON_CLIENT, buffer_read, taille_combinaison*1);

		/*test resultat*/
		for (int i = 0; i < 8; i++){
			couleur_test[i] = couleur[i];
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

			bien_place += (buffer_read[i] == combinaison[i]) ? 1 : 0;
			printf("DEBUG bien place boucle %d\n",bien_place);
			if(couleur_test[trouve_couleur(buffer_read[i])] > 0){
				mal_place ++;
				couleur_test[trouve_couleur(buffer_read[i])]--; 
				printf("Debug val couleur %d\n",couleur_test[trouve_couleur(buffer_read[i])]);
				printf("Debug test couleur\n");
			}
		}
		/*On envoie le couple (bien placé, mal placé)*/
		printf("Debug avant couple couleur\n");
		printf("DEBUG mal place %d\n",mal_place);
		printf("DEBUG bien place %d\n",bien_place);
		buffer_write[0] = (char)(mal_place-bien_place);
		buffer_write[1] = (char)bien_place;
		h_writes (SOCKET_LIAISON_CLIENT, buffer_write, 2);
	}


	/*Libération pointeur*/
	free(buffer_read);
	free(buffer_write);
	free(couleur);
	free(couleur_test);

	/*Fermeture socket*/
	h_close(SOCKET);
	h_close(SOCKET_LIAISON_CLIENT);
	}



/******************************************************************************/	

