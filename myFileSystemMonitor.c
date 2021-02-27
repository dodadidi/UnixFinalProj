#include <unistd.h>
#include <stdio.h>
#include <pthread.h>
#include "inotify.c"

pthread_t thread_inotify;

int main(int argc, char* argv[])
{
	if(argc < 3)
	{
		printf("needed 2 arguments, -d/directory and -iIp, to run the program");
		return 0;
	}
	char* ip;
	char* target;					//target is file path		
	int inp;				
	while((inp = getopt(argc, argv, "d:i:")) != -1)
	{
		switch(inp)
		{
			case 'd':
				target = optarg;
				break;

			case 'i':
				ip = optarg;
				break;
		}
	}
	
	printf("ip is: %s\n", ip);		//TODO: delete
	printf("target is: %s\n", target);	//TODO: delete

	if (pthread_create(&thread_inotify, NULL, watch, target)) //WARNING:  the "target" argument should be void*, it works for now but may be risky
	{
		perror("pthread_create");
	        return 1;
	}
    	pthread_join(thread_inotify, NULL);
	return 1;
}
