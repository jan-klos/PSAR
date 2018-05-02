

#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>
#include <arpa/inet.h>

#define PORTSERV 4567
int main(int argc, char *argv[])
{
    struct sockaddr_in sin; /* Nom de la socket du serveur */
    struct sockaddr_in exp; /* Nom de l'expediteur */
    char host[64];
    int sc ;
    int fromlen = sizeof(exp);
    char message[80];
    int cpt = 0;

    /* creation de la socket */
    if ((sc = socket(AF_INET,SOCK_DGRAM,0)) < 0) {
        perror("socket"); exit(1);
    }
/* remplir le « nom » */
    memset((char *)&sin,0, sizeof(sin));
    sin.sin_addr.s_addr = htonl(INADDR_ANY);
    sin.sin_port = htons(PORTSERV);
    sin.sin_family = AF_INET;
/* nommage */
    if (bind(sc,(struct sockaddr *)&sin,sizeof(sin)) < 0) {
        perror("bind"); exit(2);
    }
/*** Reception du message ***/
    if ( recvfrom(sc,message,sizeof(message),0,(struct sockaddr *)&exp,&fromlen) == -1) {
        perror("recvfrom"); exit(2);
    }
/*** Affichage de l'expediteur ***/
    printf("Exp : <IP = %s,PORT = %d> \n", inet_ntoa(exp.sin_addr),ntohs(exp.sin_port));
/* Nom de la machine */
    if (getnameinfo((struct sockaddr*)&exp, sizeof(exp),host, sizeof(host), NULL, NULL, 0) != 0) {
        perror("getnameinfo"); exit(3);
    }
    printf("Machine : %s\n", host);
/*** Traitement ***/
    printf("Message : %s \n", message);
    cpt++;
/*** Envoyer la reponse ***/
    if (sendto(sc,&cpt,sizeof(cpt),0,(struct sockaddr *)&exp,fromlen) == -1) {
    perror("sendto"); exit(4);
    } close(sc);
return (0);
}
