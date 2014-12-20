/* A simple server in the internet domain using TCP
   The port number is passed as an argument */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/select.h>

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[])
{
     int sockfd, newsockfd, portno;
     socklen_t clilen;
     char buffer[256];
     struct sockaddr_in serv_addr, cli_addr;
     int n;
     if (argc < 2) {
         fprintf(stderr,"ERROR, no port provided\n");
         exit(1);
     }
     sockfd = socket(AF_INET, SOCK_STREAM, 0);
     if (sockfd < 0)
        error("ERROR opening socket");
     bzero((char *) &serv_addr, sizeof(serv_addr));
     portno = atoi(argv[1]);
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;
     serv_addr.sin_port = htons(portno);
     if (bind(sockfd, (struct sockaddr *) &serv_addr,
              sizeof(serv_addr)) < 0)
              error("ERROR on binding");
     listen(sockfd,5);
     clilen = sizeof(cli_addr);

     /**
      * The below code is refrenced from beej socket programming
      * example
      * http://beej.us/guide/bgnet/output/html/multipage/advanced.html
      */

     fd_set master;    // master file descriptor list
     fd_set read_fds;  // temp file descriptor list for select()
     int fdmax;
     int newfd;
     int i;

     FD_ZERO(&master);
     FD_ZERO(&read_fds);

     // Add the listener socket sockfd to masterset
     FD_SET(sockfd, &master);
     // Keep track if the biggest file descriptor
     fdmax = sockfd;

     for (;;){
         // Copy it
         read_fds = master;
         if (select(fdmax+1, &read_fds, NULL, NULL, NULL)==-1){
             error("ERROR on select");
         }

         // Run through the existing connection looking for fd to read
         for (i=0; i<=fdmax; i++){
             bzero(buffer,256);
             if (FD_ISSET(i, &read_fds)){
                 // We have one to read
                 if (i==sockfd){
                     // Handle new connection
                     newsockfd = accept(sockfd,
                        (struct sockaddr *) &cli_addr,
                        &clilen);
                     if (newsockfd<0)
                         error("ERROR on accept");
                     else {
                         // Add to master set
                         FD_SET(newsockfd, &master);
                         // Keep track of maximum
                         if (newsockfd > fdmax)
                             fdmax = newsockfd;
                     }
                 }
                 else {
                     // Handle data from the client
                     if (n=recv(i, buffer, sizeof(buffer), 0)<=0){
                         if (n==0) {
                             // Connectin is closed
                             fprintf(stdout, "select server: socket %d hung up\n", i);
                         }
                         else{
                             //perror("");
                         }
                         close(i);
                         FD_CLR(i, &master);
                     }
                     else {
                         fprintf(stdout, "Here is the message: %s\n",buffer);
                         fflush(stdout);
                         if (send(i, "I got the message", 18, 0) == -1)
                             perror("ERROR on send");
                     }
                 } // data send else
             } // fd is ready to read
         } // Iterate all the fds
     } // Infinite loop
    return 0;
}
