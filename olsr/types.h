#ifndef TYPES_H
#define TYPES_H

/*
 * LES CONSTANTES DE TAILLES
 */



#define MAX_TAILLE_PSEUDO 128

#define MAX_TAILLE_TEXTE 512

#define MAX_NOM_FICHIER 256

#define MAX_PARTIE 512


/*
 * LES PORTS DU SERVEUR
 */

 #define PORT_SERVEUR 5000
 #define PORT_SERVEURTCP 5001


 
/*
 * Structure des messages échangés entre les clients et le serveur
 */


typedef struct{

 int MsgType; //definit le type de message
              /*
                 ->0 demande de connexion     
                 
                 	->1 acceptation de la connection   
		        ->2 connection refusée
		        
                 ->3 message toUser
                 
                 ->4 broadcast
                 
                 ->5 demande de deconnection
                 
                 ->6 envoi de fichier (reste a verifier la bonne reception des paquets et dans l'ordre ce qui n'est pas garantit
                 		       par le protocole UDP)	
                      		      		
                      		      		 
                 ->8 serveur en deconnection 
                
               */


  /*--------Partie communication entre utilisateur--------*/

 	char Pseudo[MAX_TAILLE_PSEUDO];
	char ToPseudo[MAX_TAILLE_PSEUDO];
	char txt[MAX_TAILLE_TEXTE];
	
	
	
	int tcp; //port tcp utilise lors de la connexion pour le stcocker
	 
}Msg;



#endif
