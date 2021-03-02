#include <stdio.h>
#include <strings.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include<netinet/in.h>
#include<unistd.h>
#include<stdlib.h>

struct sockaddr_in address;
int sockfd;

void send_message(char *file_name, char *event_name, char *time_stamp)
{
    if(connect(sockfd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        printf("\n Error : Connect Failed \n");
        exit(0);
    }

	char msg[SIZE];
	sprintf(msg, "FILE ACCESSED: %s\nACCESS: %s\nTIME OF ACCESS: %s\n", file_name, event_name, time_stamp);
	sendto(sockfd, (const char *)msg, strlen(msg), MSG_CONFIRM, (const struct sockaddr *) &address, sizeof(address)); 
	printf("message sent\n"); 
}

int udp() 
{ 
	// Creating socket file descriptor 
	if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) 
	{ 
		perror("socket"); 
		exit(0); 
	} 

	memset(&address, 0, sizeof(address)); 

	//connect to netcat
	address.sin_family = AF_INET; 
	address.sin_port = htons(NETCAT_PORT); 
	address.sin_addr.s_addr = INADDR_ANY; 
	return 0; 
} 