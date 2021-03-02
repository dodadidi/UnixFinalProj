#define _GNU_SOURCE
#define BT_BUF_SIZE 200
#define BT_LINE_SIZE 200
#define EVENT_SIZE  ( sizeof (struct inotify_event) ) //event's size
#define EVENT_CAP 1024
#define BUFFER_SIZE     ( EVENT_CAP * ( EVENT_SIZE + 20 )) //buffer of event's data
#define HTML_FILE "/var/www/html/index.html"
#define NETCAT_PORT 5555
#define SIZE 1024
#define TELNET_PORT 8080
#ifdef __GNUC__
#define UNUSED(d) d __attribute__((unused))
#else
#define UNUSED(d) d
#endif
