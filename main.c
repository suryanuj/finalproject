// Name: Aiden Crilley & Shaun Gupta
// Date: 5-18-20
// Project: Final Project
// File: main.c

#include <stdio.h>
#include <sense/sense.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <linux/input.h>
#include <time.h>
#define BUFFER 4096


void error(char* msg) {
	perror(msg);
	exit(0);
}

//This function creates the server
int CreateServer(int port) {
	int sockfd,newsockfd,clilen;
	struct sockaddr_in serv_addr,cli_addr;

	// Creating a socket
	sockfd = socket(AF_INET,SOCK_STREAM,0);
	if (sockfd < 0) {
		error("ERROR opening socket");
	}

	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(port);

	// Binding the created socket to the server
	if (bind(sockfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0) {
		error("ERROR on binding");
	}

	listen(sockfd, 5);
	clilen = sizeof(cli_addr);

	// Allows the socket to listen to the client
	newsockfd = accept(sockfd, (struct sockaddr*) &cli_addr, &clilen);
	if (newsockfd < 0) {
		error("ERROR connecting socket to client");
	}

	return newsockfd;
}

//This function creates the recv server
int RecvServer(int sock) {
	// Receives a GET request from the client
	int n;
	char buffer[4096];

	bzero(buffer,4096);
	n = recv(sock, buffer, 4095, 0);

	return n;
}

//This function sends the server
int SendServer(int sock, char* buffer) {
	int n;

	n = send(sock, buffer, strlen(buffer), 0);

	return n;
}

//Main function, if the command line gets 2 arguments, it executes the server part; if command line gets 3 arguments, it executes the client part.
int main(int argc, char*argv[]) {

	//Defintion of variables
	pi_i2c_t* device=geti2cDevice();
	int red = getColor(255,0,0);
	int green = getColor(0,255,0);
	int blue = getColor(0,0,255);
	int blank = getColor(0,0,0);
	pi_framebuffer_t* fb=getFBDevice();

	//Clears Map
	clearBitmap(fb->bitmap,blank);

	//If the command line has 2 arguments, then it executes the server code. 
	if (argc == 2) {
		int newsockfd,n,portno;
		float temp,humidity,pressure;
		char sendInfo[2096];
		char buff[4096];

		portno = atoi(argv[1]);

		// Create a server with the given port number
		newsockfd = CreateServer(portno);

		// Server receiving a GET request from the client
		n = RecvServer(newsockfd);
		if (n < 0) {
			printf("Couldn't receive message");
		}

		if (device) {
			configureHumiditySensor(device);
			temp = getTemp(device);
			humidity = getHumidity(device);
			pressure = getPressure(device);
		}

		bzero(buff,4096);

		// Concatenating the information to the buffer that will be sent to the client
		gcvt(temp,2,sendInfo);
		strcat(buff, "Temperature: ");
		strcat(buff, sendInfo);
		strcat(buff, "\n");
		strcat(buff, "Humidity: ");
		gcvt(humidity,2,sendInfo);
		strcat(buff, sendInfo);
		strcat(buff, "\n");
		strcat(buff, "Pressure: ");
		gcvt(pressure,2,sendInfo);
		strcat(buff, sendInfo);

		// Sending information to the client
		n = SendServer(newsockfd, buff);

		clearBitmap(fb->bitmap,blank);

		//Below is to make the “S” for sent message		
		setPixel(fb->bitmap,0,0,red);
		setPixel(fb->bitmap,1,0,red);
		setPixel(fb->bitmap,2,0,red);
		setPixel(fb->bitmap,0,1,red);
		setPixel(fb->bitmap,0,2,red);
		setPixel(fb->bitmap,1,2,red);
		setPixel(fb->bitmap,2,2,red);
		setPixel(fb->bitmap,2,3,red);
		setPixel(fb->bitmap,2,4,red);
		setPixel(fb->bitmap,1,4,red);
		setPixel(fb->bitmap,0,4,red);


		freei2cDevice(device);
		freeFrameBuffer(fb);
		close(newsockfd);
	}
	
	//If the command line has 3 arguments, then it executes the client code. 
	else if (argc == 3){
		int sockfd, portno, n;
        	struct sockaddr_in serv_addr;
        	struct hostent*server;
        	char buffer[BUFFER];

        	if (argc != 3){
                	fprintf(stderr,"usage %s hostname port\n", argv[0]);
                	exit(0);
       	 		}

		portno = atoi(argv[2]);

        	sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

        	if (sockfd < 0)
                	error("ERROR opening socket");

		server = gethostbyname(argv[1]);

		if (server== NULL){
			fprintf(stderr,"ERROR,no suchhost\n");
			exit(0);
		}

		bzero((char *) &serv_addr,sizeof(serv_addr));
		serv_addr.sin_family =AF_INET;
		bcopy((char *)server->h_addr, (char*)&serv_addr.sin_addr.s_addr, server->h_length);
		serv_addr.sin_port =htons(portno);

		if (connect(sockfd, (const struct sockaddr *) &serv_addr, sizeof(serv_addr))<0)
			error("ERROR connecting");

		printf("Please press enter key to display information ");
		bzero(buffer,BUFFER);fgets(buffer,BUFFER-1,stdin);
		n = send(sockfd,buffer,strlen(buffer),0);

		if (n < 0)
			error("ERROR writing to socket");
			bzero(buffer,BUFFER);
			n = recv(sockfd,buffer,BUFFER-1,0);

		if (n < 0)
			error("ERROR reading from socket");

		printf("%s\n",buffer);

		clearBitmap(fb->bitmap,blank);

		//Below is to make the “R” for received message
		setPixel(fb->bitmap,0,0,blue);
		setPixel(fb->bitmap,1,0,blue);
		setPixel(fb->bitmap,2,0,blue);
		setPixel(fb->bitmap,2,1,blue);
		setPixel(fb->bitmap,2,2,blue);
		setPixel(fb->bitmap,1,2,blue);
		setPixel(fb->bitmap,0,2,blue);
		setPixel(fb->bitmap,0,1,blue);
		setPixel(fb->bitmap,0,2,blue);
		setPixel(fb->bitmap,0,3,blue);
		setPixel(fb->bitmap,0,4,blue);
		setPixel(fb->bitmap,0,5,blue);
		setPixel(fb->bitmap,1,3,blue);
		setPixel(fb->bitmap,2,4,blue);
		setPixel(fb->bitmap,3,5,blue);

		freeFrameBuffer(fb);
		freei2cDevice(device);	
		close(sockfd);
		}
	freeFrameBuffer(fb);
	return 0;
}
