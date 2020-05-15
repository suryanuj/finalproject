//Name of file: main.c
//Name of author: Shaun Gupta
//Date: May 18, 2020
//Description of file: Client side for the final project.

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#define BUFFER 4096

void error(char*msg){
	perror(msg);
	exit(0);
}
//This function takes in the website url as an argument, and whatever you enter as the "message", that thing gets searched and the result's page gets printed out as a html page. 

int main(int argc, char* argv[]){
        int sockfd, portno,n;
        struct sockaddr_in serv_addr;
        struct hostent*server;
        char buffer[BUFFER];

        if (argc != 3){
                fprintf(stderr,"usage %s hostname port\n", argv[0]);
                exit(0);
        }

	portno =atoi(argv[2]);
	
        sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (sockfd<0)
                error("ERROR openingsocket");
	server =gethostbyname(argv[1]);
	if(server== NULL){ 
		fprintf(stderr,"ERROR,no suchhost\n");
		exit(0);
	}  

	bzero((char *) &serv_addr,sizeof(serv_addr));
	serv_addr.sin_family =AF_INET;
	bcopy((char *)server->h_addr, (char*)&serv_addr.sin_addr.s_addr, server->h_length);  
	serv_addr.sin_port =htons(portno);

	if(connect(sockfd, (const struct sockaddr *) &serv_addr, sizeof(serv_addr))<0)
		error("ERRORconnecting");

	printf("Please enter the message: ");  
	bzero(buffer,BUFFER);fgets(buffer,BUFFER-1,stdin);  
	n =send(sockfd,buffer,strlen(buffer),0);
	if (n< 0)
		error("ERROR writing to socket");  
		bzero(buffer,BUFFER);
		n = recv(sockfd,buffer,BUFFER-1,0);  
	if (n <0)
		error("ERROR reading from socket");  
	printf("%s\n",buffer);
	close(sockfd);  
	return 0;
}


