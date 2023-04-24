/******************************************************************************/
/*							Application: Mastermind					 	      */
/******************************************************************************/
/*									      									  */
/*						   	   Programme  SERVEUR							  */
/*									      									  */
/******************************************************************************/
/*									 									      */
/*			Auteurs : CHERBLANC Noah - ROSANO Romain - ROUBIA Akram			  */
/*									   									      */
/******************************************************************************/		

#include <stdio.h>
#include <curses.h>
#include <time.h>
#include <sys/signal.h>
#include <sys/wait.h>
#include <stdlib.h>

#include "fon.h"     		/* Primitives de la boite a outils */

#define SERVICE_DEFAUT "1111"

/*Tableau de couleurs global à partir de leurs initiales :
	Red, White, Green, Yellow, Orange, Purple, Fuschia, Blue*/
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

/*Permet de donner l'index d'une couleur donnée dans le tableau global "couleurs"*/
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

/* Procedure correspondant au traitement du serveur de votre application */

{	
	/*********************************************************************************/
	/***************************Démarrage du serveur**********************************/
	/*********************************************************************************/
	int SOCKET = h_socket(PF_INET, SOCK_STREAM); //Crée socket TCP locale
	
	struct sockaddr_in *config_socket;
	adr_socket(service, NULL, SOCK_STREAM, &config_socket); //Initialise une adresse de socket (selon le type attendu, ici, TCP) qui sera défini par la suite pour le serveur

	h_bind(SOCKET,config_socket); //Associe l'adresse (IP, port en gros) à la socket créée précédemment
	
	h_listen(SOCKET,1); //On prévoit une "communication" avec un client à la fois
	/*********************************************************************************/
	/*********************************************************************************/


	/*********************************************************************************/
	/***************************Connexion avec client*********************************/
	/*********************************************************************************/
	while (true) { //Le serveur est toujours en attente d'une connexion client, même quand une partie s'est terminée
		int SOCKET_LIAISON_CLIENT = h_accept(SOCKET,TOUT_CLIENT); //Accepte toute connexion client (fonction bloquante s'il n'y en a pas)

		/*Initialisation des buffers de lecture et écriture*/
		char *buffer_write = malloc(1000);
		char *buffer_read = malloc(1000);

		/*On attend une difficulté (int)*/
		h_reads(SOCKET_LIAISON_CLIENT, buffer_read, 1); //Lit la difficulté
		int taille_combinaison = (int)*buffer_read;
		
		/*On initialise les tableaux pour les propositions de combinaisons*/
		int *occurence_couleurs = malloc(sizeof(int)*8);
		for (int i = 0; i < 8; i++) {	
			occurence_couleurs[i]=0;
		}
		int *occurence_couleurs_boucle = malloc(sizeof(int)*taille_combinaison); //Afin de pouvoir coder les couleurs mal placées dans la suite
		int ajout;
		char combinaison[taille_combinaison];

		/*Après l'entier de difficulté reçu, on génére un tableau aléatoire de couleurs*/
		srand(clock()); //On récupère le temps actuel pour faire fonctionner l'aléatoire
		for (int i = 0; i < taille_combinaison; i++) {	
			ajout = rand()%7; //Selon le temps, on fait modulo 7 pour avoir l'index d'une couleur dans le tableau
			combinaison[i] = couleurs[ajout]; //On ajoute la couleur associée dans combinaison
			occurence_couleurs[ajout]++; //Et on incrémente son occurence
		}
		
		int bien_place = 0;
		int bonne_couleur = 0;
		while(bonne_couleur != 0 || bien_place != taille_combinaison){
			bien_place = 0;
			bonne_couleur = 0;
			
			/*Attente/lecture proposition*/
			h_reads(SOCKET_LIAISON_CLIENT, buffer_read, taille_combinaison*1);

			/*On note le nombre d'occurences des couleurs selon leurs indexs*/
			for (int i = 0; i < 8; i++){
				occurence_couleurs_boucle[i] = occurence_couleurs[i];
			}
			
			/*On teste la proposition reçue*/
			for (int i = 0; i < taille_combinaison; i++){
				
				/*Si on reçoit le caractère de fin de partie (NULL) alors on envoie la combinaison qui était à trouver*/
				if(buffer_read[0] == '\0') {
					for (int j=0 ; j<taille_combinaison ; j++) {
						buffer_write[j] = combinaison[j];
						printf("%c", combinaison[j]);
					}
					h_writes (SOCKET_LIAISON_CLIENT, buffer_write, taille_combinaison);
				}

				/*On calcule le nombre de couleurs bien et mal placées*/
				bien_place += (buffer_read[i] == combinaison[i]) ? 1 : 0; 
				if(occurence_couleurs_boucle[trouve_couleur(buffer_read[i])] > 0){
					bonne_couleur ++;
					occurence_couleurs_boucle[trouve_couleur(buffer_read[i])]--; 
				}
			}

			/*On envoie le couple (mal placé, bien placé)*/
			buffer_write[0] = (char)(bonne_couleur-bien_place); //Couleurs mal placées
			buffer_write[1] = (char)bien_place; //Couleurs bien placées
			h_writes (SOCKET_LIAISON_CLIENT, buffer_write, 2);

			/*Pour sortir du while en cas de défaite*/
			if(buffer_read[0] == '\0') bien_place = taille_combinaison; 
		}

		/*On libère les pointeurs et on ferme la socket de communication avec le client seulement*/
		free(buffer_read);
		free(buffer_write);
		free(occurence_couleurs);
		free(occurence_couleurs_boucle);
		h_close(SOCKET_LIAISON_CLIENT);
		}
	/*********************************************************************************/
	/*********************************************************************************/

}

/******************************************************************************/	

