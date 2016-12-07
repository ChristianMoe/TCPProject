/**
 * @file simple_message_client.c
 * Distributed Systems - Implementation of a simple message client
 *
 * @author Christian Mödlhammer,ic14b027
 * @details This program implements a simple message client using a Library libsimple_message_client_commandline_handling.a for parsing the command line input
 * @date 2016-01-12
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

#include <simple_message_client_commandline_handling.h> /* Include external Parser Functions */


/*
 * -------------------------------------------------------------- prototypes --
 */
void bindadd(const char **server,const char **port,const char **message);
static void exitCR(int status);
static void usageinfo(FILE *outputdevice, const char *filename, int status);

/*
 * -------------------------------------------------------------- defines --
 */
#define BUF_SIZE 10
#define DEBUG 1 /* 1 ... debugging messages activated, 0 ... normal program mode */

/**
 * closes all open resources
 *
 * \param none
 *
 * \return none
 *
 */

static void exitCR(int status){

	if (DEBUG==1) fprintf(stdout, "Resources cleaned!\n");
	if (DEBUG==1) fprintf(stdout, "Exit status: %d\n",status);
	exit(status);

}

/**
 * prints the usage information
 *
 * \param outputdevice 	specifies outputdevice for usage message
 * \param filename 		filename for which the usage is valid
 * \param suc_or_fail	exit as success or as failure
 *
 * \return none
 *
 */


static void usageinfo(FILE *outputdevice, const char *filename, int status) {

	fprintf(outputdevice, "usage: %s options\n", filename);
	fprintf(outputdevice,"options:\n");
	fprintf(outputdevice,"	-s, --server <server>   full qualified domain name or IP address of the server\n");
	fprintf(outputdevice,"	-p, --port <port>       well-known port of the server [0..65535]\n");
	fprintf(outputdevice,"	-u, --user <name>       name of the posting user\n");
	fprintf(outputdevice,"	-i, --image <URL>       URL pointing to an image of the posting user\n");
	fprintf(outputdevice,"	-m, --message <message> message to be added to the bulletin board\n");
	fprintf(outputdevice,"	-v, --verbose           verbose output\n");
	fprintf(outputdevice,"	-h, --help\n");

	exitCR(status);
}


/**
 * start of main program
 *
 **/


int main(int argc, const char * argv[]) {

	  const char *server = NULL;
	  const char *port = NULL;
	  const char *user = NULL;
	  const char *message = NULL;
	  const char *imgurl = NULL;
	  int verbose = 0;
	  int opt = 0; /* variable for getopt */
	  int buffersize = 0; /* size for sendbuffer */

	  /* calling parsing function for return of command line parameters */
	  smc_parsecommandline(argc, argv, &usageinfo, &server, &port, &user, &message, &imgurl, &verbose);

	  /* checking whether -h is a parameter of command line */
	  while ((opt = getopt(argc,(char **) argv, "h:")) != -1) {
	               switch (opt) {
	               case 'n':
	            	   usageinfo(stdout,argv[0],EXIT_SUCCESS);
	            	   exitCR(EXIT_SUCCESS);
	            	   break;
	               default:
	            	   break;
	               }
	  }

	  /* Print values*/
	  fprintf(stdout, "Server:%s \n", server);
	  fprintf(stdout, "Port:%s \n", port);
	  fprintf(stdout, "User:%s \n", user);
	  fprintf(stdout, "Message:%s \n", message);

	  /* variable for string to be sent */
	  if (imgurl!=NULL) buffersize = (char*) malloc (strlen(user) + strlen(imgurl) + strlen(message) + 20);
	  if (imgurl==NULL) buffersize = (char*) malloc (strlen(user) + strlen(message) + 20);
	  char *sendbuffer = (char*) malloc (buffersize);

	  /* building string to be sent */
	  strcpy(sendbuffer,"user=");
	  strcat(sendbuffer,user);
	  strcat(sendbuffer,"\n");
	  if (imgurl!=NULL) {
		  strcat(sendbuffer,"img=");
		  strcat(sendbuffer,imgurl);
		  strcat(sendbuffer,"\n");
	  }
	  sendbuffer=strcat(sendbuffer,message);
	  fprintf(stdout, "Text to send:%s \n", sendbuffer);

	  const char *finalmessage = sendbuffer;

	  bindadd (&server, &port, & finalmessage);

	  return EXIT_SUCCESS;

}


void bindadd(const char **server,const char **port,const char **message){

          struct addrinfo hints;
          struct addrinfo *result, *rp;
          // void *readbuffer;
          int sfd, s;
          size_t len;
          ssize_t retlen;


          fprintf(stdout, "Server:%s \n", *server);
          fprintf(stdout, "Port:%s \n", *port);
          fprintf(stdout, "Message:%s \n", *message);

          /* Obtain address matching host/port */
          memset(&hints, 0, sizeof(struct addrinfo));
          hints.ai_family = AF_UNSPEC;    /* Allow IPv4 or IPv6 */
          hints.ai_socktype = SOCK_DGRAM; /* Datagram socket */
          hints.ai_flags = 0;
          hints.ai_protocol = 0;          /* Any protocol */

          s = getaddrinfo(*server, *port, &hints, &result);
          if (s != 0) {
              fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
              exit(EXIT_FAILURE);
          }

          /* getaddrinfo() returns a list of address structures.
                        Try each address until we successfully connect(2).
                        If socket(2) (or connect(2)) fails, we (close the socket
                        and) try the next address. */

          for (rp = result; rp != NULL; rp = rp->ai_next) {
          sfd = socket(rp->ai_family, rp->ai_socktype,rp->ai_protocol);
                         if (sfd == -1) continue;

                         if (connect(sfd, rp->ai_addr, rp->ai_addrlen) != -1) break; /* Success */

                         close(sfd);
                     }

              if (rp == NULL) {               /* No address succeeded */
                 fprintf(stderr, "Could not connect\n");
                 exit(EXIT_FAILURE);
              }

              freeaddrinfo(result);           /* No longer needed */

              /* Send message datagram to server */

                  len = strlen(*message) + 1;
                  retlen = (ssize_t) len;

                  if (len + 1 > BUF_SIZE) fprintf(stderr, "Ignoring long message in argument\n");

                  if (write(sfd, *message, len) != retlen) {
                                    fprintf(stderr, "partial/failed write\n");
                                    exit(EXIT_FAILURE);
                                }
                  close(sfd);

}


