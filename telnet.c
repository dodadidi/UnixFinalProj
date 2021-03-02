#include <stdio.h>
#include <libcli.h>
#include <signal.h>
#include <limits.h>
#include <strings.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <errno.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

unsigned int counter = 0;
unsigned int debug_regular = 0;
backtrace_s* backtrace_ptr;

int check_enable(const char *password) 
{
    return !strcmp(password, "topsecret");
}

int check_auth(const char *username, const char *password) 
{
    if (strcmp(username, "fred") != 0 || strcmp(password, "nerk") != 0)
	{		
		return CLI_ERROR;
	}
    return CLI_OK;
}

int regular_callback(struct cli_def *cli) 
{
    counter++;
    if (debug_regular) 
	{
        cli_print(cli, "Regular callback - %u times so far", counter);
        cli_reprompt(cli);
    }
    return CLI_OK;
}

int init_backtrace(struct cli_def *cli, UNUSED(const char *command), UNUSED(char *argv[]), UNUSED(int argc)) 
{
    backtrace_ptr->is_active = 1;
    cli_print(cli, "backtrace() returned %d addresses\n", backtrace_ptr->trace_count);
    for (int j = 0; j < backtrace_ptr->trace_count; j++) 
	{
        cli_print(cli, "%s\n", backtrace_ptr->trace[j]);
    }
    sem_post(&telnet_sem);
    return CLI_OK;
}

void init_cli(int sock)
 {
    struct cli_def *cli;
    cli = cli_init();
    cli_set_banner(cli, "Connection established.\nType 'help' to see all the possible commands");
    cli_telnet_protocol(cli, 1);
    cli_regular(cli, regular_callback);
    cli_register_command(cli, NULL, "backtrace", init_backtrace, PRIVILEGE_UNPRIVILEGED, MODE_EXEC, "Executes backtrace");
    cli_set_auth_callback(cli, check_auth);
    cli_set_enable_callback(cli, check_enable);
    cli_loop(cli, sock);
    cli_done(cli);
}


void* telnet (void *args)
{
	int sock, connection;
    backtrace_ptr = ((backtrace_s *)args);
    struct sockaddr_in address;
    int on = 1;
    telnet_th = pthread_self();
    signal(SIGCHLD, SIG_IGN);
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
	{
        perror("socket");
        exit(0);
    }
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on))) 
	{
        perror("setsockopt");
        exit(0);
    }
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    address.sin_port = htons(TELNET_PORT);
    if (bind(sock, (struct sockaddr *)&address, sizeof(address)) < 0) 
	{
        perror("bind");
        exit(0);
    }

    if (listen(sock, 20) < 0) 
	{
        perror("listen");
        exit(0);
    }

    printf("Listening on port %d\n", TELNET_PORT);
    while ((connection = accept(sock, NULL, 0)))
	{
        init_cli(connection);
        exit(0);
    }
    return 0;
}
