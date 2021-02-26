#include <unistd.h>
#include <stdio.h>
#include "inotify.c"

int main(int argc, char* argv[])
{
	if(argc < 2)
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

	watch(target);

	return 1;
}
