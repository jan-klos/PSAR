    /*+-----------------------------------------------+
  |						  |	
  |          Serveur en mode non connecté	  |
  |            non connecté pour Upload           |
  |						  |
  +-----------------------------------------------+ */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <sys/un.h>
#include <fcntl.h>
#include <pthread.h>
#include <signal.h>
#include <assert.h>

#include "types.h"

#define IP_SERVEUR "127.0.0.1"

#define USE printf("USAGE %s < terminal affichage des messages > < terminal pour upload >\n",argv[0]); exit(EXIT_FAILURE);


/*
 *	Variables globales
 */


 char MyPseudo[MAX_TAILLE_PSEUDO];
 char Fic[MAX_NOM_FICHIER];
 
 char *term;
 int sckid;
 pthread_t pere;
 pthread_t th; //thread qui gere la reception des messages
 int porttcp;
 
/*
 * 	Prototype des fonctions
 */
 
 
 void error(char *s);
 void* codeThread(void *arg);  //thread qui sert au chat
 void* codeThread2(void *arg); //ce thread servira au partage de fichier
 void quitter(int sig);

 
/*
 * Fonction principale
 */


int main(int arg,char *argv[]){


    if (arg != 3) {  USE }

     term = argv[1];
     pere = pthread_self();

     
  /*
   * Masquage de tous les signaux
   */  
   
    sigset_t mask;
    if (  sigfillset(&mask) == -1 ) error("Erreur lors de la creation du mask"); 
    if (  sigprocmask(SIG_SETMASK,&mask,NULL) == -1 ) error("Erreur lors d ela creation du mask"); 
    
     
   /*
    * On utilise sigint afin de mettre fin au Client
    */
     
    struct sigaction quit;
    quit.sa_handler = quitter;
    quit.sa_flags = 0; 
     
    if (  sigfillset(&quit.sa_mask) == -1 ) error("Erreur lors d ela creation du mask"); 
    if (  sigaction(SIGINT,&quit,NULL) == -1 ) error("Erreur lors d ela creation du mask");     
    if (  sigaction(SIGUSR1,&quit,NULL) == -1 ) error("Erreur lors d ela creation du mask");     
       
     

    /*
     * Creation de la socket
     */
     
      int portudp;
      printf("Port UDP ouvert pour la communication :");
      scanf("%i",&portudp);
      
  
      printf("Port TCP ouvert pour la communication (pas encore ajouter) :");
      scanf("%i",&porttcp);
            

      if ( (sckid=socket(PF_INET,SOCK_DGRAM,0)) == -1 ) error("Erreur lors de la creation de la socket\n");	
 
        struct sockaddr_in adresse;
        adresse.sin_family = AF_INET;
        adresse.sin_port = htons(portudp); //adresse de port libre 
        adresse.sin_addr.s_addr = htonl(INADDR_ANY);
          
          
         if ( bind(sckid,(struct sockaddr *)&adresse,sizeof(adresse)) == -1 ) error("PORT UDP INVALIDE RESERVE"); //attachement du socket pour que
         											    //pour eviter les ports bloquer par le 
         											    //firewall et reconnaissance au niveau 
 									                           //local	     
        adresse.sin_family = AF_INET;
        adresse.sin_port = htons(PORT_SERVEUR); 
        inet_aton(IP_SERVEUR,&(adresse.sin_addr));

       
        Msg mess;
        mess.MsgType=0;
        mess.tcp = porttcp;

          printf("Pseudo : ");
          scanf("%255s",mess.Pseudo);
           
         strcpy(MyPseudo,mess.Pseudo);
          
         if ( sendto(sckid,&mess,sizeof(Msg),0,(struct sockaddr *)&adresse,sizeof(struct sockaddr_in)) == -1 ) error("Erreur send");       
	          
          printf("Attente de la reponse du serveur.....\n");
          
         socklen_t lg = sizeof(struct sockaddr_in);
	 if( recvfrom(sckid,&mess,sizeof(Msg),0,(struct sockaddr *)&adresse,&lg) == -1 ) error("Erreur recv");
	 
              switch(mess.MsgType){
              
                case 1 : printf("Connexion reussie....\n"); break;
                case 2 : printf("Connexion refusee....\n"); exit(EXIT_FAILURE); break;                                

              }    
              
              
        /*
         * On demasque sigint et sigusr1
         */ 
              
         if (  sigdelset(&mask,SIGINT) == -1 ) error("Erreur lors de la suppression");
         if (  sigdelset(&mask,SIGUSR1) == -1 ) error("Erreur lors de la suppression");  
         if (  sigprocmask(SIG_SETMASK,&mask,NULL) == -1 ) error("Erreur lors de l'application du mask"); 
                     
        //Creation d'un thread qui se chargera d'afficher les messages reçus       
        if ( pthread_create(&th,NULL,codeThread,&sckid) > 0 ) error("Erreur creation thread");

       strcpy(mess.Pseudo,MyPseudo);

       while(1){
                    
          printf("Pseudo du destinataire (broadcast our message salon) (UPLOAD) : ");
          scanf("%s",mess.ToPseudo);
                   
         if( strcmp(mess.ToPseudo,"UPLOAD") == 0 ){ //UPLOAD d'un fichier vers le serveur
         
              mess.MsgType = 6;
              if ( sendto(sckid,&mess,sizeof(Msg),0,(struct sockaddr *)&adresse,sizeof(struct sockaddr_in)) == -1 ) error("Erreur send"); 
              
              printf("Fichier :");
              scanf("%255s",Fic);
                                          
              //attente de la fin de l'upload se passe dans un nouveau terminal
              pthread_t th2;
              if ( pthread_create(&th2,NULL,codeThread2,argv[2]) > 0 ) error("Erreur creation thread");
        
         }
         else{
               
         	 if( strcmp(mess.ToPseudo,"broadcast") == 0 ) mess.MsgType = 4;
         	 else mess.MsgType = 3;
         
             	     char c;
            	     while( (c = getchar()) && c != '\n' ); //evite les erreur dues au \n
        
	       	  printf("Entrer le message : ");
        	  fgets(mess.txt,MAX_TAILLE_TEXTE,stdin);
        
                //envoi du message
	       if ( sendto(sckid,&mess,sizeof(Msg),0,(struct sockaddr *)&adresse,sizeof(struct sockaddr_in)) == -1 ) error("Erreur send");       
        
        }
        
     }

   return EXIT_SUCCESS;
}


 void error(char *s){
   perror(s);
   exit(EXIT_FAILURE);
 }


 /*
  * Code du thread qui servira au partage de fichier
  */


