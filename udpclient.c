#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 

#define PORT	 8080 
#define SIZE 1024 


struct sockaddr_in servaddr; 
int sockfd; 

void send_message(char *file_name, char *event_name, char *time_stamp)
{
    if(connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
    {
        printf("\n Error : Connect Failed \n");
        exit(0);
    }

	char msg[SIZE];
	sprintf(msg, "FILE ACCESSED: %s\nACCESS: %s\nTIME OF ACCESS: %s\n", file_name, event_name, time_stamp);
	sendto(sockfd, (const char *)msg, strlen(msg), MSG_CONFIRM, (const struct sockaddr *) &servaddr, sizeof(servaddr)); 
	printf("message sent\n"); 
}


int udp() { 

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
		
	return 0; 
} 

