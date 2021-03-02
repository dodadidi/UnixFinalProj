#include <sys/inotify.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <pthread.h>
#include <execinfo.h>
#include <semaphore.h>
#include "constants.c"

typedef struct backtrace 
{
    char **trace;
    int trace_count;
    char is_active;
} backtrace_s;

typedef struct parameters
{
  char* target_dir;
  char* ip;
} parameters;

sem_t telnet_sem;
void *backtrace_buffer[BT_BUF_SIZE];
pthread_t inotfiy_th;
pthread_t telnet_th;
backtrace_s back_ptr;

void  __attribute__ ((no_instrument_function)) reset_backtrace () 
{
    free(back_ptr.trace);
    back_ptr.trace = (char**)malloc(0*sizeof(char*));
    back_ptr.trace_count = 0;
    back_ptr.is_active = 0;
}

void  __attribute__ ((no_instrument_function)) collect_backtrace  (int trace_count, char** string)
 {
    back_ptr.trace = (char**)realloc(back_ptr.trace, (back_ptr.trace_count + trace_count) * sizeof(char*));
    for (int h = 0; h < trace_count; h++) 
	{
        back_ptr.trace[back_ptr.trace_count + h] = (char*)malloc(BT_LINE_SIZE*sizeof(char));
        strcpy(back_ptr.trace[back_ptr.trace_count + h], string[h]);
    }
    back_ptr.trace_count += trace_count;
}

void  __attribute__ ((no_instrument_function))  __cyg_profile_func_enter (void *this_fn,  void *call_site)
{
        if(back_ptr.is_active == 1) 
		{
            sem_wait(&telnet_sem);
            reset_backtrace();
        }
        if (!pthread_equal(telnet_th, pthread_self())) 
		{
            int trace_count = backtrace(backtrace_buffer, BT_BUF_SIZE);
            char** string = backtrace_symbols(backtrace_buffer, trace_count);
            collect_backtrace(trace_count, string);
        }

}

#include "inotify.c"
#include "telnet.c"

int main(int argc, char **argv) {
	if(argc < 3)
	{
		printf("needed 2 arguments, -d/directory and -iIp, to run the program");
		exit(0);
	}
	parameters p;
	int inp;				
	while((inp = getopt(argc, argv, "d:i:")) != -1)
	{
		switch(inp)
		{
			case 'd':
				p.target_dir = optarg;
				break;

			case 'i':
				p.ip = optarg;
				break;
		}
	}
	
    signal(SIGINT, sig_handler);
    signal(SIGABRT, sig_handler);
    if (sem_init(&telnet_sem, 0, 0) == -1)
	{
		perror("sem_init");
        return 0;
    }

    if (pthread_create(&inotfiy_th, NULL, watch, (void*)&p))
	{
		perror("pthread_create");
        return 0;
	}
    if (pthread_create(&telnet_th, NULL, telnet, (void*)&back_ptr))
	{
		
        return 0;
	}

    pthread_join(inotfiy_th, NULL);
    pthread_join(telnet_th, NULL);
    sem_close (&telnet_sem);
	return 1;
}








 
