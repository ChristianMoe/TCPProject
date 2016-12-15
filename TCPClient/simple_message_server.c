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
#include <netdb.h>
#include <unistd.h>
#include <errno.h>

/*
 * -------------------------------------------------------------- prototypes --
 */
static void usageinfo(FILE *outputdevice, const char *filename, int status);
void parsecommandline(int argc, const char * argv[], long int *port);

/*
 * -------------------------------------------------------------- defines --
 */

#define DEBUG 1  /* 1 for debugging mode, 0 if it is turned off */

/*
 * -------------------------------------------------------------- global resource variables --
 */

const char* argv0; /* necessary for verbose for not handing parameter to every function */


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

int main(int argc, const char * argv[]) {

	/* define the program variables */
		long int *port=0;
	/* end of variable definition */

		argv0=argv[0]; /*copy prog name to global variable*/

		parsecommandline(argc, argv, port);

		if (DEBUG){
			fprintf(stdout,"%s [%s, %s(), line %d]: Using the following options: port=\"%ld\"\n", argv[0],__FILE__, __func__ ,__LINE__,*port);
			}

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




void parsecommandline(int argc, const char * argv[], long int *port){

	int opt=0;
	char **endptr=NULL;

	/* checking whether -h is a parameter of command line */
			while ((opt = getopt(argc,(char **) argv, "ph:")) != -1) {
				switch (opt) {
				case 'p':
					errno = 0;    /* To distinguish success/failure after call */
					*port=strtol(optarg, endptr, 10);
					/* Check for various possible errors */
					           if ((errno == ERANGE && (*port == LONG_MAX || *port == LONG_MIN))
					                   || (errno != 0 && *port == 0)) {
					               perror("strtol failed!");
					               exit(EXIT_FAILURE);
					           }
					           if (endptr == str) {
					               fprintf(stderr, "No digits were found\n");
					               exit(EXIT_FAILURE);
					           }
					break;
		        case 'h':
		        	usageinfo(stdout,argv[0],EXIT_SUCCESS);
		        	break;
		        default:
		        	usageinfo(stderr,argv[0],EXIT_FAILURE);
		        	break;
				}
			} /* end of while */
}