void* codeThread2(void *arg){ 

        /*
         *  Etablissement de la connection TCP pour l'upload
         */ 
    
    
   int sck;
   int out;
   struct sockaddr_in adresse;
      
   if ( (out = open((char *)arg,O_RDWR)) == -1) error("Erreur open");
   
   if( !isatty(out) ) { exit(EXIT_FAILURE); } //realiser une terminaison plus propre  
   
   
   if ( (sck=socket(PF_INET,SOCK_STREAM,0)) == -1 ) error("Erreur lors de la creation de la socket\n");
      
    adresse.sin_family = AF_INET;
    adresse.sin_port = htons(PORT_SERVEURTCP); //adresse de port serveur
    inet_aton(IP_SERVEUR,&(adresse.sin_addr));
        
   if ( connect(sck,(struct sockaddr *)&adresse,sizeof(adresse)) == -1) error("Erreur connection");
         
           int d; 
                                                         
           if ( (d=open(Fic,O_RDONLY) ) == -1 ) error("Erreur open fichier inexistant");   

           //statistique sur le fichier
          
           struct stat s;
	   if ( fstat(d,&s) == -1 ) error("Erreur stat");	
	
	   int taille =  s.st_size;
	   	   	    
	  //envoi du nom de fichier
	  if ( write(sck,Fic,strlen(Fic)+1) == -1) error("Erreur write");    
	  
	  //envoi du nombre de partie
	  if ( write(sck,&taille,sizeof(int)) == -1) error("Erreur write");    
	      
	   char Partie[MAX_PARTIE];              
	   int lu;
	   	                 
           while ( (lu=read(d,Partie,MAX_PARTIE)) != 0 )                          
              if ( write(sck,Partie,lu) == -1) error("Erreur write");   
                              
    close(sck);
    close(d);      
          
   pthread_exit(NULL);

 }



 /*
  * Code du thread qui recoit les messages sur le socket
  */

  
void* codeThread(void *arg){ 

  struct sockaddr_in adresse;
  adresse.sin_family = AF_INET;
  adresse.sin_port = htons(PORT_SERVEUR);
  inet_aton(IP_SERVEUR,&(adresse.sin_addr));
  
  int sid = *((int *)arg);

  int out;
  char Mess[MAX_TAILLE_PSEUDO + 16]; 
  
  if ( (out = open(term,O_WRONLY)) == -1 ) error("Erreur open");
     
     if ( !isatty(out) ) error("Erreur pas de second terminal");
 
   Msg mess;  

   do{
    	    socklen_t lg = sizeof(struct sockaddr_in);
     
	     if( recvfrom(sid,&mess,sizeof(Msg),0,(struct sockaddr *)&adresse,&lg) == -1 ) error("Erreur recv");
          
 	    switch(mess.MsgType){
 	    
 	        case 3 : case 4 : {
 	          		   sprintf(Mess,"Message de [%s] : %s",mess.Pseudo,mess.txt);
	                           if ( write(out,Mess,strlen(Mess) ) == -1) error("Erreur write");
				  }break;
 	     
 	      }
 	
 	    
   }while(mess.MsgType != 8); 

   printf("\nLe serveur a ete arrete fin des clients...\n");	
   
   if ( pthread_kill(pere,SIGUSR1) >0 )error("Erreur kill");	
 
   pthread_exit(NULL);
}



 /*
  * Handler qui sert a quitter
  */



 void quitter(int sig){

 if ( sig == SIGINT){
 
  // On avertir le serveur que l'on se deconnecte
	  Msg mess;
	  mess.MsgType = 5;
	  strcpy(mess.Pseudo,MyPseudo);  
  
  //envoi au serveur
  struct sockaddr_in adr;
  adr.sin_family = AF_INET;
  adr.sin_port = htons(PORT_SERVEUR);
  inet_aton(IP_SERVEUR,&(adr.sin_addr));

  
    if ( sendto(sckid,&mess,sizeof(Msg),0,(struct sockaddr *)&adr,sizeof(struct sockaddr_in)) == -1 ) error("Erreur send");
  
  
  printf("\nTerminaison du client...\n");
  exit(EXIT_SUCCESS); //termine le processus et par consequent les deux threads 
  
 }
 else{
   
   printf("Terminaison des threads..\n");
   if ( pthread_join(th,NULL) == -1) error("Erreur join thread");
   //terminaison du thread natif
   pthread_exit(NULL);    
 
 }

 
 }
 
 
 
