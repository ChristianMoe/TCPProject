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
char* parsecommandline(int argc, const char * argv[]);
void handle_error(const char *msg);
void print_verbose(const char *msg);
int createsocket(const char* port);
int acceptConnectRequest(int listen_sock_fd);

/*
 * -------------------------------------------------------------- defines --
 */

#define DEBUG 1  /* 1 for debugging mode, 0 if it is turned off */

#define MY_SOCK_PATH "/home/ic15b039/TCPProject/TCPServer" /* wofür das?*/
#define LISTEN_BACKLOG 50

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

int createsocket(const char* port){

/* variables for socket */
	struct addrinfo hints,*result, *respointer; /*parameters for getaddrinfo and socket/connect*/
	int gea_ret; /* variable for getaddrinfo return */
	int socketdescriptor;

/* Obtain address matching host/port */
	memset(&hints, 0, sizeof(struct addrinfo)); /* allocate memory and set all values to 0 */
	hints.ai_family = AF_UNSPEC;    /* Allow IPv4 or IPv6 */
	hints.ai_socktype = SOCK_STREAM; /* TCP Stream socket */
	hints.ai_protocol = 0;	/* Any protocol */
	hints.ai_flags = AI_PASSIVE;

/*
 * getaddrinfo return == 0 if success, otherwise Error-Code
 * and fills results with
 */

	if ((gea_ret=getaddrinfo(NULL, port, &hints, &result))!= 0) handle_error("Get Address Info: ");
	if (DEBUG) print_verbose("getaddrinfo() successful!\n");

/* Info: getaddrinfo() returns a list of address structures.
              Try each address until successfully connected.
              If socket (or connect) fails, close the socket
              and try the next address.

   struct addrinfo {
         int              ai_flags;
         int              ai_family;
         int              ai_socktype;
         int              ai_protocol;
         size_t           ai_addrlen;
         struct sockaddr *ai_addr;
         char            *ai_canonname;
         struct addrinfo *ai_next;
     }; */

	respointer = result;

	socketdescriptor=socket(respointer->ai_family, respointer->ai_socktype,respointer->ai_protocol);
	if (DEBUG) print_verbose("Socket successfully created!\n");
	if (socketdescriptor == -1) handle_error("Could not connect to socket: ");
    freeaddrinfo(result); 		/* result of getaddrinfo no longer needed */

    return socketdescriptor; /*returns socketdescriptor on successfully executed subroutine*/
}

int main(int argc, const char * argv[]) {

	/* define the program variables */
		char *port=0;
		int optval=1;
		int listen_sock_fd;
		struct sockaddr_in listen_sock_addr;
		int connections=0;

	/* end of variable definition */

		argv0=argv[0]; /*copy prog name to global variable*/

		port=parsecommandline(argc, argv);

		if (port==NULL){
			fprintf(stderr,"%s [%s, %s(), line %d]: no valid arguments in command line found!\n", argv[0],__FILE__, __func__ ,__LINE__);
        	usageinfo(stderr,argv[0],EXIT_FAILURE);
		}

		if (DEBUG) print_verbose("successfully parsed port value!");

		memset(&listen_sock_addr, 0, sizeof(struct sockaddr_in)); /* Clear structure */
		listen_sock_addr.sin_family=AF_INET;
		listen_sock_addr.sin_port=htons(6816);
		listen_sock_addr.sin_addr.s_addr = htonl(INADDR_ANY); /* set address to any interface */

		listen_sock_fd = socket(AF_INET, SOCK_STREAM, 0);
		setsockopt(listen_sock_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof optval);

		if ((bind(listen_sock_fd, (struct sockaddr *)&listen_sock_addr, sizeof(struct sockaddr))) == -1) handle_error("Bind: ");
		else print_verbose("Successfully bound to socket!");

		if (listen(listen_sock_fd, LISTEN_BACKLOG) == -1) handle_error("Listen: ");
		else print_verbose("Listening on socket!");

		while (connections<=LISTEN_BACKLOG){

			acceptConnectRequest(listen_sock_fd);

		}

	return 0;
}


int acceptConnectRequest(int listen_sock_fd){

	int connected_sock_fd;
	struct sockaddr_in conneted_sock_addr;

	if ((connected_sock_fd = accept(listen_sock_fd, (struct sockaddr*)&conneted_sock_addr, (socklen_t*) sizeof(struct sockaddr))) == -1)
		error_handle("Connect: ");
	if (DEBUG) print_verbose("Successfully connected to client!\n");

	return 0;
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




char* parsecommandline(int argc, const char * argv[]){

	int opt=0;
	char *tmp=NULL;

	/* checking whether -p <value> or -h is a parameter of command line */
			while ((opt = getopt(argc,(char **) argv, "p:h")) != -1) {
				switch (opt) {
				case 'p':
					tmp=optarg;
					break;
		        case 'h':
		        	usageinfo(stdout,argv[0],EXIT_SUCCESS);
		        	break;
		        default:
		        	usageinfo(stderr,argv[0],EXIT_FAILURE);
		        	break;
				}
			} /* end of while */

	return tmp;

}
