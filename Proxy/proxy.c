#include  <stdio.h>
#include  <stdlib.h>
#include  <sys/socket.h>
#include  <netdb.h>
#include  <string.h>
#include  <unistd.h>
#include  <stdbool.h>
#include "./simpleSocketAPI.h"


#define SERVADDR "127.0.0.1"        // Définition de l'adresse IP d'écoute
#define SERVPORT "0"                // Définition du port d'écoute, si 0 port choisi dynamiquement
#define LISTENLEN 1                 // Taille de la file des demandes de connexion
#define MAXBUFFERLEN 1024           // Taille du tampon pour les échanges de données
#define MAXHOSTLEN 64               // Taille d'un nom de machine
#define MAXPORTLEN 64               // Taille d'un numéro de port


int main(){
    int ecode;                       // Code retour des fonctions
    char serverAddr[MAXHOSTLEN];     // Adresse du serveur
    char serverPort[MAXPORTLEN];     // Port du server
    int descSockRDV;                 // Descripteur de socket de rendez-vous
    int descSockCOM;                 // Descripteur de socket de communication
    struct addrinfo hints;           // Contrôle la fonction getaddrinfo
    struct addrinfo *res;            // Contient le résultat de la fonction getaddrinfo
    struct sockaddr_storage myinfo;  // Informations sur la connexion de RDV
    struct sockaddr_storage from;    // Informations sur le client connecté
    socklen_t len;                   // Variable utilisée pour stocker les
				                     // longueurs des structures de socket
    char buffer[MAXBUFFERLEN];       // Tampon de communication entre le client et le serveur

    // Initialisation de la socket de RDV IPv4/TCP
    descSockRDV = socket(AF_INET, SOCK_STREAM, 0);
    if (descSockRDV == -1) {
         perror("Erreur création socket RDV\n");
         exit(2);
    }
    // Publication de la socket au niveau du système
    // Assignation d'une adresse IP et un numéro de port
    // Mise à zéro de hints
    memset(&hints, 0, sizeof(hints));
    // Initialisation de hints
    hints.ai_flags = AI_PASSIVE;      // mode serveur, nous allons utiliser la fonction bind
    hints.ai_socktype = SOCK_STREAM;  // TCP
    hints.ai_family = AF_INET;        // seules les adresses IPv4 seront présentées par
				                      // la fonction getaddrinfo

     // Récupération des informations du serveur
     ecode = getaddrinfo(SERVADDR, SERVPORT, &hints, &res);
     if (ecode) {
         fprintf(stderr,"getaddrinfo: %s\n", gai_strerror(ecode));
         exit(1);
     }
     // Publication de la socket
     ecode = bind(descSockRDV, res->ai_addr, res->ai_addrlen);
     if (ecode == -1) {
         perror("Erreur liaison de la socket de RDV");
         exit(3);
     }
     // Nous n'avons plus besoin de cette liste chainée addrinfo
     freeaddrinfo(res);

     // Récuppération du nom de la machine et du numéro de port pour affichage à l'écran
     len=sizeof(struct sockaddr_storage);
     ecode=getsockname(descSockRDV, (struct sockaddr *) &myinfo, &len);
     if (ecode == -1)
     {
         perror("SERVEUR: getsockname");
         exit(4);
     }
     ecode = getnameinfo((struct sockaddr*)&myinfo, sizeof(myinfo), serverAddr,MAXHOSTLEN,
                         serverPort, MAXPORTLEN, NI_NUMERICHOST | NI_NUMERICSERV);
     if (ecode != 0) {
             fprintf(stderr, "error in getnameinfo: %s\n", gai_strerror(ecode));
             exit(4);
     }
     printf("L'adresse d'ecoute est: %s\n", serverAddr);
     printf("Le port d'ecoute est: %s\n", serverPort);

     // Definition de la taille du tampon contenant les demandes de connexion
     ecode = listen(descSockRDV, LISTENLEN);
     if (ecode == -1) {
         perror("Erreur initialisation buffer d'écoute");
         exit(5);
     }

	len = sizeof(struct sockaddr_storage);
     // Attente connexion du client
     // Lorsque demande de connexion, creation d'une socket de communication avec le client
     descSockCOM = accept(descSockRDV, (struct sockaddr *) &from, &len);
     if (descSockCOM == -1){
         perror("Erreur accept\n");
         exit(6);
     }
    // Echange de données avec le client connecté

    /*****
     * Testez de mettre 220 devant BLABLABLA ...
     * **/
    strcpy(buffer, "220 BLABLABLA\n");
    write(descSockCOM, buffer, strlen(buffer));

    /*******
     *
     * A vous de continuer !
     *
     * *****/

     char Fin[3];
     Fin[0] = '\r';
     Fin[1] = '\n';
     Fin[2] = '\0';
     //lecture des identifiants
         ecode = read(descSockCOM, buffer, MAXBUFFERLEN-1);
         if(ecode == -1) {
             perror("problème de lecture des identifiants");
             exit(3);
         }
         buffer[ecode] = '\0';

         //récupération des valeur USER anonymous@ftp.fau.de
         char commandeUser[5]; // récupère la chaine USER
         char data[31]; //Contient anonymous@NomHôte
         char login[20]; //contient anonymous
         char hostftp[21]; //contient le nom d'hôte de la connexion
         sscanf(buffer, "%s %s", commandeUser, data);
         sscanf(data, "%[^@]@%s", login, hostftp);

         printf(" commande : %s\n login : %s\n host : %s\n", commandeUser, login, hostftp);

         //Connexion au serveur
         int SokCOMHost;
         char portServeur[5];
         strcpy(portServeur, "21");
         int estconnecte;
         estconnecte = connect2Server(hostftp, portServeur, &SokCOMHost); // Socket de communication vers le serveur host connecté au port 21;

         //Msg d'erreur si la connexion ne se passe pas correctement
         if(estconnecte == -1) {
             perror(" problème, connexion impossible.\n");
             exit(3);
         } else {
            printf(" Connecté au serveur %s.\n", hostftp);
         }

        bzero(buffer, MAXBUFFERLEN - 1);
        ecode = read(SokCOMHost, buffer, MAXBUFFERLEN -1);
        if(ecode == -1) {
            perror("Problème pour lire les réponses du serveur.\n");
            exit(3);
        }

        strncat(buffer, Fin, strlen(Fin));
        printf("%s\n", buffer);

        char LoginTab[50 + 1] ; //tableau de stockage
        bzero(LoginTab, 50); //vide le tableau
        strncat(LoginTab, commandeUser, strlen(commandeUser));
        strncat(LoginTab," ", 2);
        strncat(LoginTab, login, strlen(login)); // ajout du login dans le tableau
        strncat(LoginTab, Fin, strlen(Fin));
        //On écrit l'identifiant sur le serveur qu'on a recupérrer précédemment
        ecode = write(SokCOMHost, LoginTab, strlen(LoginTab));
        if (ecode == -1) {
                perror("Erreur d'écriture serveur\n");
                exit(3);
        }

        printf("%s\n",LoginTab );


        bzero(buffer, MAXBUFFERLEN);
        ecode = read(SokCOMHost, buffer, MAXBUFFERLEN - 1);
        if (ecode == -1) {
                perror("Error read serveur\n");
                exit(3);
        }

        printf("%s\n",buffer);
        //
        //
        strncat(buffer, Fin, strlen(Fin));
        //printf("Serveur ::1 \"%s\".\n", buffer);
        //On écrit le retour sur le client : login anonymous ok
        ecode = write(descSockCOM, buffer, strlen(buffer)-1);
        if (ecode == -1) {
                perror("Error write client\n");
                exit(3);
        }

        //printf("%s\n", buffer );
        bzero(buffer, MAXBUFFERLEN);//vider le buffer
        // demande au client son mdp (331)
           write(descSockCOM, buffer, strlen(buffer));
           if (ecode == -1)
           {
               perror("probleme d'ecriture");
               exit(1);
           }

           //la reponse du client PASS
           ecode = read(descSockCOM, buffer, MAXBUFFERLEN - 1);
           if (ecode == -1)
           {
               perror("probleme de lecture");
               exit(1);
           }
           buffer[ecode] = '\0';

           // envoie du mdp
           write(SokCOMHost, buffer, strlen(buffer));
           if (ecode == -1)
           {
               perror("probleme d'ecriture client");
               exit(1);
           }

           // ecoute la reponse (230)
           ecode = read(SokCOMHost, buffer, MAXBUFFERLEN - 1);
           if (ecode == -1)
           {
               perror("probleme de lecture");
               exit(1);
           }
           buffer[ecode] = '\0';

           // envoie la confirmation au client (230)
           write(descSockCOM, buffer, strlen(buffer));
           if (ecode == -1)
           {
               perror("probleme d'ecriture client");
               exit(1);
           }

           // ecoute la reponse du client (SYST)
           ecode = read(descSockCOM, buffer, MAXBUFFERLEN - 1);
           if (ecode == -1)
           {
               perror("probleme de lecture");
               exit(1);
           }
           buffer[ecode] = '\0';


           // envoie au serveur (SYST)
           write(SokCOMHost, buffer, strlen(buffer));
           if (ecode == -1)
           {
               perror("probleme d'ecriture serveur");
               exit(1);
           }

           // ecoute la reponse du serveur (215)
           ecode = read(SokCOMHost, buffer, MAXBUFFERLEN - 1);
           if (ecode == -1)
           {
               perror("probleme de lecture");
               exit(1);
           }
           buffer[ecode] = '\0';

           // envoie au client (215)
           write(descSockCOM, buffer, strlen(buffer));
           if (ecode == -1)
           {
               perror("probleme d'ecriture client");
               exit(1);
           }

           // ecoute la reponse du client
           ecode = read(descSockCOM, buffer, MAXBUFFERLEN - 1);
           if (ecode == -1)
           {
               perror("probleme de lecture");
               exit(1);
           }
           buffer[ecode] = '\0';

           // decoupage du PORT
           int n1, n2, n3, n4, n5, n6;
           sscanf(buffer, "PORT %d,%d,%d,%d,%d,%d", &n1, &n2, &n3, &n4, &n5, &n6); // decoupage du message buffer recu

           // calcul de pasv
           char ipC[50];
           char portC[10];
           sprintf(ipC, "%d.%d.%d.%d", n1, n2, n3, n4);
           sprintf(portC, "%d", n5 * 256 + n6);

           // connection avec le client
           int modeActif;
           estconnecte = connect2Server(ipC, portC, &modeActif);
           if(estconnecte == -1) {
               perror(" problème, connexion impossible.\n");
               exit(3);
           } else {
              printf(" Connecté au serveur %s.\n", ipC);
           }

           // envoie de PASV au serveur
           strcpy(buffer, "PASV\r\n");
           ecode = write(SokCOMHost, buffer, strlen(buffer));
           if (ecode == -1)
           {
               perror("probleme d'envoie de PASV");
               exit(1);
           }

           // ecoute du serveur
           ecode = read(SokCOMHost, buffer, MAXBUFFERLEN - 1);
           if (ecode == -1)
           {
               perror("probleme de lecture");
               exit(1);
           }
           buffer[ecode] = '\0';
           printf("Recu du serveur: %s\n", buffer);

           // decoupage de l'ip et du port
           sscanf(buffer, "227 Entering Passive Mode (%d,%d,%d,%d,%d,%d)", &n1, &n2, &n3, &n4, &n5, &n6);

           char ipS[50];
           char portS[10];
           sprintf(ipS, "%d.%d.%d.%d", n1, n2, n3, n4);
           sprintf(portS, "%d", n5 * 256 + n6);

           // connection au serveur
           int modePassif;
           estconnecte = connect2Server(ipS, portS, &modePassif);
           if(estconnecte == -1) {
               perror(" problème, connexion impossible.\n");
               exit(3);
           } else {
              printf(" Connecté au serveur %s.\n", ipS);
           }


           // envoie de 200 au client
           char message[50] = "200 OK\n";
           ecode = write(descSockCOM, message, strlen(message));
           if (ecode == -1)
           {
               perror("probleme attendu 200");
               exit(1);
           }

           // ecoute de LIST au client
           ecode = read(descSockCOM, buffer, MAXBUFFERLEN - 1);
           if (ecode == -1)
           {
               perror("probleme de lecture");
               exit(1);
           }
           buffer[ecode] = '\0';
           printf("Recu du client ftp: %s\n", buffer);

           // envoie au serveur (LIST)
           ecode = write(SokCOMHost, buffer, strlen(buffer));
           if (ecode == -1)
           {
               perror("probleme d'envoie de LIST");
               exit(1);
           }

           printf("%s\n", buffer);

           // ecoute du serveur
           ecode = read(SokCOMHost, buffer, MAXBUFFERLEN - 1);
           if (ecode == -1)
           {
               perror("probleme de lecture");
               exit(1);
           }
           buffer[ecode] = '\0';
           printf("Recu du serveur: %s\n", buffer);

           // envoie au client (150)
          ecode = write(descSockCOM, buffer, strlen(buffer));
           if (ecode == -1)
           {
               perror("probleme attendu 150");
               exit(1);
           }

           printf("Recu du serveur: \n");
           while (read(modePassif, buffer, MAXBUFFERLEN - 1) != 0){// boucle afin de lire l'entierete du ls
               // lecture de donnees du serveur
               ecode = read(modePassif, buffer, MAXBUFFERLEN - 1);
               if (ecode == -1)
               {
                   perror("probleme de lecture");
                   exit(1);
               }
               buffer[ecode] = '\0';
               printf("%s", buffer);

               // envoie des donnees au client
               ecode = write(modeActif, buffer, strlen(buffer));
               if (ecode == -1)
               {
                   perror("probleme d'envoie de donnees");
                   exit(1);
               }
           };

           close(modeActif);
           close(modePassif);





        // ecode = read(descSockCOM, buffer, MAXBUFFERLEN - 1);
        // if (ecode == -1) {
        //   perror("Erreur de lecture serveur\n");
        //   exit(3);
        // }

        //printf("%s\n", buffer );





    //Fermeture de la connexion
    close(descSockCOM);
    close(descSockRDV);
}
