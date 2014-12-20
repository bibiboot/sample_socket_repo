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

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

void SigCatcher(int n){
    wait3(NULL, WNOHANG, NULL);
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
     sockfd = socket(AF_INET, SOCK_DGRAM, 0);
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
     signal(SIGCHLD, SigCatcher);
     int fromlen = sizeof(struct sockaddr_in);
     struct sockaddr_in from;
     while(1){
        bzero(buffer,256);
        int n=recvfrom(sockfd, buffer, 1024, 0, (struct sockaddr *)&from, &fromlen);
        if (n < 0)
            error("ERROR on recvfrom");
        fprintf(stdout, "Here is the message: %s\n",buffer);
        fflush(stdout);

        n = sendto(sockfd, "I got your message", 18, 0, (struct sockaddr *)&from, fromlen);
        if (n < 0) error("ERROR on sendto");
    }
    close(sockfd);
    return 0;
}
