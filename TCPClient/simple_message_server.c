/**
 * @file simple_message_server.c
 * Distributed Systems - Implementation of a simple message message client
 *
 * simple_message_server (VCS  TCP/IP  message  bulletin board server) is the daemon program for the
 * simple_message_client(1) client program.  simple_message_server  listens  on  TCP  port  port  for
 * incoming  connections  from  simple_message_client(1) applications.  simple_message_server forks a
 * new daemon for each incoming connection, redirects stdin and stdout of the forked child daemons to
 * the  connected  socket  and  executes  the simple_message_server_logic(1) executable in the forked
 * child daemons.
 *
 *
 * @author Christian Moedlhammer,ic14b027
 * @author Harald Partmann,ic15b039
 *
 * @date 2016-16-12
 *
 * @version 0.1
 *
 */

/*
 * -------------------------------------------------------------- includes --
 */
#include <stdio.h> /* Include Standard Input/Output Features */
#include <stdlib.h> /* Include Standard Library Functions */
#include <string.h> /* Include String Functions */
#include <getopt.h> /* Include Function for parsing -h */


#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h> /* definitions for UNIX domain sockets --> socket.h sollte ausreichen? */

#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#include <limits.h>



/*
 * -------------------------------------------------------------- prototypes --
 */
static void usageinfo(FILE *outputdevice, const char *filename, int status);
unsigned long int parsecommandline(int argc, const char * argv[]);
void handle_error(const char *msg);
void print_verbose(const char *msg);

/*
 * -------------------------------------------------------------- defines --
 */

#define DEBUG 1  /* 1 for debugging mode, 0 if it is turned off */

#define LISTEN_BACKLOG 50
#define SMSNAME "simple_message_server_sdlogic"
#define SMSPATH "/usr/local/bin/simple_message_server_logic"

/*
 * -------------------------------------------------------------- global resource variables --
 */

const char* argv0; /* necessary for DEBUG for not handing parameter to every function */


/**
 *
 * \brief The main routine for the Client
 *
 * This is the main entry point for the simple_message_client
 *
 * \param argc the number of arguments
 * \param argv the arguments from the command line (including the program name in argv[0])
 *
 * \return always "success"
 * \retval 0 always
 *
 */


void handle_error(const char *msg){
	perror(msg);
	exit(EXIT_FAILURE);
}


void print_verbose(const char *msg){
	fprintf(stdout,"%s [%s, %s(), line %d]: %s\n", argv0,__FILE__, __func__ ,__LINE__,msg);
}


int main(int argc, const char * argv[]) {

	/* define the program variables */
	 	unsigned long int port=0;
		int optval=1;
		int listen_sock_fd, connected_sock_fd;
		struct sockaddr_in listen_sock_addr, conneted_sock_addr;
		pid_t child_pid;
		socklen_t conneted_sock_addr_len;


	/* end of variable definition */

		argv0=argv[0]; /*copy prog name to global variable*/

		port=parsecommandline(argc, argv);
		if ((port>0)&&(DEBUG)) print_verbose("successfully parsed port value!");

		memset(&listen_sock_addr, 0, sizeof(struct sockaddr_in)); /* Clear structure */
		listen_sock_addr.sin_family=AF_INET;
		listen_sock_addr.sin_port=htons(port);
		listen_sock_addr.sin_addr.s_addr = htonl(INADDR_ANY); /* set address to any interface */

		listen_sock_fd = socket(AF_INET,SOCK_STREAM,0);
		setsockopt(listen_sock_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof optval);

		if ((bind(listen_sock_fd, (struct sockaddr *)&listen_sock_addr, sizeof(struct sockaddr))) == -1) handle_error("Bind: ");

		if (DEBUG) print_verbose("Successfully bound to socket!");

		if (listen(listen_sock_fd, LISTEN_BACKLOG) == -1) {
			if (close(listen_sock_fd)==-1) handle_error("Close listen socket: ");
			handle_error("Listen: ");
			}

		if (DEBUG) print_verbose("Listening on socket!");

		while (1){

			conneted_sock_addr_len= sizeof(struct sockaddr);

			if ((connected_sock_fd = accept(listen_sock_fd, (struct sockaddr*)&conneted_sock_addr, &conneted_sock_addr_len)) == -1){
				if (close(listen_sock_fd)==-1) handle_error("Close connected socket: ");
				handle_error("Accept: ");
				}

			if (DEBUG) print_verbose("Successfully connected to client!\n");
			fflush(stdout);


			if((child_pid = fork()) ==-1) {
				if (close(listen_sock_fd)==-1){
					handle_error("Close listen socket: ");
					}
				handle_error("Fork: ");
				}

			/* 0 is for child process */
			if(child_pid == 0){
				if (close(listen_sock_fd)==-1) handle_error("Close listen socket: ");

				if (dup2(connected_sock_fd, STDIN_FILENO)==-1){  /* umleiten stdin */
					if (close(connected_sock_fd)==-1) handle_error("Close connected socket: ");
					handle_error("Dup2 stdin: ");
					}
				if (dup2(connected_sock_fd, STDOUT_FILENO)==-1){  /* umleiten stdout */
					if (close(connected_sock_fd)==-1) handle_error("Close connected socket: ");
					handle_error("Dup2 stout: ");
					}
				if (close(connected_sock_fd)==-1) handle_error("Close connected socket: ");

				if (execlp(SMSPATH,SMSNAME,NULL)==-1) handle_error("Server Logic: ");

				exit(EXIT_SUCCESS);
				} /* end child if */
			/* parent process */
			else{
				if (close(connected_sock_fd)==-1){
					if (close(listen_sock_fd)==-1) handle_error("Close listen socket: ");
					handle_error("Close connected socket: ");
					}

			    }
			} /* end while */

		/* should never be reached */

		if (close(listen_sock_fd)==-1) handle_error("Close listen socket: ");


return 0; /* return 0 on success */

}


/**
 *
 * \brief The usageinfo for the server
 *
 * This subroutine prints the usageinfo for the server and exits
 *
 * \param outputdevice the outputdevice for usage message
 * \param filename the file for which the usage info is valid
 * \param suc_or_fail the exit status
 *
 * \return void
 * \retval none
 *
 */

static void usageinfo(FILE *outputdevice, const char *filename, int status) {

	fprintf(outputdevice, "usage: %s options\n", filename);
	fprintf(outputdevice,"options:\n");
	fprintf(outputdevice,"	-p, --port <port>       well-known port [0..65535]\n");
	fprintf(outputdevice,"	-h, --help\n");

	exit(status);
}




unsigned long int parsecommandline(int argc, const char * argv[]){

	int opt=0;
	//char *tmp=NULL;
	unsigned long int port=0;

	/* checking whether -p <value> or -h is a parameter of command line */
			while ((opt = getopt(argc,(char **) argv, "p:h")) != -1) {
				switch (opt) {
				case 'p':
					port=strtoul(optarg,NULL,10);
					if ((port>65535)) usageinfo(stderr,argv[0],EXIT_FAILURE);
					break;
		        case 'h':
		        	usageinfo(stdout,argv[0],EXIT_SUCCESS);
		        	break;
		        default:
		        	usageinfo(stderr,argv[0],EXIT_FAILURE);
		        	break;
				}
			} /* end of while */

	return port;

}
