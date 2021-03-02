#include<stdio.h>
#include<sys/inotify.h>
#include<unistd.h>
#include<stdlib.h>
#include<signal.h>
#include <time.h>
#include "udpclient.c"


//print to html file: file name, purpose of access and time stamp 
void print_to_file(char *file_name, char *event_name, char *time_stamp)
 {
    FILE *file = fopen(HTML_FILE, "a+");
    fprintf(file, "<h2>\t******</h2>");
    fprintf(file, "<h2> File Name: %s </h2><h2>Purpose: %s<h2>Time Stamp: %s</h2>", file_name, event_name,time_stamp);
    fclose(file);
}


// delete data from html file
void reinit_html(char* dir)
{
    FILE *file = fopen(HTML_FILE, "w");
	if(file ==NULL)
	{
	perror("fopen");
	exit(0);
	}
    fprintf(file, "<head><title>Unix Project - Adi & Or</title></head>");
    fprintf(file, "<h1>My File System Monitor</h1><h2>Directory: %s </h2>", dir);
    fclose(file);
}

// print the time of the event 
char* time_stamp()	
{
	char* timebuffer;
	time_t rawtime;
	timebuffer = (char*)malloc(40*sizeof(char));
	struct tm *info;
	time( &rawtime );
	info = localtime( &rawtime );
	strftime(timebuffer, 40, "%c",info);
	printf("at %s ", timebuffer);
    return timebuffer;
}

int fd, watch_dir;

void sig_handler(int sig)
{
       inotify_rm_watch( fd, watch_dir ); //remove watch
       close( fd ); 
       exit( 0 ); 
}


void* watch (void *args)
{	
	signal(SIGINT,sig_handler);
    char* directory_path = ((parameters *)args)->target_dir;

    fd = inotify_init();
    watch_dir = inotify_add_watch(fd, directory_path, IN_MODIFY | IN_ACCESS);

    if (watch_dir == -1)
	{
        printf("Could not watch : %s\n", directory_path);
		exit(0);
    } 
	else 
	{
        printf("Watching: %s\n", directory_path);
    }
	reinit_html(directory_path);
	udp();
    int i, length;
    char buffer[BUFFER_SIZE];

    while (1) 
	{
        i = 0;

        length = read(fd, buffer, BUFFER_SIZE);

        while (i < length) 
		{
            struct inotify_event *event = (struct inotify_event *) &buffer[i];

            if (event->len)
			{
                char* timebuffer = time_stamp();
                if(!(event->mask & IN_ISDIR))
					{
                    if (event->mask & IN_MODIFY)
						{
                        print_to_file(event->name, "modified", timebuffer);
						send_message(event->name, "write", timebuffer);
                    } 
					else if (event->mask & IN_ACCESS) 
					{
                        print_to_file(event->name, "read", timebuffer);
						send_message(event->name, "read", timebuffer);
                    }
                }
            }
            i += EVENT_SIZE + event->len;
        }
    }
}