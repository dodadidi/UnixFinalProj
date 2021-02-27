#include<stdio.h>
#include<sys/inotify.h>
#include<unistd.h>
#include<stdlib.h>
#include<signal.h>
#include <time.h>
#include "udpclient.c"
 
#define EVENTS_SIZE 1024  
#define EVENT_SIZE  ( sizeof (struct inotify_event) ) //event's size 
#define BUFFER_SIZE ( EVENTS_SIZE * ( EVENT_SIZE + 20 )) //buffer of event's data

#define HTML_FILE "/var/www/html/index.html"

int fd,wd;

// delete data from html file
void reinit_html(char* dir){
    FILE *file = fopen(HTML_FILE, "w");
	if(file ==NULL){
	perror("fopen");
	exit(0);
}
    fprintf(file, "<head><title>Unix Project - Adi & Or</title></head>");
    fprintf(file, "<h1>My File System Monitor</h1><h2>Directory: %s </h2>", dir);
    fclose(file);
}

//print to html file: file name, purpose of access and time stamp 
void print_to_file(char *file_name, char *event_name, char *time_stamp) {
    FILE *file = fopen(HTML_FILE, "a+");
    fprintf(file, "<h2>\t******</h2>");
    fprintf(file, "<h2> File Name: %s </h2><h2>Purpose: %s<h2>Time Stamp: %s</h2>", file_name, event_name,time_stamp);
    fclose(file);
}

// print the time of the event 
char* time_stamp()	
{
    char* timebuffer;
	timebuffer = (char*)malloc(40*sizeof(char));
	time_t rawtime;
	struct tm *info;
	time( &rawtime );
	info = localtime( &rawtime );
	strftime(timebuffer, 40, "%c",info);
	printf("at %s ", timebuffer);
    return timebuffer;
}


void sig_handler(int sig)
{
       inotify_rm_watch( fd, wd ); //remove watch
       close( fd ); 
       exit( 0 ); 
}
 
 
void* watch(char* directory_path)
{
       signal(SIGINT,sig_handler);
  
       fd = inotify_init();  // initialize inotify 
 
       wd = inotify_add_watch(fd,directory_path, IN_MODIFY | IN_ACCESS); // add watch
 
       if(wd==-1){
               printf("Could not watch : %s\n",(char*)directory_path);
		exit(0);
       }
       else{
              printf("Watching : %s\n",(char*)directory_path);
       }
 
	reinit_html((char*)directory_path);
	udp();
        int i,length;
        char buffer[BUFFER_SIZE];
        
        while(1){
 
		i = 0;
 
            length = read(fd,buffer,BUFFER_SIZE); // read buffer                    
            while(i<length)                        //handel event
            {                   
                struct inotify_event *event = (struct inotify_event *) &buffer[i];
 
                if (event->len)
                {

                    char* timebuffer = time_stamp();
                    if (!(event->mask & IN_ISDIR)) {
                        if (event->mask & IN_MODIFY) {
                            print_to_file(event->name, "write", timebuffer);
				send_message(event->name, "write", timebuffer);

                        }
                        else if (event->mask & IN_ACCESS) {
                            print_to_file(event->name, "read", timebuffer);
				send_message(event->name, "read", timebuffer);

                        }
                    }
                    i += EVENT_SIZE + event->len;
                }
            }
        }
}
