#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 

#define PORT	 8080 
#define MAXLINE 1024 

/***UDP CLIENT TEST***/
//TODO: Chnage main to function

int main() { 
	int recvMsg, len; 
	int sockfd; 
	char buffer[MAXLINE]; 
	char *string = "Hello from client\n"; 	//TODO: DELETE, only for testing
	struct sockaddr_in	 servaddr; 

	// Creating socket file descriptor 
	if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) { 
		perror("socket"); 
		exit(0); 
	} 

	memset(&servaddr, 0, sizeof(servaddr)); 
	
	//connect to netcat
	servaddr.sin_family = AF_INET; 
	servaddr.sin_port = htons(PORT); 
	servaddr.sin_addr.s_addr = INADDR_ANY; 
	
	sendto(sockfd, (const char *)string, strlen(string), 
		MSG_CONFIRM, (const struct sockaddr *) &servaddr, 
			sizeof(servaddr)); 
	printf("Hello message sent\n"); 
		


	close(sockfd); 
	return 0; 
} 

